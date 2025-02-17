#include "httplib.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <nlohmann/json.hpp>
#include <random>
#include <ctime>
#include <yaml-cpp/yaml.h>
#include "GengYouFuturesUI.h"

// 使用 nlohmann::json 簡化 JSON 的生成
using json = nlohmann::json;

json MainOutputToJson();

// 全局結構實例
USER_ACCOUNT_UI gUserAccountUI;
STRATEGY_CONFIG_UI gStrategyConfigUI;
MARKET_DATA_UI gMarketDataUI;
OPEN_INTEREST_INFO_UI gOpenInterestInfoUI;

// Runtime cache to store in-memory data
std::deque<YAML::Node> gCacheData;

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
    gMarketDataUI.gTransactionList[productIdxNo][3] = priceDist(rng);
    gMarketDataUI.gTransactionList[productIdxNo][4] = priceDist(rng);

    // for Bid Offer Long Short

    gMarketDataUI.gBidOfferLongShortSlope = priceDist(rng);
    gMarketDataUI.gLongShort = gMarketDataUI.gLongShort + 10;
    gMarketDataUI.gCurServerTime[2] += 1;
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
        long TotalBid = gMarketDataUI.gBest5BidOffer[productIdxNo][0].second +
                        gMarketDataUI.gBest5BidOffer[productIdxNo][1].second +
                        gMarketDataUI.gBest5BidOffer[productIdxNo][2].second +
                        gMarketDataUI.gBest5BidOffer[productIdxNo][3].second +
                        gMarketDataUI.gBest5BidOffer[productIdxNo][4].second;
        long TotalOffer = gMarketDataUI.gBest5BidOffer[productIdxNo][9].second +
                          gMarketDataUI.gBest5BidOffer[productIdxNo][8].second +
                          gMarketDataUI.gBest5BidOffer[productIdxNo][7].second +
                          gMarketDataUI.gBest5BidOffer[productIdxNo][6].second +
                          gMarketDataUI.gBest5BidOffer[productIdxNo][5].second;

        responseData["TotalBid"] = TotalBid;
        responseData["TotalOffer"] = TotalOffer;

        const auto &bidOffer = gMarketDataUI.gBest5BidOffer[productIdxNo];

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

    // 其他數據
    long nPtr = 0, nBid = 0, nAsk = 0, nClose = 0, nQty = 0;

    if (gMarketDataUI.gTransactionList.count(productIdxNo))
    {
        nPtr = gMarketDataUI.gTransactionList[productIdxNo][0];
        nBid = gMarketDataUI.gTransactionList[productIdxNo][1];
        nAsk = gMarketDataUI.gTransactionList[productIdxNo][2];
        nClose = gMarketDataUI.gTransactionList[productIdxNo][3];
        nQty = gMarketDataUI.gTransactionList[productIdxNo][4];
    }
    responseData["ClosePrice"] = nClose;
    responseData["Quantity"] = nQty;

    return responseData;
}

json ConvertDequeToJSON(const std::deque<YAML::Node> &gCacheData)
{
    json jsonData = json::array(); // 使用 JSON 數組格式初始化

    // 遍歷 gCacheData，將每個節點轉換為 JSON 對象並添加到數組中
    for (const auto &node : gCacheData)
    {
        json record;
        record["timestamp"] = node["timestamp"].as<std::string>();
        record["gLongShort"] = node["gLongShort"].as<LONG>();
        record["gBidOfferLongShortSlope"] = node["gBidOfferLongShortSlope"].as<double>(); // 將資料作為 double 處理
        jsonData.push_back(record);                                                       // 插入數據到 JSON 數組中
    }

    return jsonData; // 返回 JSON 數據
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

    svr.Get("/Long-Short-Cache-data", [](const httplib::Request &, httplib::Response &res)
            {
            auto jsonResponse = QueryCacheData(COMMODITY_MAIN);
            res.set_content(jsonResponse.dump(), "application/json"); });

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

// // 主程序
// int main()
// {
//     std::atomic<bool> isRunning(true);
//     std::thread serverThread(startHttpServer, std::ref(isRunning));

//     gMarketDataUI.gLongShort = 10;
//     gMarketDataUI.gCurServerTime[0] = 0;
//     gMarketDataUI.gCurServerTime[1] = 0;
//     gMarketDataUI.gCurServerTime[2] = 0;

//     while (isRunning.load())
//     {
//         std::this_thread::sleep_for(std::chrono::seconds(1));
//         simulateMarketData();
//         std::cout << "Current Price: " << currentPrice.load() << std::endl;
//     }

//     serverThread.join();
//     return 0;
// }

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
        {"Updating", gMarketDataUI.Updating},
        {"CurCommPrice", gMarketDataUI.gCurCommPrice},
        {"MtxPrices", gMarketDataUI.MtxPrices},
        {"Diff", gMarketDataUI.Diff},
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

    // 高低點數據以及其他的統計指標
    output["MarketData"]["Statistics"] = {
        {"CurHigh", gMarketDataUI.CurHigh},
        {"CurLow", gMarketDataUI.CurLow},
        {"CurAvg", gMarketDataUI.CurAvg},
        {"CurAmp", gMarketDataUI.CurAmp},
        {"CostMovingAverage", gMarketDataUI.CostMovingAverage},
        {"OpenPrice", gMarketDataUI.OpenPrice},
        {"ShockLongExtremeValue", gMarketDataUI.ShockLongExtremeValue},
        {"ShockShortExtremeValue", gMarketDataUI.ShockShortExtremeValue}};

    // Best 5 Bid/Offer 數據
    json best5Data;
    for (const auto &[key, value] : gMarketDataUI.gBest5BidOffer)
    {
        json best5Array;
        for (const auto &[price, volume] : value)
        {
            best5Array.push_back({{"Price", price}, {"Volume", volume}});
        }
        best5Data[std::to_string(key)] = best5Array;
    }
    output["MarketData"]["Best5BidOffer"] = best5Data;

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
