#include "httplib.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <array>
#include <thread>
#include <nlohmann/json.hpp>
#include <random>
#include <ctime>
#include <atomic>

using json = nlohmann::json;

// 模擬全域設定
struct StrategyConfig
{
    long strategyMode = 1;
    long closingKeyPriceLevel = 2;
    long bidOfferLongShortThreshold = 40000;
    double bidOfferLongShortAttackSlope = 1.0;
    double maximumLoss = 2.0;
} gStrategyConfig;

// 模擬全域數據
struct MarketData
{
    long currentPrice[10] = {0};
    long evaluatePosition = 1;
    double futureRight = 10000.0;
    double closedProfitLoss = 1234.0;
    std::unordered_map<long, std::array<long, 4>> highLowPoints; // High, Low, Open, Data
    std::unordered_map<long, std::vector<std::pair<long, long>>> best5BidOffer;
} gMarketData;

// 當前價格（使用 atomic 保證線程安全）
std::atomic<double> currentPrice(20000.0);

// 模擬填充 MarketData 的函數
void simulateMarketData()
{
    static std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr))); // 隨機數生成器
    std::uniform_int_distribution<long> priceDist(19000, 21000);            // 模擬價格範圍
    std::uniform_int_distribution<long> volumeDist(1, 100);                 // 模擬成交量範圍

    long productIdxNo = 0; // 模擬產品索引

    // 模擬高低點數據
    long openPrice = priceDist(rng);
    long highPrice = openPrice + priceDist(rng) % 100; // 模擬高點（略高於開盤價）
    long lowPrice = openPrice - priceDist(rng) % 100;  // 模擬低點（略低於開盤價）
    gMarketData.highLowPoints[productIdxNo] = {highPrice, lowPrice, openPrice, 0};

    // 模擬買五檔和賣五檔數據
    std::vector<std::pair<long, long>> bidOffer;
    for (int i = 0; i < 10; ++i)
    {
        long price = priceDist(rng);
        long volume = volumeDist(rng);
        bidOffer.emplace_back(price, volume);
    }
    gMarketData.best5BidOffer[productIdxNo] = bidOffer;

    // 更新當前價格
    currentPrice.store(priceDist(rng));
}

// 生成隨機價格
double generateRandomPrice(double basePrice)
{
    double fluctuation = (rand() % 21 - 10) / 10.0; // ±1.0
    return basePrice + fluctuation;
}

// 將市場數據轉換為 JSON 格式
json AutoBest5LongToJson(long productIdxNo, const std::string &productName)
{
    json responseData;

    // 取得高低點數據
    if (gMarketData.highLowPoints.count(productIdxNo) > 0)
    {
        auto &points = gMarketData.highLowPoints[productIdxNo];
        responseData["ProductName"] = productName;
        responseData["CurrentPrice"] = gMarketData.currentPrice[productIdxNo];
        responseData["High"] = points[0];
        responseData["Low"] = points[1];
        responseData["Open"] = points[2];
    }
    else
    {
        responseData["Error"] = "High-Low data not available.";
    }

    // 取得 Bid/Offer 數據
    if (gMarketData.best5BidOffer.count(productIdxNo) > 0 && gMarketData.best5BidOffer[productIdxNo].size() >= 10)
    {
        const auto &bidOffer = gMarketData.best5BidOffer[productIdxNo];
        json bidOfferData;

        for (int i = 0; i < 5; ++i)
        {
            bidOfferData["Bid" + std::to_string(i + 1)] = {{"Price", bidOffer[i].first}, {"Volume", bidOffer[i].second}};
        }
        for (int i = 5; i < 10; ++i)
        {
            bidOfferData["Offer" + std::to_string(i - 4)] = {{"Price", bidOffer[i].first}, {"Volume", bidOffer[i].second}};
        }

        responseData["BidOffer"] = bidOfferData;
    }
    else
    {
        responseData["Error"] = "Insufficient bid-offer data.";
    }

    return responseData;
}

// 啟動 HTTP 伺服器
void startHttpServer(std::atomic<bool> &isRunning)
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
    // index-data 路由
    svr.Get("/index-data", [](const httplib::Request &, httplib::Response &res)
            {
        json indexData = {
            {"StrategyMode", gStrategyConfig.strategyMode},
            {"ClosingKeyPriceLevel", gStrategyConfig.closingKeyPriceLevel},
            {"BidOfferLongShortThreshold", gStrategyConfig.bidOfferLongShortThreshold},
            {"BidOfferLongShortAttackSlope", gStrategyConfig.bidOfferLongShortAttackSlope},
            {"MaximumLoss", gStrategyConfig.maximumLoss},
            {"CurrentPrice", currentPrice.load()},
            {"FutureRight", gMarketData.futureRight},
            {"ClosedProfitLoss", gMarketData.closedProfitLoss},
            {"Timestamp", time(0)},
        };

        res.set_content(indexData.dump(), "application/json"); });

    // bid-offer-data 路由
    svr.Get("/bid-offer-data", [](const httplib::Request &, httplib::Response &res)
            {
        long productIdxNo = 0; // 模擬產品索引
        std::string productName = "FUTURE1";

        json bidOfferData = AutoBest5LongToJson(productIdxNo, productName);
        res.set_content(bidOfferData.dump(), "application/json"); });

    // 啟動伺服器
    if (!svr.listen("0.0.0.0", 8080))
    {
        std::cerr << "Error: Unable to start HTTP server. Check if port 8080 is in use." << std::endl;
        isRunning.store(false); // 停止主線程
    }
}

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    // 控制伺服器運行狀態
    std::atomic<bool> isRunning(true);

    // 啟動伺服器執行緒
    std::thread serverThread(startHttpServer, std::ref(isRunning));

    // 主線程模擬市場數據處理
    while (isRunning.load())
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        simulateMarketData();
        gMarketData.closedProfitLoss += 1.0;

        std::cout << "Current Price: " << currentPrice.load() << ", Closed Profit/Loss: " << gMarketData.closedProfitLoss << std::endl;
    }

    serverThread.join();
    return 0;
}
