#include "httplib.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <nlohmann/json.hpp>
#include <thread>
#include <deque>
#include <string>
#include <vector>

#include <iostream>
#include <sstream>
using json = nlohmann::json;

// 模擬外部全域變數
std::string g_strUserId = "gengyou";
long gStrategyConfig_StrategyMode = 1, gStrategyConfig_ClosingKeyPriceLevel = 2, gStrategyConfig_BidOfferLongShortThreshold = 40000;
double gStrategyConfig_BidOfferLongShortAttackSlope = 1, gStrategyConfig_MaximumLoss = 2;
long gCurCommPrice[10] = {0}, gEvaluatePosition = 1;
double gFutureRight = 10000, gClosedProfitLoss = 1234;
long gDayAmpAndKeyPrice_LongKey1 = 0, gDayAmpAndKeyPrice_SmallestAmp = 100;
double gBidOfferLongShortSlope = 1, gNumberOfStocksRisingAndFalling = 1;

// bid offer data

std::unordered_map<long, std::array<long, 4>> gCurCommHighLowPoint; // {High, Low, Open, Data}
std::unordered_map<long, vector<pair<long, long>>> gBest5BidOffer;

#include <nlohmann/json.hpp>
using json = nlohmann::json;

json AutoBest5LongToJson(LONG ProductIdxNo, const std::string &ProductName)
{
    json responseData;

    if (gCurCommHighLowPoint.count(ProductIdxNo) > 0)
    {
        long CurHigh = gCurCommHighLowPoint[ProductIdxNo][0];
        long CurLow = gCurCommHighLowPoint[ProductIdxNo][1];
        long Open = gCurCommHighLowPoint[ProductIdxNo][2];

        responseData["ProductName"] = ProductName;
        responseData["CurrentPrice"] = gCurCommPrice[ProductIdxNo];
        responseData["High"] = CurHigh;
        responseData["Low"] = CurLow;
        responseData["Open"] = Open;
    }

    if (gBest5BidOffer.count(ProductIdxNo) && gBest5BidOffer[ProductIdxNo].size() >= 10)
    {
        long TotalBid = gBest5BidOffer[ProductIdxNo][0].second +
                        gBest5BidOffer[ProductIdxNo][1].second +
                        gBest5BidOffer[ProductIdxNo][2].second +
                        gBest5BidOffer[ProductIdxNo][3].second +
                        gBest5BidOffer[ProductIdxNo][4].second;
        long TotalOffer = gBest5BidOffer[ProductIdxNo][9].second +
                          gBest5BidOffer[ProductIdxNo][8].second +
                          gBest5BidOffer[ProductIdxNo][7].second +
                          gBest5BidOffer[ProductIdxNo][6].second +
                          gBest5BidOffer[ProductIdxNo][5].second;

        json bidOfferData;
        for (int i = 0; i < 5; ++i)
        {
            bidOfferData["Bid" + std::to_string(i + 1)] = {
                {"Price", gBest5BidOffer[ProductIdxNo][i].first},
                {"Volume", gBest5BidOffer[ProductIdxNo][i].second}};
        }
        for (int i = 5; i < 10; ++i)
        {
            bidOfferData["Offer" + std::to_string(i - 4)] = {
                {"Price", gBest5BidOffer[ProductIdxNo][i].first},
                {"Volume", gBest5BidOffer[ProductIdxNo][i].second}};
        }

        responseData["BidOffer"] = bidOfferData;
        responseData["TotalBid"] = TotalBid;
        responseData["TotalOffer"] = TotalOffer;
    }
    else
    {
        responseData["Error"] = "Insufficient bid-offer data.";
    }

    return responseData;
}

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

    // 處理 index.html 的數據
    svr.Get("/index-data", [&currentPrice](const httplib::Request &req, httplib::Response &res)
            {
        currentPrice += generateRandomPrice(1);

        json indexData = {
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
            {"Time", time(0)},
            {"Symbol", "FUTURE1"}};

        std::cout << "Sending index response: " << indexData.dump(4) << std::endl;
        res.set_content(indexData.dump(), "application/json"); });

    svr.Get("/bid-offer-data", [](const httplib::Request &req, httplib::Response &res)
            {
    LONG ProductIdxNo = 0; // 假設為測試的產品索引
    std::string ProductName = "FUTURE1";

    json bidOfferData = AutoBest5LongToJson(ProductIdxNo, ProductName);

    std::cout << "Sending bid-offer response: " << bidOfferData.dump(4) << std::endl;
    res.set_content(bidOfferData.dump(), "application/json"); });

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
        ++gClosedProfitLoss;
    }

    serverThread.join();
    return 0;
}
