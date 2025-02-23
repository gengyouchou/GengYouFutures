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

using json = nlohmann::json;

// 全域變數
static std::atomic<bool> server_running(false);
static std::thread server_thread;

// 儲存最新從 MQL4 傳入的未平倉部位資料（全 JSON 格式）
static json g_latestPositions;

// 新增：用來存放最新接收到的訂單訊號 JSON（來自發訊機的 POST 請求）
static std::mutex g_orderMutex;
static std::string g_newOrder = "";

// 固定策略參數
const double STOP_LOSS_AMOUNT = 50.0;
const double TAKE_PROFIT_AMOUNT = STOP_LOSS_AMOUNT * 2; // 1:2 盈虧比
const double BASE_ORDER_PNL_RANGE = 100.0;              // 當天實現損益在 ±100 內可接受底單
const double ADD_ORDER_THRESHOLD = 20.0;                // 浮動盈虧大於等於 20 可接受加碼
const double MAX_POSITION_LOTS = 1.0;                   // 每商品最大倉位

//--------------------------------------------------------------
// HTTP 服務器邏輯：接收新訂單信號，並根據最新未平倉資料決定下單類型
//--------------------------------------------------------------
void Receive_Strategy_Server_Signals()
{
    httplib::Server svr;

    // 新增 GET 接口：供外部查詢是否有新訂單訊號
    svr.Get("/getNewOrder", [](const httplib::Request &req, httplib::Response &res)
            {
        std::lock_guard<std::mutex> lock(g_orderMutex);
        if(g_newOrder.empty())
            res.set_content("{}", "application/json");
        else
        {
            res.set_content(g_newOrder, "application/json");
            // 回傳後清空訂單訊號
            g_newOrder = "";
        } });

    // HTTP POST /createPosition 接口：接收新訂單訊號
    svr.Post("/createPosition", [](const httplib::Request &req, httplib::Response &res)
             {
        try {
            json newSignal = json::parse(req.body);
            json retOrder;
            std::string commodity = newSignal.value("CommodityId", "");
            double signalLots = newSignal.value("Lots", 0.0);
            int longShort = newSignal.value("LongShort", 1);
            int newOrClosed = newSignal.value("NewOrClosedPosition", 1);

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

            if (newOrClosed == 0)
            {
                retOrder["CommodityId"] = commodity;
                retOrder["Lots"] = signalLots;
                retOrder["OrderType"] = "CloseOrder";
                retOrder["LongShort"] = longShort;
            }
            else
            {
                if (!hasPosition)
                {
                    double closedPL = g_latestPositions.value("ClosedProfitLoss", 0.0);
                    if (std::abs(closedPL) <= BASE_ORDER_PNL_RANGE)
                    {
                        retOrder["CommodityId"] = commodity;
                        retOrder["Lots"] = signalLots;
                        retOrder["OrderType"] = "BaseOrder";
                        retOrder["LongShort"] = longShort;
                    }
                    else
                    {
                        retOrder["error"] = "Realized P&L out of acceptable range for base order.";
                    }
                }
                else
                {
                    if (anyFloatingPL >= ADD_ORDER_THRESHOLD && totalLots < MAX_POSITION_LOTS)
                    {
                        retOrder["CommodityId"] = commodity;
                        double addLots = std::min(signalLots, MAX_POSITION_LOTS - totalLots);
                        retOrder["Lots"] = addLots;
                        retOrder["OrderType"] = "AddOrder";
                        retOrder["LongShort"] = longShort;
                    }
                    else
                    {
                        retOrder["error"] = "Add order conditions not met.";
                    }
                }
            }

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
    // 改用 port 1688
    svr.listen("0.0.0.0", 1688);
    server_running = false;
}

//--------------------------------------------------------------
// 導出函數：啟動 HTTP 服務器
//--------------------------------------------------------------
extern "C" __declspec(dllexport) void StartHttpServer()
{
    if (!server_running)
    {
        server_thread = std::thread(Receive_Strategy_Server_Signals);
    }
}

//--------------------------------------------------------------
// 導出函數：停止 HTTP 服務器
//--------------------------------------------------------------
extern "C" __declspec(dllexport) void StopHttpServer()
{
    if (server_thread.joinable())
    {
        server_thread.detach();
    }
}

//--------------------------------------------------------------
// 導出函數：由 MQL4 呼叫，處理持倉 JSON 並生成停利/停損/加碼單訊號
//--------------------------------------------------------------
extern "C" __declspec(dllexport) const char *ProcessPositions(const char *jsonInput)
{
    static std::string ret;
    try
    {
        json input = json::parse(jsonInput);
        g_latestPositions = input;

        json orders = json::array();
        if (input.contains("OpenPosition") && input["OpenPosition"].is_array())
        {
            for (auto &pos : input["OpenPosition"])
            {
                std::string commodity = pos.value("CommodityId", "");
                double lots = pos.value("Lots", 0.0);
                double floatingPL = pos.value("FloatingProfitLoss", 0.0);

                if (floatingPL >= TAKE_PROFIT_AMOUNT)
                {
                    json order;
                    order["CommodityId"] = commodity;
                    order["Lots"] = lots;
                    order["OrderType"] = "TakeProfit";
                    order["Amount"] = TAKE_PROFIT_AMOUNT;
                    orders.push_back(order);
                }
                else if (floatingPL <= -STOP_LOSS_AMOUNT)
                {
                    json order;
                    order["CommodityId"] = commodity;
                    order["Lots"] = lots;
                    order["OrderType"] = "StopLoss";
                    order["Amount"] = STOP_LOSS_AMOUNT;
                    orders.push_back(order);
                }
                else if (floatingPL >= ADD_ORDER_THRESHOLD)
                {
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
                        double addLots = std::min(lots, MAX_POSITION_LOTS - totalLots);
                        order["Lots"] = addLots;
                        order["OrderType"] = "AddOrder";
                        order["Amount"] = STOP_LOSS_AMOUNT;
                        orders.push_back(order);
                    }
                }
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

//--------------------------------------------------------------
// 新增導出函數：供 MQ4 輕鬆取得最新訂單訊號 JSON
//--------------------------------------------------------------
extern "C" __declspec(dllexport) const char *GetNewOrder()
{
    static std::string orderOut;
    std::lock_guard<std::mutex> lock(g_orderMutex);
    if (g_newOrder.empty())
    {
        orderOut = "{}";
    }
    else
    {
        orderOut = g_newOrder;
        g_newOrder = "";
    }
    return orderOut.c_str();
}
