// GengYouCfdStrategy.cpp

#include "httplib.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <atomic>
#include <vector>
#include <sstream>
#include <windows.h>
#include <algorithm> // for std::min
#include <ctime>
#include <unordered_map>
#include <cstdio>
#include <cstring>

using json = nlohmann::json;

//-----------------------------
// 定義 MQ4 傳入的訂單結構 (新增 LongShort 欄位：1 = 多頭, -1 = 空頭)
struct SIMULATED_POSITION
{
    unsigned long long OrderSerialNumber;
    std::string CommodityId;
    double CostPrice;
    double Lots;
    double FloatingPL;
    int LongShort; // 1 for long, -1 for short
};

//-----------------------------
// 全域變數
static std::atomic<bool> server_running(false);
static std::thread server_thread;

// 使用 ticket 為 key，確保同一個 ticket 只存一筆資料
static std::unordered_map<int, SIMULATED_POSITION> gCurOpenPosition, gSimulatedPosition;
// 使用 commodityId 為 key 儲存當前 CFD 價格
static std::unordered_map<std::string, double> gCurCfdPrices;

// 其他全域變數（例如 HTTP 服務用）
static std::mutex g_orderMutex;
static std::string g_newOrder = "";

// 固定策略參數（可根據需求調整）
const double STOP_LOSS_AMOUNT = 50.0;
const double TAKE_PROFIT_AMOUNT = STOP_LOSS_AMOUNT * 2; // 1:2 盈虧比
const double BASE_ORDER_PNL_RANGE = 100.0;
const double ADD_ORDER_THRESHOLD = 20.0;
const double MAX_POSITION_LOTS = 1.0;

//-------------------------------------------
// DLL 主入口，開啟 CMD 視窗輸出 log
//-------------------------------------------
BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        std::cout << "Console allocated for logging." << std::endl;
        // 輸出賺賠比數值
        std::cout << "[DllMain] STOP_LOSS_AMOUNT = " << STOP_LOSS_AMOUNT
                  << ", TAKE_PROFIT_AMOUNT = " << TAKE_PROFIT_AMOUNT
                  << " (ratio 1:" << (TAKE_PROFIT_AMOUNT / STOP_LOSS_AMOUNT)
                  << ")" << std::endl;
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

//-------------------------------------------
// HTTP 服務器邏輯 (保留)
//-------------------------------------------
void Receive_Strategy_Server_Signals()
{
    httplib::Server svr;
    svr.Get("/getNewOrder", [](const httplib::Request &req, httplib::Response &res)
            {
        std::lock_guard<std::mutex> lock(g_orderMutex);
        if(g_newOrder.empty())
            res.set_content("{}", "application/json");
        else
        {
            res.set_content(g_newOrder, "application/json");
            g_newOrder.clear();
        } });
    svr.Post("/createPosition", [](const httplib::Request &req, httplib::Response &res)
             {
        try {
            json newSignal = json::parse(req.body);
            json retOrder;
            std::string commodity = newSignal.value("CommodityId", "");
            double signalLots = newSignal.value("Lots", 0.0);
            int longShort = newSignal.value("LongShort", 1);
            int newOrClosed = newSignal.value("NewOrClosedPosition", 1);
            retOrder["CommodityId"] = commodity;
            retOrder["Lots"] = signalLots;
            retOrder["OrderType"] = (newOrClosed == 0) ? "CloseOrder" : "BaseOrder";
            retOrder["LongShort"] = longShort;
            res.set_content(retOrder.dump(), "application/json");
            {
                std::lock_guard<std::mutex> lock(g_orderMutex);
                g_newOrder = retOrder.dump();
            }
        }
        catch (const std::exception& e)
        {
            json err;
            err["error"] = e.what();
            res.status = 400;
            res.set_content(err.dump(), "application/json");
        } });
    server_running = true;
    svr.listen("0.0.0.0", 1688);
    server_running = false;
}

extern "C" __declspec(dllexport) void StartHttpServer()
{
    if (!server_running)
    {
        server_thread = std::thread(Receive_Strategy_Server_Signals);
    }
}

extern "C" __declspec(dllexport) void StopHttpServer()
{
    if (server_thread.joinable())
    {
        server_thread.detach();
    }
}

//-----------------------------------------------------------
// MQ4 傳入商品價格：以 commodityId 為 key 更新 CFD 價格
//-----------------------------------------------------------
extern "C" __declspec(dllexport) void GetCurCfdPrices(const char *commodityId, double commodityCurPrice)
{
    std::string comm(commodityId);
    gCurCfdPrices[comm] = commodityCurPrice;
    std::cout << "[GetCurCfdPrices] Commodity: " << comm
              << " => Price: " << commodityCurPrice << std::endl;
    std::cout << "=== gCurCfdPrices ===" << std::endl;
    for (const auto &kv : gCurCfdPrices)
    {
        std::cout << "  " << kv.first << " => " << kv.second << std::endl;
    }
}

//-----------------------------------------------------------
// 輔助函式：取得指定商品的 CFD 價格
//-----------------------------------------------------------
static double GetCurrentCfdPrice(const std::string &commodityId)
{
    auto it = gCurCfdPrices.find(commodityId);
    if (it != gCurCfdPrices.end())
        return it->second;
    return 0.0;
}

//-----------------------------------------------------------
// MQ4 傳入訂單資料：以 ticket 為 key，多參數傳遞 (包含 LongShort)
// 並輸出完整商品名稱與當前 CFD 價格
//-----------------------------------------------------------
extern "C" __declspec(dllexport) void GetCurOpenPosition(const char *commodityId, int ticket, double costPrice, double lots, double floatingPL, int longShort)
{
    SIMULATED_POSITION pos;
    pos.OrderSerialNumber = static_cast<unsigned long long>(ticket);
    pos.CommodityId = std::string(commodityId);
    pos.CostPrice = costPrice;
    pos.Lots = lots;
    pos.FloatingPL = floatingPL;
    pos.LongShort = longShort;
    gCurOpenPosition[ticket] = pos;
    double cfdPrice = GetCurrentCfdPrice(pos.CommodityId);
    std::cout << "[GetCurOpenPosition] Ticket: " << ticket
              << ", Commodity: " << pos.CommodityId
              << ", CFD Price: " << cfdPrice
              << ", CostPrice: " << costPrice
              << ", Lots: " << lots
              << ", FloatingPL: " << floatingPL
              << ", LongShort: " << longShort << std::endl;
}

//-----------------------------------------------------------
// MQ4 傳入模擬訂單資料：以 ticket 為 key，多參數傳遞 (包含 LongShort)
// 並輸出完整商品名稱與當前 CFD 價格
//-----------------------------------------------------------
extern "C" __declspec(dllexport) void GetSimulatedOpenPosition(const char *commodityId, int ticket, double costPrice, double lots, int longShort)
{
    SIMULATED_POSITION pos;
    pos.OrderSerialNumber = static_cast<unsigned long long>(ticket);
    pos.CommodityId = std::string(commodityId);
    pos.CostPrice = costPrice;
    pos.Lots = lots;
    pos.FloatingPL = 0.0;
    pos.LongShort = longShort;
    gSimulatedPosition[ticket] = pos;
    double cfdPrice = GetCurrentCfdPrice(pos.CommodityId);
    std::cout << "[GetSimulatedOpenPosition] Ticket: " << ticket
              << ", Commodity: " << pos.CommodityId
              << ", CFD Price: " << cfdPrice
              << ", CostPrice: " << costPrice
              << ", Lots: " << lots
              << ", FloatingPL: " << pos.FloatingPL
              << ", LongShort: " << pos.LongShort << std::endl;
}

//-----------------------------------------------------------
// 更新 gSimulatedPosition 的 FloatingPL：公式：FloatingPL = (commodityCurPrice - CostPrice) * Lots * LongShort
//-----------------------------------------------------------
extern "C" __declspec(dllexport) void UpdatedSimulatedOpenPosition(const char *commodityId, double commodityCurPrice)
{
    std::string comm(commodityId);
    bool updated = false;
    for (auto &pair : gSimulatedPosition)
    {
        SIMULATED_POSITION &pos = pair.second;
        if (pos.CommodityId == comm)
        {
            pos.FloatingPL = (commodityCurPrice - pos.CostPrice) * pos.Lots * pos.LongShort;
            std::cout << "[UpdatedSimulatedOpenPosition] Ticket: " << pair.first
                      << ", Commodity: " << pos.CommodityId
                      << ", New FloatingPL: " << pos.FloatingPL << std::endl;
            updated = true;
        }
    }
    if (updated)
    {
        std::cout << "=== gSimulatedPosition ===" << std::endl;
        for (const auto &kv : gSimulatedPosition)
        {
            const SIMULATED_POSITION &p = kv.second;
            std::cout << "  Ticket=" << kv.first
                      << ", CommodityId=" << p.CommodityId
                      << ", CostPrice=" << p.CostPrice
                      << ", Lots=" << p.Lots
                      << ", FloatingPL=" << p.FloatingPL
                      << ", LongShort=" << p.LongShort << std::endl;
        }
    }
}

//-----------------------------------------------------------
// 處理所有 MQ4 傳入的開倉資料，計算停損/停利/加碼邏輯，並返回下單 JSON
//-----------------------------------------------------------
extern "C" __declspec(dllexport) const char *ProcessSimulatedPositions()
{
    static std::string ret;
    json orders = json::array();
    bool stopLossTriggered = false;
    bool takeProfitTriggered = false;
    for (const auto &pair : gCurOpenPosition)
    {
        const SIMULATED_POSITION &pos = pair.second;
        if (pos.FloatingPL >= TAKE_PROFIT_AMOUNT)
        {
            json order;
            order["CommodityId"] = pos.CommodityId;
            order["OrderType"] = "TakeProfit";
            order["Lots"] = pos.Lots;
            order["Amount"] = TAKE_PROFIT_AMOUNT;
            order["OrderSerialNumber"] = pos.OrderSerialNumber;
            order["LongShort"] = pos.LongShort;
            orders.push_back(order);
            takeProfitTriggered = true;
            std::cout << "[TakeProfit] Commodity: " << pos.CommodityId
                      << ", OrderSerial: " << pos.OrderSerialNumber
                      << ", Lots: " << pos.Lots
                      << ", Amount: " << TAKE_PROFIT_AMOUNT
                      << ", LongShort: " << pos.LongShort << std::endl;
        }
        else if (pos.FloatingPL <= -STOP_LOSS_AMOUNT)
        {
            json order;
            order["CommodityId"] = pos.CommodityId;
            order["OrderType"] = "StopLoss";
            order["Lots"] = pos.Lots;
            order["Amount"] = STOP_LOSS_AMOUNT;
            order["OrderSerialNumber"] = pos.OrderSerialNumber;
            order["LongShort"] = pos.LongShort;
            orders.push_back(order);
            stopLossTriggered = true;
            std::cout << "[StopLoss] Commodity: " << pos.CommodityId
                      << ", OrderSerial: " << pos.OrderSerialNumber
                      << ", Lots: " << pos.Lots
                      << ", Amount: " << STOP_LOSS_AMOUNT
                      << ", LongShort: " << pos.LongShort << std::endl;
        }
    }
    ret = orders.dump();
    std::cout << "[ProcessSimulatedPositions] Orders: " << ret << std::endl;
    if (stopLossTriggered || takeProfitTriggered)
    {
        std::cout << "[ProcessSimulatedPositions] => Some SL/TP triggered: ";
        if (stopLossTriggered)
            std::cout << "StopLoss ";
        if (takeProfitTriggered)
            std::cout << "TakeProfit";
        std::cout << std::endl;
    }
    return ret.c_str();
}

//-----------------------------------------------------------
// 返回可執行下單的資訊字串
// 格式: CommodityId,OrderType,Lots,Amount,OrderSerialNumber,LongShort;
extern "C" __declspec(dllexport) std::string GetOrdersForExecution()
{
    std::string ordersStr;
    ordersStr.clear();
    for (const auto &pair : gCurOpenPosition)
    {
        const SIMULATED_POSITION &pos = pair.second;
        if (pos.FloatingPL >= TAKE_PROFIT_AMOUNT)
        {
            ordersStr += pos.CommodityId + ",";
            ordersStr += "TakeProfit,";
            ordersStr += std::to_string(pos.Lots) + ",";
            ordersStr += std::to_string(TAKE_PROFIT_AMOUNT) + ",";
            ordersStr += std::to_string(pos.OrderSerialNumber) + ",";
            ordersStr += std::to_string(pos.LongShort) + ";";
        }
        else if (pos.FloatingPL <= -STOP_LOSS_AMOUNT)
        {
            ordersStr += pos.CommodityId + ",";
            ordersStr += "StopLoss,";
            ordersStr += std::to_string(pos.Lots) + ",";
            ordersStr += std::to_string(STOP_LOSS_AMOUNT) + ",";
            ordersStr += std::to_string(pos.OrderSerialNumber) + ",";
            ordersStr += std::to_string(pos.LongShort) + ";";
        }
    }
    std::cout << "[GetOrdersForExecution] " << ordersStr << std::endl;
    return ordersStr;
}

//-----------------------------------------------------------
// 保留 CustomProcessParameters (僅作示範)
//-----------------------------------------------------------
extern "C" __declspec(dllexport) const char *CustomProcessParameters(
    double margin,
    double closedPL,
    const char *commodityId,
    double lots,
    double floatingPL)
{
    static std::string output;
    try
    {
        json j;
        j["Margin"] = margin;
        j["ClosedProfitLoss"] = closedPL;
        json openPos;
        openPos["CommodityId"] = std::string(commodityId);
        openPos["Lots"] = lots;
        openPos["FloatingProfitLoss"] = floatingPL;
        j["OpenPosition"] = json::array({openPos});
        std::time_t now = std::time(nullptr);
        j["ProcessedTime"] = now;
        output = j.dump();
        return output.c_str();
    }
    catch (const std::exception &e)
    {
        json err;
        err["error"] = e.what();
        output = err.dump();
        return output.c_str();
    }
}
