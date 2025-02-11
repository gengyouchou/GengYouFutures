// MyFxOrderEngine.cpp
#include <windows.h>
#include <string>
#include "json.hpp" // 請確保你已經包含 nlohmann/json.hpp

using json = nlohmann::json;

// 固定參數設定（可根據實際策略調整）
const double STOP_LOSS_AMOUNT = 50.0;                   // 固定風險金額，例如 50 美元
const double TAKE_PROFIT_AMOUNT = STOP_LOSS_AMOUNT * 2; // 盈虧比 1:2
const double ADD_ORDER_THRESHOLD = 20.0;                // 當浮動盈虧達到此值時考慮加碼
const double MAX_POSITION_LOTS = 1.0;                   // 最大倉位口數

// 為返回字串定義一個全局緩衝區
static std::string g_returnJson;

/// <summary>
/// 處理 MT4 傳入的未平倉 JSON 資料，根據每筆持倉的 FloatingProfitLoss 產生相應的下單指令
/// 輸入 JSON 格式例如：
/// {
///    "Margin": 10000,
///    "ClosedProfitLoss": 0,
///    "OpenPosition": [
///       { "CommodityId": "XAUUSD", "Lots": 0.1, "FloatingProfitLoss": 10.0 },
///       { "CommodityId": "NQ100", "Lots": 0.1, "FloatingProfitLoss": -5.5 }
///    ]
/// }
/// 返回的 JSON 為陣列，每個元素代表一筆下單指令，例如：
/// [
///   { "CommodityId": "XAUUSD", "Lots": 0.1, "OrderType": "TakeProfit", "Amount": 100.0 },
///   { "CommodityId": "NQ100", "Lots": 0.1, "OrderType": "StopLoss", "Amount": 50.0 }
/// ]
/// </summary>
extern "C" __declspec(dllexport) const char *ProcessPositions(const char *jsonInput)
{
    try
    {
        // 解析傳入的 JSON 字串
        json input = json::parse(jsonInput);
        json orders = json::array();

        // 如果存在未平倉陣列則處理每筆持倉
        if (input.contains("OpenPosition") && input["OpenPosition"].is_array())
        {
            for (auto &pos : input["OpenPosition"])
            {
                std::string commodity = pos.value("CommodityId", "");
                double lots = pos.value("Lots", 0.0);
                double floatingPL = pos.value("FloatingProfitLoss", 0.0);

                // 停利單：當浮動盈虧 >= TAKE_PROFIT_AMOUNT 則觸發停利
                if (floatingPL >= TAKE_PROFIT_AMOUNT)
                {
                    json order;
                    order["CommodityId"] = commodity;
                    order["Lots"] = lots;
                    order["OrderType"] = "TakeProfit";
                    order["Amount"] = TAKE_PROFIT_AMOUNT;
                    orders.push_back(order);
                }
                // 停損單：當浮動盈虧 <= -STOP_LOSS_AMOUNT 則觸發停損
                else if (floatingPL <= -STOP_LOSS_AMOUNT)
                {
                    json order;
                    order["CommodityId"] = commodity;
                    order["Lots"] = lots;
                    order["OrderType"] = "StopLoss";
                    order["Amount"] = STOP_LOSS_AMOUNT;
                    orders.push_back(order);
                }
                // 加碼單：如果浮動盈虧 >= ADD_ORDER_THRESHOLD 且當前該商品的總倉位小於最大限制
                else if (floatingPL >= ADD_ORDER_THRESHOLD)
                {
                    // 計算該商品總倉位
                    double totalLots = 0.0;
                    for (auto &pos2 : input["OpenPosition"])
                    {
                        if (pos2.value("CommodityId", "") == commodity)
                            totalLots += pos2.value("Lots", 0.0);
                    }
                    if (totalLots < MAX_POSITION_LOTS)
                    {
                        json order;
                        order["CommodityId"] = commodity;
                        // 可設定加碼數量為差額或固定值（此處以差額示例）
                        double addLots = MAX_POSITION_LOTS - totalLots;
                        order["Lots"] = addLots;
                        order["OrderType"] = "AddOrder";
                        order["Amount"] = STOP_LOSS_AMOUNT; // 以風險金額作為參考
                        orders.push_back(order);
                    }
                }
                // 底單：根據策略，當天已實現損益在範圍內則允許接收底單（此處示例不做特殊處理）
            }
        }
        g_returnJson = orders.dump();
        return g_returnJson.c_str();
    }
    catch (const std::exception &e)
    {
        json errorJson;
        errorJson["error"] = e.what();
        g_returnJson = errorJson.dump();
        return g_returnJson.c_str();
    }
}

/// <summary>
/// 處理外部 HTTP 發來的新倉訊號 JSON 資料
/// 輸入 JSON 格式例如：
/// {
///     "CommodityId": "XAUUSD",
///     "Lots": 0.1,
///     "LongShort": 1,             // 1: Buy, -1: Sell
///     "NewOrClosedPosition": 1    // 1: New Order, 0: Close Order
/// }
/// 返回 JSON 指令，附加計算好的停損停利金額（固定的盈虧比1:2）
/// </summary>
extern "C" __declspec(dllexport) const char *ProcessNewOrderSignal(const char *jsonInput)
{
    try
    {
        json input = json::parse(jsonInput);
        json order;
        order["CommodityId"] = input.value("CommodityId", "");
        order["Lots"] = input.value("Lots", 0.0);
        int longShort = input.value("LongShort", 1);
        order["OrderType"] = (longShort == 1) ? "Buy" : "Sell";
        order["StopLossAmount"] = STOP_LOSS_AMOUNT;
        order["TakeProfitAmount"] = TAKE_PROFIT_AMOUNT;
        order["NewOrClosedPosition"] = input.value("NewOrClosedPosition", 1);
        g_returnJson = order.dump();
        return g_returnJson.c_str();
    }
    catch (const std::exception &e)
    {
        json errorJson;
        errorJson["error"] = e.what();
        g_returnJson = errorJson.dump();
        return g_returnJson.c_str();
    }
}

// ※ 若需在 DLL 中啟動 HTTP 服務（例如監聽本機 port 23 接收 JSON），則需額外使用 Windows Sockets API 實現 HTTP 服務器（此處不作完整實現）。
