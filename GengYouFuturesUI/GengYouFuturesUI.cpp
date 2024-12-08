#include "GengYouFuturesUI.h"
#include "httplib.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <nlohmann/json.hpp>
#include <random>
#include <ctime>

// 使用 nlohmann::json 簡化 JSON 的生成
using json = nlohmann::json;

json MainOutputToJson();

// 全局結構實例
USER_ACCOUNT_UI gUserAccountUI;
STRATEGY_CONFIG_UI gStrategyConfigUI;
MARKET_DATA_UI gMarketDataUI;
OPEN_INTEREST_INFO_UI gOpenInterestInfoUI;

// 互斥鎖保護全局變數
std::mutex marketDataMutex;
std::atomic<double> currentPrice(20000.0); // 當前價格

// 模擬填充 MarketData 的函數
void simulateMarketData()
{
    static std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<long> priceDist(19000, 21000);
    std::uniform_int_distribution<long> volumeDist(1, 100);

    long productIdxNo = 0; // 模擬產品索引

    // 使用鎖保護全局數據
    std::lock_guard<std::mutex> lock(marketDataMutex);

    // 模擬高低點數據
    long openPrice = priceDist(rng);
    long highPrice = openPrice + priceDist(rng) % 100;
    long lowPrice = openPrice - priceDist(rng) % 100;
    gMarketDataUI.gCurCommHighLowPoint[productIdxNo] = {highPrice, lowPrice, openPrice, 0};

    // 模擬買五檔和賣五檔數據
    std::vector<std::pair<long, long>> bidOffer;
    for (int i = 0; i < 10; ++i)
    {
        long price = priceDist(rng);
        long volume = volumeDist(rng);
        bidOffer.emplace_back(price, volume);
    }
    gMarketDataUI.gBest5BidOffer[productIdxNo] = bidOffer;

    // 模擬其他數據
    gMarketDataUI.gClosedProfitLoss += 1.0;
    currentPrice.store(priceDist(rng));
}

// 將市場數據轉換為 JSON 格式
json AutoBest5LongToJson(long productIdxNo, const std::string &productName)
{
    json responseData;

    std::lock_guard<std::mutex> lock(marketDataMutex); // 鎖定數據

    // 高低點數據
    if (gMarketDataUI.gCurCommHighLowPoint.count(productIdxNo) > 0)
    {
        auto &points = gMarketDataUI.gCurCommHighLowPoint[productIdxNo];
        responseData["ProductName"] = productName;
        responseData["High"] = points[0];
        responseData["Low"] = points[1];
        responseData["Open"] = points[2];
    }
    else
    {
        responseData["Error"] = "High-Low data not available.";
    }

    // Bid/Offer 數據
    if (gMarketDataUI.gBest5BidOffer.count(productIdxNo) > 0 && gMarketDataUI.gBest5BidOffer[productIdxNo].size() >= 10)
    {
        const auto &bidOffer = gMarketDataUI.gBest5BidOffer[productIdxNo];
        for (int i = 0; i < 5; ++i)
        {
            responseData["Bid" + std::to_string(i + 1)] = {{"Price", bidOffer[i].first}, {"Volume", bidOffer[i].second}};
            responseData["Offer" + std::to_string(i + 1)] = {{"Price", bidOffer[i + 5].first}, {"Volume", bidOffer[i + 5].second}};
        }
    }
    else
    {
        responseData["Error"] = "Insufficient bid-offer data.";
    }

    // 其他數據
    responseData["ClosePrice"] = gMarketDataUI.gClosedProfitLoss;
    responseData["CurrentPrice"] = currentPrice.load();

    return responseData;
}

// 啟動 HTTP 伺服器
void startHttpServer(std::atomic<bool> &isRunning)
{
    httplib::Server svr;

    // 支援 CORS
    svr.set_pre_routing_handler([](const httplib::Request &req, httplib::Response &res)
                                {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "*");
        if (req.method == "OPTIONS") {
            res.status = 204;
            return httplib::Server::HandlerResponse::Handled;
        }
        return httplib::Server::HandlerResponse::Unhandled; });

    // 路由處理
    svr.Get("/index-data", [](const httplib::Request &, httplib::Response &res)
            {
    auto jsonResponse = MainOutputToJson();
    res.set_content(jsonResponse.dump(), "application/json"); });

    svr.Get("/bid-offer-data", [](const httplib::Request &, httplib::Response &res)
            {
        long productIdxNo = 0;
        std::string productName = "FUTURE1";
        res.set_content(AutoBest5LongToJson(productIdxNo, productName).dump(), "application/json"); });

    // 啟動伺服器
    if (!svr.listen("0.0.0.0", 8080))
    {
        std::cerr << "Error: Unable to start HTTP server. Check port." << std::endl;
        isRunning.store(false);
    }
}

// 主程序
int main()
{
    std::atomic<bool> isRunning(true);
    std::thread serverThread(startHttpServer, std::ref(isRunning));

    while (isRunning.load())
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        simulateMarketData();
        std::cout << "Current Price: " << currentPrice.load() << std::endl;
    }

    serverThread.join();
    return 0;
}

json MainOutputToJson()
{
    // 構建 JSON 對象
    json output;

    // User account 信息
    output["UserAccount"] = {
        {"UserId", gUserAccountUI.g_strUserId}};

    // Strategy 配置
    output["StrategyConfig"] = {
        {"ClosingKeyPriceLevel", gStrategyConfigUI.ClosingKeyPriceLevel},
        {"BidOfferLongShortThreshold", gStrategyConfigUI.BidOfferLongShortThreshold},
        {"BidOfferLongShortExtremeValue", gStrategyConfigUI.BidOfferLongShortExtremeValue},
        {"BidOfferLongShortAttackSlope", gStrategyConfigUI.BidOfferLongShortAttackSlope},
        {"ActivePoint", gStrategyConfigUI.ActivePoint},
        {"MaximumLoss", gStrategyConfigUI.MaximumLoss},
        {"StrategyMode", gStrategyConfigUI.StrategyMode},
        {"SpecifyLongShort", gStrategyConfigUI.SpecifyLongShort}};

    // Market Data
    output["MarketData"] = {
        {"CurCommPrice", gMarketDataUI.gCurCommPrice},
        {"CurTaiexInfo", gMarketDataUI.gCurTaiexInfo},
        {"CurServerTime", {gMarketDataUI.gCurServerTime[0], gMarketDataUI.gCurServerTime[1], gMarketDataUI.gCurServerTime[2]}},
        {"CurOsCommPrice", gMarketDataUI.gCurOsCommPrice},
        {"BidOfferLongShort", gMarketDataUI.gBidOfferLongShort},
        {"TransactionListLongShort", gMarketDataUI.gTransactionListLongShort},
        {"OsTransactionListLongShort", gMarketDataUI.gOsTransactionListLongShort},
        {"LongShort", gMarketDataUI.gLongShort},
        {"CostMovingAverageVal", gMarketDataUI.gCostMovingAverageVal},
        {"Ma5", gMarketDataUI.gMa5},
        {"Ma5LongShort", gMarketDataUI.gMa5LongShort},
        {"NQMa20", gMarketDataUI.gNQMa20},
        {"NQMa20LongShort", gMarketDataUI.gNQMa20LongShort},
        {"BidOfferLongShortSlope", gMarketDataUI.gBidOfferLongShortSlope},
        {"NumberOfStocksRisingAndFalling", gMarketDataUI.gNumberOfStocksRisingAndFalling},
        {"EvaluatePosition", gMarketDataUI.gEvaluatePosition},
        {"ClosedProfitLoss", gMarketDataUI.gClosedProfitLoss},
        {"FutureRight", gMarketDataUI.gFutureRight}};

    // 高低點數據
    json highLowData;
    for (const auto &[key, value] : gMarketDataUI.gCurCommHighLowPoint)
    {
        highLowData[std::to_string(key)] = {value[0], value[1], value[2], value[3]};
    }
    output["MarketData"]["CurCommHighLowPoint"] = highLowData;

    // Day Amp and Key Price
    output["MarketData"]["DayAmpAndKeyPrice"] = {
        {"SmallestAmp", gMarketDataUI.gDayAmpAndKeyPrice.SmallestAmp},
        {"SmallAmp", gMarketDataUI.gDayAmpAndKeyPrice.SmallAmp},
        {"AvgAmp", gMarketDataUI.gDayAmpAndKeyPrice.AvgAmp},
        {"LargerAmp", gMarketDataUI.gDayAmpAndKeyPrice.LargerAmp},
        {"LargestAmp", gMarketDataUI.gDayAmpAndKeyPrice.LargestAmp},
        {"LongKey5", gMarketDataUI.gDayAmpAndKeyPrice.LongKey5},
        {"LongKey4", gMarketDataUI.gDayAmpAndKeyPrice.LongKey4},
        {"LongKey3", gMarketDataUI.gDayAmpAndKeyPrice.LongKey3},
        {"LongKey2", gMarketDataUI.gDayAmpAndKeyPrice.LongKey2},
        {"LongKey1", gMarketDataUI.gDayAmpAndKeyPrice.LongKey1},
        {"ShortKey5", gMarketDataUI.gDayAmpAndKeyPrice.ShortKey5},
        {"ShortKey4", gMarketDataUI.gDayAmpAndKeyPrice.ShortKey4},
        {"ShortKey3", gMarketDataUI.gDayAmpAndKeyPrice.ShortKey3},
        {"ShortKey2", gMarketDataUI.gDayAmpAndKeyPrice.ShortKey2},
        {"ShortKey1", gMarketDataUI.gDayAmpAndKeyPrice.ShortKey1}};

    // Open Interest Info
    output["OpenInterestInfo"] = {
        {"Product", gOpenInterestInfoUI.product},
        {"BuySell", gOpenInterestInfoUI.buySell},
        {"OpenPosition", gOpenInterestInfoUI.openPosition},
        {"DayTradePosition", gOpenInterestInfoUI.dayTradePosition},
        {"AvgCost", gOpenInterestInfoUI.avgCost},
        {"ProfitAndLoss", gOpenInterestInfoUI.profitAndLoss},
        {"NeedToUpdate", gOpenInterestInfoUI.NeedToUpdate}};

    return output;
}
