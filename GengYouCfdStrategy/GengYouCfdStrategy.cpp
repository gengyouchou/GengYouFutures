// GengYouCfdStrategy.cpp
#include <windows.h>
#include <string>
#include <thread>
#include <atomic>
#include "httplib.h" // cpp-httplib
#include "json.hpp"  // nlohmann/json
using json = nlohmann::json;

static std::atomic<bool> server_running(false);
static std::thread server_thread;

// 為示例起見，固定策略參數
const double STOP_LOSS_AMOUNT = 50.0;
const double TAKE_PROFIT_AMOUNT = STOP_LOSS_AMOUNT * 2; // 盈虧比1:2

// HTTP 服務主函數
void run_http_server()
{
    httplib::Server svr;

    // 設置 /createPosition 接口，接收 POST 請求，內容為 JSON 字串
    svr.Post("/createPosition", [](const httplib::Request &req, httplib::Response &res)
             {
        try {
            // 解析請求 body 中的 JSON 資料
            json input = json::parse(req.body);

            // 模擬根據未平倉部位建立對應的下單訊號
            // 假設 input 格式如下：
            // {
            //   "Margin": 10000,
            //   "ClosedProfitLoss": 0,
            //   "OpenPosition": [
            //      { "CommodityId": "XAUUSD", "Lots": 0.1, "FloatingProfitLoss": 10.0 },
            //      { "CommodityId": "NQ100", "Lots": 0.1, "FloatingProfitLoss": -5.5 }
            //   ]
            // }
            json orders = json::array();
            if (input.contains("OpenPosition") && input["OpenPosition"].is_array()) {
                for (auto& pos : input["OpenPosition"]) {
                    std::string commodity = pos.value("CommodityId", "");
                    double lots = pos.value("Lots", 0.0);
                    double floatingPL = pos.value("FloatingProfitLoss", 0.0);

                    // 根據策略決定下單類型：停利、停損、或加碼
                    if (floatingPL >= TAKE_PROFIT_AMOUNT) {
                        json order;
                        order["CommodityId"] = commodity;
                        order["Lots"] = lots;
                        order["OrderType"] = "TakeProfit";
                        order["Amount"] = TAKE_PROFIT_AMOUNT;
                        orders.push_back(order);
                    } else if (floatingPL <= -STOP_LOSS_AMOUNT) {
                        json order;
                        order["CommodityId"] = commodity;
                        order["Lots"] = lots;
                        order["OrderType"] = "StopLoss";
                        order["Amount"] = STOP_LOSS_AMOUNT;
                        orders.push_back(order);
                    }
                    // 其他策略（例如加碼單）可根據需要擴充
                }
            }
            // 回傳結果 JSON
            res.set_content(orders.dump(), "application/json");
        }
        catch (const std::exception& e) {
            json err;
            err["error"] = e.what();
            res.status = 400;
            res.set_content(err.dump(), "application/json");
        } });

    server_running = true;
    // 監聽所有網路介面，本機 port 23（注意：port 23 為 Telnet 預設埠，實際部署請確認是否可用）
    svr.listen("0.0.0.0", 23);
    server_running = false;
}

// 導出函數，啟動 HTTP 服務器（在 DLL 中呼叫此函數以啟動服務）
extern "C" __declspec(dllexport) void StartHttpServer()
{
    if (!server_running)
    {
        // 啟動 HTTP 服務器於獨立執行緒中
        server_thread = std::thread(run_http_server);
    }
}

// 導出函數，停止 HTTP 服務器（此處僅示範，實際中應依 http server 的停止機制實現）
extern "C" __declspec(dllexport) void StopHttpServer()
{
    // 由於 httplib::Server::listen() 為阻塞呼叫，
    // 實際上需要保存 svr 物件並呼叫 svr.stop() 來停止服務。
    // 此範例簡單地將執行緒分離。
    if (server_thread.joinable())
    {
        // 此處應該通知服務器停止，然後 join 執行緒
        server_thread.detach();
    }
}

// 另外，若需要額外的下單處理函數（例如由 MT4 呼叫並傳入 JSON 建立對應倉位）
// 下面提供一個示例函數：ProcessPositions
extern "C" __declspec(dllexport) const char *ProcessPositions(const char *jsonInput)
{
    static std::string ret;
    try
    {
        json input = json::parse(jsonInput);
        json orders = json::array();

        // 模擬根據未平倉部位生成下單訊號
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
