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
    long ClosePrice = 0;
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

    gMarketData.ClosePrice = priceDist(rng);

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

    // 添加 ClosePrice 和 evaluatePosition 的轉換
    responseData["ClosePrice"] = gMarketData.ClosePrice;

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

VOID MainOutputToJson(VOID)
{
    // Ouput start

    printf("[UserId:%s], [StrategyMode:%ld], [SpecifyLongShort:%d], [ClosingKeyPriceLevel:%ld], [BidOfferLongShortThreshold:%ld], [BidOfferLongShortExtremeValue:%ld], [BidOfferLongShortAttackSlope:%f], [ActivePoint:%ld], [MaximumLoss:%f]\n",
           g_strUserId.c_str(), gStrategyConfig.StrategyMode, gStrategyConfig.SpecifyLongShort, gStrategyConfig.ClosingKeyPriceLevel,
           gStrategyConfig.BidOfferLongShortThreshold, gStrategyConfig.BidOfferLongShortExtremeValue, gStrategyConfig.BidOfferLongShortAttackSlope, gStrategyConfig.ActivePoint, gStrategyConfig.MaximumLoss);
    printf("=========================================\n");
    printf("[CurMtxPrice: %ld] ", gCurCommPrice[MtxCommodtyInfo] / 100);
    printf("[TSEA prices: %ld, Valume: %ld] ",
           gCurCommPrice[gCommodtyInfo.TSEAIdxNo] / 100, gCurTaiexInfo[0][1]);
    printf("[Diff: %d] ", (gCurCommPrice[MtxCommodtyInfo] - gCurCommPrice[gCommodtyInfo.TSEAIdxNo]) / 100);
    printf("[ServerTime: %d: %d: %d]\n", gCurServerTime[0], gCurServerTime[1], gCurServerTime[2]);

    printf("=========================================\n");

    printf("[CurNQPrice: %ld], [NQMa20: %f], [NQMa20LongShort: %f]\n",
           gCurOsCommPrice[gCommodtyOsInfo.NQIdxNo], gNQMa20, gNQMa20LongShort);

    printf("=========================================\n");

    if (gCurCommHighLowPoint.count(MtxCommodtyInfo) > 0)
    {

        long CurHigh = gCurCommHighLowPoint[MtxCommodtyInfo][0] / 100;
        long CurLow = gCurCommHighLowPoint[MtxCommodtyInfo][1] / 100;
        long CostMovingAverage = static_cast<long>(gCostMovingAverageVal);
        long OpenPrice = gCurCommHighLowPoint[MtxCommodtyInfo][2] / 100;
        double ShockLongExtremeValue = gCostMovingAverageVal - EstimatedTodaysAmplitude() / 2;
        double ShockShortExtremeValue = gCostMovingAverageVal + EstimatedTodaysAmplitude() / 2;

        printf("Open: %ld, CurHigh: %ld, CurLow: %ld, Ma5: %f, Ma5LongShort: %f, CostMovingAverage: %ld, ",
               OpenPrice, CurHigh, CurLow, gMa5, gMa5LongShort, CostMovingAverage);
        printf("CurAvg: %ld, CurAmp : %ld, ", (CurHigh + CurLow) / 2, CurHigh - CurLow);
        printf("LongExtremeValue: %ld, ShortExtremeValue: %ld\n", static_cast<long>(ShockLongExtremeValue), static_cast<long>(ShockShortExtremeValue));
    }

    printf("=========================================\n");

    printf("EvaluatePosition: %ld, FutureRight: %f, ClosedProfitLoss: %f", gEvaluatePosition, gFutureRight, gClosedProfitLoss);

    if (gOpenInterestInfo.openPosition != 0)
    {
        printf(", Open Position: %d, AvgCost:%f, ProfitAndLoss: %f\n",
               gOpenInterestInfo.openPosition,
               gOpenInterestInfo.avgCost,
               gOpenInterestInfo.profitAndLoss);
    }

    printf("\n=========================================\n");

    printf("Long Key 5: %ld\n", gDayAmpAndKeyPrice.LongKey5);
    printf("Long Key 4: %ld\n", gDayAmpAndKeyPrice.LongKey4);
    printf("Long Key 3: %ld\n", gDayAmpAndKeyPrice.LongKey3);
    printf("Long Key 2: %ld\n", gDayAmpAndKeyPrice.LongKey2);
    printf("Long Key 1: %ld\n", gDayAmpAndKeyPrice.LongKey1);
    printf("=========================================\n");
    printf("Short Key 1: %ld\n", gDayAmpAndKeyPrice.ShortKey1);
    printf("Short Key 2: %ld\n", gDayAmpAndKeyPrice.ShortKey2);
    printf("Short Key 3: %ld\n", gDayAmpAndKeyPrice.ShortKey3);
    printf("Short Key 4: %ld\n", gDayAmpAndKeyPrice.ShortKey4);
    printf("Short Key 5: %ld\n", gDayAmpAndKeyPrice.ShortKey5);

    printf("=========================================\n");

    printf("SmallestAmp : %ld, ", gDayAmpAndKeyPrice.SmallestAmp);
    printf("SmallAmp : %ld, ", gDayAmpAndKeyPrice.SmallAmp);
    printf("AvgAmp : %ld, ", gDayAmpAndKeyPrice.AvgAmp);
    printf("LargerAmp : %ld, ", gDayAmpAndKeyPrice.LargerAmp);
    printf("LargestAmp : %ld\n", gDayAmpAndKeyPrice.LargestAmp);

    printf("=========================================\n");

    printf("BidOfferLongShortSlope: %f, LongShort: %ld, BidOfferLongShort: %ld, TransactionListLongShort: %ld, OsTransactionListLongShort: %ld, NumberOfStocksRisingAndFalling: %f\n",
           gBidOfferLongShortSlope, gLongShort, gBidOfferLongShort, gTransactionListLongShort, gOsTransactionListLongShort, gNumberOfStocksRisingAndFalling);

    printf("=========================================\n");

    AutoBest5Long(gCommodtyInfo.TSMCIdxNo, TSMC);
    AutoBest5Long(gCommodtyInfo.FOXCONNIdxNo, FOXCONN);
    AutoBest5Long(gCommodtyInfo.MediaTekIdxNo, MEDIATEK);

    printf("=========================================\n");

    printf("TSEA Total OFFER: [%ld]\n", gCurTaiexInfo[0x00][3]);
    printf("            BID : [%ld]\n", gCurTaiexInfo[0x00][2]);
    printf("TPEX Total OFFER: [%ld]\n", gCurTaiexInfo[0x01][3]);
    printf("            BID : [%ld]\n", gCurTaiexInfo[0x01][2]);
}
