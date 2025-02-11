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

using json = nlohmann::json;

// 全域變數
static std::atomic<bool> server_running(false);
static std::thread server_thread;

// 儲存最新從 MQL4 傳入的未平倉部位資料（全 JSON 格式）
static json g_latestPositions;

// 固定策略參數
const double STOP_LOSS_AMOUNT = 50.0;
const double TAKE_PROFIT_AMOUNT = STOP_LOSS_AMOUNT * 2; // 1:2 盈虧比
const double BASE_ORDER_PNL_RANGE = 100.0;              // 當天實現損益在 ±100 內可接受底單
const double ADD_ORDER_THRESHOLD = 20.0;                // 浮動盈虧大於等於 20 可接受加碼
const double MAX_POSITION_LOTS = 1.0;                   // 每商品最大倉位

//--------------------------------------------------------------
// HTTP 服務器邏輯：接收新訂單信號，並根據最新未平倉資料決定下單類型
//--------------------------------------------------------------
void run_http_server()
{
    httplib::Server svr;

    // HTTP POST /createPosition 接口
    svr.Post("/createPosition", [](const httplib::Request &req, httplib::Response &res)
             {
        try {
            // 解析 HTTP 請求中的新訂單信號 JSON
            json newSignal = json::parse(req.body);
            // newSignal 格式：
            // {
            //   "CommodityId": "XAUUSD",
            //   "Lots": 0.1,
            //   "LongShort": 1,             // 1: Buy, -1: Sell
            //   "NewOrClosedPosition": 1    // 1: New Order, 0: Close Order
            // }

            json retOrder;
            std::string commodity = newSignal.value("CommodityId", "");
            double signalLots = newSignal.value("Lots", 0.0);
            int longShort = newSignal.value("LongShort", 1);
            int newOrClosed = newSignal.value("NewOrClosedPosition", 1);

            // 根據最新未平倉資料判斷：若有該商品的持倉則可能為加碼單，否則為底單
            bool hasPosition = false;
            double totalLots = 0.0;
            double anyFloatingPL = 0.0;
            if (g_latestPositions.contains("OpenPosition") && g_latestPositions["OpenPosition"].is_array())
            {
                for (auto& pos : g_latestPositions["OpenPosition"])
                {
                    if (pos.value("CommodityId", "") == commodity)
                    {
                        hasPosition = true;
                        totalLots += pos.value("Lots", 0.0);
                        anyFloatingPL = pos.value("FloatingProfitLoss", 0.0);
                    }
                }
            }

            // 若 newSignal 要關閉部位，直接回傳關閉指令
            if (newOrClosed == 0)
            {
                retOrder["CommodityId"] = commodity;
                retOrder["Lots"] = signalLots;
                retOrder["OrderType"] = "CloseOrder";
                retOrder["LongShort"] = longShort;
            }
            else
            {
                // 新倉訊號（NewOrClosedPosition == 1）
                if (!hasPosition)
                {
                    // 若無現有持倉，則檢查當天已實現損益是否在範圍內
                    double closedPL = g_latestPositions.value("ClosedProfitLoss", 0.0);
                    if (std::abs(closedPL) <= BASE_ORDER_PNL_RANGE)
                    {
                        // 接收底單：原始新訂單訊號
                        retOrder["CommodityId"] = commodity;
                        retOrder["Lots"] = signalLots;
                        retOrder["OrderType"] = "BaseOrder";
                        retOrder["LongShort"] = longShort;
                    }
                    else
                    {
                        // 不符合底單條件，回傳錯誤訊息
                        retOrder["error"] = "Realized P&L out of acceptable range for base order.";
                    }
                }
                else
                {
                    // 若已有持倉，判斷是否滿足加碼條件
                    if (anyFloatingPL >= ADD_ORDER_THRESHOLD && totalLots < MAX_POSITION_LOTS)
                    {
                        retOrder["CommodityId"] = commodity;
                        // 設定加碼單手數為剩餘可加量（或 signalLots, 視策略而定）
                        double addLots = std::min(signalLots, MAX_POSITION_LOTS - totalLots);
                        retOrder["Lots"] = addLots;
                        retOrder["OrderType"] = "AddOrder";
                        retOrder["LongShort"] = longShort;
                    }
                    else
                    {
                        // 若不滿足加碼條件，回傳提示訊息
                        retOrder["error"] = "Add order conditions not met.";
                    }
                }
            }

            res.set_content(retOrder.dump(), "application/json");
        }
        catch (const std::exception& e)
        {
            json err;
            err["error"] = e.what();
            res.status = 400;
            res.set_content(err.dump(), "application/json");
        } });

    server_running = true;
    // 注意：port 23 是 Telnet 預設埠，部署時請確保無其他服務衝突
    svr.listen("0.0.0.0", 23);
    server_running = false;
}

//--------------------------------------------------------------
// 導出函數：啟動 HTTP 服務器（由 MT4 調用以啟動服務）
extern "C" __declspec(dllexport) void StartHttpServer()
{
    if (!server_running)
    {
        server_thread = std::thread(run_http_server);
    }
}

//--------------------------------------------------------------
// 導出函數：停止 HTTP 服務器（示範用，實際需調用 svr.stop()）
extern "C" __declspec(dllexport) void StopHttpServer()
{
    if (server_thread.joinable())
    {
        // 此處示範 detach，實際應實作停止邏輯
        server_thread.detach();
    }
}

//--------------------------------------------------------------
// 導出函數：由 MQL4 呼叫，傳入最新未平倉部位 JSON資料，並計算對應的停損/停利/加碼單訊號
// 傳入格式（Function get JSON data form MQL4）：
// {
//   "Margin": 10000,
//   "ClosedProfitLoss": 0,
//   "OpenPosition": [
//       { "CommodityId": "XAUUSD", "Lots": 0.1, "FloatingProfitLoss": 10.0 },
//       { "CommodityId": "NQ100", "Lots": 0.1, "FloatingProfitLoss": -5.5 }
//   ]
// }
// 回傳下單訊號 JSON 陣列，格式範例：
// [
//    { "CommodityId": "XAUUSD", "Lots": 0.1, "OrderType": "TakeProfit", "Amount": 100.0 },
//    { "CommodityId": "NQ100", "Lots": 0.1, "OrderType": "StopLoss", "Amount": 50.0 }
// ]
extern "C" __declspec(dllexport) const char *ProcessPositions(const char *jsonInput)
{
    static std::string ret;
    try
    {
        json input = json::parse(jsonInput);
        // 更新全域最新持倉資料，供 HTTP 服務使用
        g_latestPositions = input;

        json orders = json::array();
        if (input.contains("OpenPosition") && input["OpenPosition"].is_array())
        {
            for (auto &pos : input["OpenPosition"])
            {
                std::string commodity = pos.value("CommodityId", "");
                double lots = pos.value("Lots", 0.0);
                double floatingPL = pos.value("FloatingProfitLoss", 0.0);

                // 如果浮動盈虧大於等於 TAKE_PROFIT_AMOUNT，則生成停利單
                if (floatingPL >= TAKE_PROFIT_AMOUNT)
                {
                    json order;
                    order["CommodityId"] = commodity;
                    order["Lots"] = lots;
                    order["OrderType"] = "TakeProfit";
                    order["Amount"] = TAKE_PROFIT_AMOUNT;
                    orders.push_back(order);
                }
                // 如果浮動盈虧小於等於 -STOP_LOSS_AMOUNT，則生成停損單
                else if (floatingPL <= -STOP_LOSS_AMOUNT)
                {
                    json order;
                    order["CommodityId"] = commodity;
                    order["Lots"] = lots;
                    order["OrderType"] = "StopLoss";
                    order["Amount"] = STOP_LOSS_AMOUNT;
                    orders.push_back(order);
                }
                // 如果有持倉且浮動盈虧達到加碼門檻，且該商品總口數未超過最大限制，則生成加碼單
                else if (floatingPL >= ADD_ORDER_THRESHOLD)
                {
                    // 計算該商品總持倉口數
                    double totalLots = 0.0;
                    for (auto &p : input["OpenPosition"])
                    {
                        if (p.value("CommodityId", "") == commodity)
                            totalLots += p.value("Lots", 0.0);
                    }
                    if (totalLots < MAX_POSITION_LOTS)
                    {
                        json order;
                        order["CommodityId"] = commodity;
                        // 加碼單手數可以設定為剩餘可加量，或固定值（此處取剩餘可加量與原始單的最小值）
                        double addLots = std::min(lots, MAX_POSITION_LOTS - totalLots);
                        order["Lots"] = addLots;
                        order["OrderType"] = "AddOrder";
                        order["Amount"] = STOP_LOSS_AMOUNT; // 此處可根據風控策略調整
                        orders.push_back(order);
                    }
                }
                // 底單：此部分由 MQL4 與 HTTP 新訂單訊號處理，這裡僅針對已有持倉生成停利/停損/加碼訊號
            }
        }
        ret = orders.dump();
        return ret.c_str();
    }
    catch (const std::exception &e)
    {
        json err;
        err["error"] = e.what();
        ret = err.dump();
        return ret.c_str();
    }
}
