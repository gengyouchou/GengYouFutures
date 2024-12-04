#include "httplib.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <nlohmann/json.hpp> // 用於生成 JSON

using json = nlohmann::json;
double generateRandomPrice(double currentPrice)
{
    // 模擬價格波動：隨機生成 -1 到 1 的價格波動，四捨五入到 1 位小數
    double fluctuation = (rand() % 21 - 10) / 10.0;
    return currentPrice + fluctuation;
}

int main()
{
    srand(static_cast<unsigned int>(time(0)));
    httplib::Server svr;

    double currentPrice = 20000.0;

    // 處理 GET 請求，返回最新的期貨價格資料
    svr.Get("/", [&currentPrice](const httplib::Request &req, httplib::Response &res)
            {
        std::cout << "Received GET request for /" << std::endl; // 輸出接收到請求的訊息

        // 更新價格
        currentPrice += generateRandomPrice(1);

        // 生成 JSON 回應資料
        json data = {
            {"time", time(0)}, // 當前時間（Unix 時間戳）
            {"price", currentPrice}, // 當前價格
            {"volume", 100}, // 假設的交易量
            {"symbol", "FUTURE1"}}; // 商品名稱

        std::cout << "Sending response with price: " << currentPrice << std::endl; // 輸出正在發送的價格

        // 設置 JSON 响應
        res.set_content(data.dump(), "application/json"); });

    // 錯誤處理：返回方法不被支持的訊息
    svr.set_error_handler([](const httplib::Request &req, httplib::Response &res)
                          {
        std::cout << "Error: Unsupported method for " << req.method << " " << req.path << std::endl; // 錯誤訊息
        res.set_content(R"({"error": "Unsupported method"})", "application/json");
        res.status = 501; });

    // 讓伺服器開始監聽，並顯示伺服器啟動的訊息
    std::cout << "Server started at http://localhost:8000" << std::endl;

    // 開始監聽並處理請求
    svr.listen("0.0.0.0", 8000);
}
