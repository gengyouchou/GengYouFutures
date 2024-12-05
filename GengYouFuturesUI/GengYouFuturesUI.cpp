#include "httplib.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <nlohmann/json.hpp>
#include <thread>

using json = nlohmann::json;

// 模擬外部全域變數
std::string g_strUserId = "gengyou";
long gStrategyConfig_StrategyMode = 1, gStrategyConfig_ClosingKeyPriceLevel = 2, gStrategyConfig_BidOfferLongShortThreshold = 40000;
double gStrategyConfig_BidOfferLongShortAttackSlope = 1, gStrategyConfig_MaximumLoss = 2;
long gCurCommPrice[10] = {0}, gEvaluatePosition = 1;
double gFutureRight = 10000, gClosedProfitLoss = 1234;
long gDayAmpAndKeyPrice_LongKey1 = 0, gDayAmpAndKeyPrice_SmallestAmp = 100;
double gBidOfferLongShortSlope = 1, gNumberOfStocksRisingAndFalling = 1;

// 模擬生成隨機價格
double generateRandomPrice(double currentPrice)
{
    double fluctuation = (rand() % 21 - 10) / 10.0;
    return currentPrice + fluctuation;
}

// 啟動 HTTP 伺服器的函數
void startHttpServer(double &currentPrice)
{
    httplib::Server svr;

    // CORS 支援的中介函數
    svr.set_pre_routing_handler([](const httplib::Request &req, httplib::Response &res)
                                {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "*");
        if (req.method == "OPTIONS") {
            res.status = 204; // 對 OPTIONS 請求回應 204 No Content
            return httplib::Server::HandlerResponse::Handled;
        }
        return httplib::Server::HandlerResponse::Unhandled; });

    // 處理根路徑的 GET 請求
    svr.Get("/", [&currentPrice](const httplib::Request &req, httplib::Response &res)
            {
        currentPrice += generateRandomPrice(1);

        json data = {
            {"UserId", g_strUserId},
            {"StrategyMode", gStrategyConfig_StrategyMode},
            {"ClosingKeyPriceLevel", gStrategyConfig_ClosingKeyPriceLevel},
            {"BidOfferLongShortThreshold", gStrategyConfig_BidOfferLongShortThreshold},
            {"BidOfferLongShortAttackSlope", gStrategyConfig_BidOfferLongShortAttackSlope},
            {"MaximumLoss", gStrategyConfig_MaximumLoss},
            {"CurrentPrice", gCurCommPrice[0] / 100.0},
            {"EvaluatePosition", gEvaluatePosition},
            {"FutureRight", gFutureRight},
            {"ClosedProfitLoss", gClosedProfitLoss},
            {"DayAmpAndKeyPrice_LongKey1", gDayAmpAndKeyPrice_LongKey1},
            {"SmallestAmp", gDayAmpAndKeyPrice_SmallestAmp},
            {"BidOfferLongShortSlope", gBidOfferLongShortSlope},
            {"NumberOfStocksRisingAndFalling", gNumberOfStocksRisingAndFalling},
            {"Time", time(0)},
            {"Symbol", "FUTURE1"}};

        std::cout << "Sending response: " << data.dump(4) << std::endl;
        res.set_content(data.dump(), "application/json"); });

    // 錯誤處理，若方法不支持則回傳 501
    svr.set_error_handler([](const httplib::Request &req, httplib::Response &res)
                          {
        res.set_content(R"({"error": "Unsupported method"})", "application/json");
        res.status = 501; });

    // 開始監聽 HTTP 伺服器
    if (!svr.listen("0.0.0.0", 8080))
    {
        std::cerr << "Error: Unable to start server on port 8080. Is the port already in use?" << std::endl;
        return;
    }
}

int main()
{
    srand(static_cast<unsigned int>(time(0)));
    double currentPrice = 20000.0;

    // 開啟伺服器執行緒
    std::thread serverThread(startHttpServer, std::ref(currentPrice));

    // 模擬市場數據處理，持續運行
    while (true)
    {
        std::cout << "Processing market data..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));

        currentPrice += 0.2;
    }

    serverThread.join();
    return 0;
}
