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

using json = nlohmann::json;

//-----------------------------
// 定義 MQ4 傳入的訂單結構 (加入 CommodityId 欄位)
struct SIMULATED_POSITION
{
    unsigned long long OrderSerialNumber;
    std::string CommodityId;
    double CostPrice;
    double Lots;
    double FloatingPL;
};

//-----------------------------
// 全域變數
static std::atomic<bool> server_running(false);
static std::thread server_thread;

// 使用 ticket 為 key，確保同一個 ticket 只存一筆資料
static std::unordered_map<int, SIMULATED_POSITION> gCurOpenPosition;

// 其他全域變數（例如 HTTP 服務用）
static json g_latestPositions;
static std::mutex g_orderMutex;
static std::string g_newOrder = "";

// 固定策略參數（可根據需求調整）
const double STOP_LOSS_AMOUNT = 50.0;
const double TAKE_PROFIT_AMOUNT = STOP_LOSS_AMOUNT * 2; // 1:2 盈虧比
const double BASE_ORDER_PNL_RANGE = 100.0;              // 當天實現損益在 ±100 內可接受底單
const double ADD_ORDER_THRESHOLD = 20.0;                // 浮動盈虧大於等於 20 可接受加碼
const double MAX_POSITION_LOTS = 1.0;                   // 每商品最大倉位

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
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

//-------------------------------------------
// HTTP 服務器邏輯 (保留，如需與其他系統溝通)
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
            g_newOrder = "";
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
// MQ4 傳入訂單資料：多參數傳遞方式，使用 ticket 為 key
//-----------------------------------------------------------
extern "C" __declspec(dllexport) void GetCurOpenPosition(const char *commodityId, int ticket, double costPrice, double lots, double floatingPL)
{
    SIMULATED_POSITION pos;
    pos.OrderSerialNumber = static_cast<unsigned long long>(ticket);
    pos.CommodityId = std::string(commodityId);
    pos.CostPrice = costPrice;
    pos.Lots = lots;
    pos.FloatingPL = floatingPL;
    // 使用 ticket 作為 key，若已有則更新
    gCurOpenPosition[ticket] = pos;
    std::cout << "[Received Position] Ticket: " << ticket
              << ", Commodity: " << commodityId
              << ", CostPrice: " << costPrice
              << ", Lots: " << lots
              << ", FloatingPL: " << floatingPL << std::endl;
}

//-----------------------------------------------------------
// 處理所有 MQ4 傳入的開倉資料，計算停損/停利/加碼邏輯，並返回下單 JSON
//-----------------------------------------------------------
extern "C" __declspec(dllexport) const char *ProcessSimulatedPositions()
{
    static std::string ret;
    json orders = json::array();

    // 遍歷所有 ticket 的未平倉資料
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
            orders.push_back(order);
            std::cout << "[TakeProfit] Commodity: " << pos.CommodityId
                      << ", OrderSerial: " << pos.OrderSerialNumber
                      << ", Lots: " << pos.Lots
                      << ", Amount: " << TAKE_PROFIT_AMOUNT << std::endl;
        }
        else if (pos.FloatingPL <= -STOP_LOSS_AMOUNT)
        {
            json order;
            order["CommodityId"] = pos.CommodityId;
            order["OrderType"] = "StopLoss";
            order["Lots"] = pos.Lots;
            order["Amount"] = STOP_LOSS_AMOUNT;
            order["OrderSerialNumber"] = pos.OrderSerialNumber;
            orders.push_back(order);
            std::cout << "[StopLoss] Commodity: " << pos.CommodityId
                      << ", OrderSerial: " << pos.OrderSerialNumber
                      << ", Lots: " << pos.Lots
                      << ", Amount: " << STOP_LOSS_AMOUNT << std::endl;
        }
        // 如有需要，可添加其他邏輯（例如加碼）
    }
    ret = orders.dump();
    std::cout << "[ProcessSimulatedPositions] Orders: " << ret << std::endl;
    return ret.c_str();
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
