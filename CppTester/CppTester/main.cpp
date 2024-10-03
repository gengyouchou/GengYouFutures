#include "SKCenterLib.h"
#include "SKOSQuoteLib.h"
#include "SKOrderLib.h"
#include "SKQuoteLib.h"
#include "SKReplyLib.h"
#include <Logger.h>
#include <array>
#include <chrono>  // For std::chrono::steady_clock
#include <conio.h> // For kbhit() and _getch()
#include <cstdlib> // For system("cls")
#include <deque>
#include <iostream>
#include <thread> // For std::this_thread::sleep_for
#include <unordered_map>
#include <yaml-cpp/yaml.h>

#include "Strategy.h"

extern std::deque<long> gDaysKlineDiff;
extern std::unordered_map<long, std::array<long, 4>> gCurCommHighLowPoint;
extern SHORT gCurServerTime[3];
extern std::unordered_map<long, long> gCurCommPrice;
extern std::unordered_map<SHORT, std::array<long, 4>> gCurTaiexInfo;
extern std::unordered_map<long, vector<pair<long, long>>> gBest5BidOffer;
extern std::unordered_map<long, std::array<long, 6>> gTransactionList;
// long nPtr, long nBid, long nAsk, long nClose, long nQty,

extern COMMODITY_INFO gCommodtyInfo;
extern DAY_AMP_AND_KEY_PRICE gDayAmpAndKeyPrice;
extern OpenInterestInfo gOpenInterestInfo;
extern LONG gBidOfferLongShort;
extern LONG gTransactionListLongShort;
extern double gCostMovingAverageVal;
extern STRATEGY_CONFIG gStrategyConfig;

// Define the global logger instance
Logger logger("debug");

CSKCenterLib *pSKCenterLib;
CSKQuoteLib *pSKQuoteLib;
CSKReplyLib *pSKReplyLib;
CSKOrderLib *pSKOrderLib;

CSKOSQuoteLib *pSKOsQuoteLib;

long g_nCode = 0;
extern string g_strUserId;
extern string gPwd;

void release();

void AutoConnect()
{
    while (pSKQuoteLib->IsConnected() != 1)
    {
        g_nCode = pSKQuoteLib->LeaveMonitor();
        g_nCode = pSKQuoteLib->EnterMonitorLONG();
        pSKCenterLib->PrintfCodeMessage("Quote", "EnterMonitor", g_nCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(3000)); //  CPU
    }

    while (pSKOsQuoteLib->IsConnected() != 1)
    {
        g_nCode = pSKOsQuoteLib->LeaveMonitor();
        g_nCode = pSKOsQuoteLib->EnterMonitorLONG();
        pSKCenterLib->PrintfCodeMessage("Quote", "EnterMonitor", g_nCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(3000)); //  CPU
    }
}

void AutoLogIn()
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

    //
    g_nCode = pSKOrderLib->Initialize();
    pSKCenterLib->PrintfCodeMessage("AutoLogIn", "Initialize", g_nCode);

    //
    g_nCode = pSKOrderLib->ReadCertByID(g_strUserId);
    pSKCenterLib->PrintfCodeMessage("AutoLogIn", "ReadCertByID", g_nCode);

    //
    g_nCode = pSKOrderLib->GetUserAccount();
    pSKCenterLib->PrintfCodeMessage("AutoLogIn", "GetUserAccount", g_nCode);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

LONG AutoGetFutureRights()
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

    LONG res = pSKOrderLib->GetFutureRights(g_strUserId);

    LOG(DEBUG_LEVEL_INFO, "GetFutureRights res = %d", res);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");

    return res;
}

LONG AutoQuote(IN string ProductNum, short sPageNo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

    LONG res = pSKQuoteLib->RequestStocks(&sPageNo, ProductNum);
    pSKCenterLib->PrintfCodeMessage("Quote", "RequestStocks", res);
    DEBUG(DEBUG_LEVEL_INFO, "res= %d", res);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");

    return res;
}

LONG AutoQuoteTicks(IN string ProductNum, short sPageNo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

    LONG res = pSKQuoteLib->RequestTicks(&sPageNo, ProductNum);

    pSKCenterLib->PrintfCodeMessage("Quote", "RequestTicks", res);

    DEBUG(DEBUG_LEVEL_INFO, "res= %d", res);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");

    return res;
}

LONG AutoOsQuoteTicks(IN string ProductNum, short sPageNo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

    LONG res = pSKOsQuoteLib->RequestTicks(&sPageNo, ProductNum);

    pSKCenterLib->PrintfCodeMessage("Quote", "RequestTicks", res);

    DEBUG(DEBUG_LEVEL_INFO, "res= %d", res);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");

    return res;
}

void AutoBest5Long(LONG ProductIdxNo, string ProductName)
{
    if (gCurCommHighLowPoint.count(ProductIdxNo) > 0)
    {
        long CurHigh = gCurCommHighLowPoint[ProductIdxNo][0];
        long CurLow = gCurCommHighLowPoint[ProductIdxNo][1];
        long Open = gCurCommHighLowPoint[ProductIdxNo][2];

        DEBUG(DEBUG_LEVEL_DEBUG, "IdxNo: %ld. High: %ld, Low: %ld", ProductIdxNo, CurHigh, CurLow);

        printf("%s : %ld, ", ProductName.c_str(), gCurCommPrice[ProductIdxNo]);

        printf("Open: %ld, CurHigh: %ld, CurLow: %ld\n", Open, CurHigh, CurLow);
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

        long nPtr = 0, nBid = 0, nAsk = 0, nClose = 0, nQty = 0;

        if (gTransactionList.count(ProductIdxNo))
        {
            nPtr = gTransactionList[ProductIdxNo][0];
            nBid = gTransactionList[ProductIdxNo][1];
            nAsk = gTransactionList[ProductIdxNo][2];
            nClose = gTransactionList[ProductIdxNo][3];
            nQty = gTransactionList[ProductIdxNo][4];
        }

        printf("Total Offer: [%ld]\n", TotalOffer);

        printf("Ask5: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][9].first, gBest5BidOffer[ProductIdxNo][9].second);
        printf("Ask4: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][8].first, gBest5BidOffer[ProductIdxNo][8].second);
        printf("Ask3: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][7].first, gBest5BidOffer[ProductIdxNo][7].second);
        printf("Ask2: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][6].first, gBest5BidOffer[ProductIdxNo][6].second);
        printf("Ask1: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][5].first, gBest5BidOffer[ProductIdxNo][5].second);
        if (nClose > 0 && nClose >= nAsk)
        {
            printf("============================Close: [%ld]: [%ld]============\n", nClose, nQty);
        }
        printf("***********************************************************************\n");
        if (nClose > 0 && nClose <= nBid)
        {
            printf("============================Close: [%ld]: [%ld]============\n", nClose, nQty);
        }
        printf("Bid1: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][0].first, gBest5BidOffer[ProductIdxNo][0].second);
        printf("Bid2: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][1].first, gBest5BidOffer[ProductIdxNo][1].second);
        printf("Bid3: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][2].first, gBest5BidOffer[ProductIdxNo][2].second);
        printf("Bid4: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][3].first, gBest5BidOffer[ProductIdxNo][3].second);
        printf("Bid5: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][4].first, gBest5BidOffer[ProductIdxNo][4].second);

        printf("Total Bid:   [%ld]\n", TotalBid);

        printf("=========================================\n");
    }
    else
    {
        DEBUG(DEBUG_LEVEL_DEBUG, "gBest5BidOffer[ProductIdxNo].size() < 10");
    }
}

void init()
{
    pSKCenterLib = new CSKCenterLib;
    pSKQuoteLib = new CSKQuoteLib;
    pSKReplyLib = new CSKReplyLib;
    pSKOrderLib = new CSKOrderLib;

    pSKOsQuoteLib = new CSKOSQuoteLib;
}

void release()
{
    delete pSKCenterLib;
    delete pSKQuoteLib;
    delete pSKReplyLib;
    delete pSKOrderLib;

    delete pSKOsQuoteLib;

    CoUninitialize();
}

LONG AutoSetup()
{
    AutoLogIn();

    AutoConnect();

    AutoGetFutureRights();

    long res = pSKQuoteLib->RequestServerTime();

    DEBUG(DEBUG_LEVEL_INFO, "pSKQuoteLib->RequestServerTime()=%d", res);

    res = res | pSKQuoteLib->GetMarketBuySellUpDown();
    DEBUG(DEBUG_LEVEL_INFO, "pSKQuoteLib->GetMarketBuySellUpDown()=%d", res);

    res = res | pSKQuoteLib->GetCommodityIdx();

    res = res | pSKOsQuoteLib->GetCommodityIdx();

    std::string CommList;

    std::ostringstream oss;
    oss << COMMODITY_MAIN << "AM" << "," << COMMODITY_MAIN << "," << "TSEA" << "," << TSMC << "," << MEDIATEK << "," << FOXCONN;
    CommList = oss.str();

    res = res | AutoQuote(CommList, -1);

    res = res | AutoQuoteTicks(TSMC, -1);
    res = res | AutoQuoteTicks(MEDIATEK, -1);
    res = res | AutoQuoteTicks(FOXCONN, -1);

    // For calculate 5MA
    res = res | AutoQuoteTicks(COMMODITY_MAIN, -1);
    res = res | AutoOsQuoteTicks(COMMODITY_OS_MAIN, -1);

    return res;
}

// To do list:
//  (done)
// Estimated trading volume
// need VIX index
// current time (done)
// Instant profit and loss
// Add open position query.
// Add stop loss and profit stop mechanism

// Bug:
// The price will be unstable at the beginning and will change from high to low.

// To do list:
//
// Estimated trading volume
// Instant profit and loss
// need VIX index
// current time
// Estimated trading volume
// Instant profit and loss

// Test code:

// Function to generate random price
// long getRandomPrice()
// {
//     // Generate a random number in the range of 2000000 to 2500000
//     return rand() % 500001 + 2000000; // 500001 is because 2500000 - 2000000 = 500000 + 1
// }

// // Function to update the price periodically (every second)
// void updatePricePeriodically(long MtxCommodtyInfo)
// {
//     // Generate a random price and update the corresponding value in the global variable
//     gCurCommPrice[MtxCommodtyInfo] = getRandomPrice();
// }

void thread_main()
{
    const int refreshInterval = 1000; // 1000 ms
    std::chrono::steady_clock::time_point lastClearTime = std::chrono::steady_clock::now();

    AutoSetup();

    DEBUG(DEBUG_LEVEL_INFO, "[ServerTime: %d: %d: %d]", gCurServerTime[0], gCurServerTime[1], gCurServerTime[2]);
    LOG(DEBUG_LEVEL_INFO, "[ServerTime: %d: %d: %d]", gCurServerTime[0], gCurServerTime[1], gCurServerTime[2]);

    LONG CheckConnected = 0;

    while (true)
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClearTime);

        // Determine whether to use day quotation or full day and night quotation

        LONG MtxCommodtyInfo = 0;

        if (gCurServerTime[0] < 8 || gCurServerTime[0] > 14)
        {
            MtxCommodtyInfo = gCommodtyInfo.MTXIdxNo;
        }
        else
        {
            MtxCommodtyInfo = gCommodtyInfo.MTXIdxNoAM;
        }

        {
            // Key prices start

            AutoCalcuKeyPrices();

            CountWeeklyAndMonthlyCosts(MtxCommodtyInfo); // apply to gCostMovingAverageVal

            if (gCostMovingAverageVal < 0)
            {
                continue;
            }
        }

        // Strategy Start

        // updatePricePeriodically(MtxCommodtyInfo);

        StrategySwitch(gStrategyConfig.StrategyMode, MtxCommodtyInfo);

        // Ouput start

        if (elapsed.count() >= refreshInterval)
        {
            system("cls");
            lastClearTime = now;

            ++CheckConnected;

            if (CheckConnected == 15)
            {
                AutoGetFutureRights();

                while (pSKQuoteLib->IsConnected() != 1 || pSKOsQuoteLib->IsConnected() != 1)
                {
                    LOG(DEBUG_LEVEL_INFO, "pSKQuoteLib->IsConnected() != 1");
                    LONG res = AutoSetup();

                    if (res == 0)
                    {
                        LOG(DEBUG_LEVEL_INFO, "AutoSetup Success");
                        break;
                    }
                }

                CheckConnected = 0;
            }

            printf("[UserId:%s], [StrategyMode:%ld], [ClosingKeyPriceLevel:%ld], [BidOfferLongShortThreshold:%ld], [ActivePoint:%ld], [MaximumLoss:%f]\n",
                   g_strUserId.c_str(), gStrategyConfig.StrategyMode, gStrategyConfig.ClosingKeyPriceLevel,
                   gStrategyConfig.BidOfferLongShortThreshold, gStrategyConfig.ActivePoint, gStrategyConfig.MaximumLoss);
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

            printf("EvaluatePosition: %ld, FloatingProfitLoss: %f", gEvaluatePosition, gFloatingProfitLoss);

            if (gOpenInterestInfo.NeedToUpdate == FALSE && gOpenInterestInfo.openPosition != 0)
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

            printf("BidOfferLongShortSlope: %f, StrategyCaluLongShort: %ld, BidOfferLongShort: %ld, TransactionListLongShort: %ld\n",
                   gBidOfferLongShortSlope, StrategyCaluLongShort(), gBidOfferLongShort, gTransactionListLongShort);

            printf("=========================================\n");

            printf("TSEA Total OFFER: [%ld]\n", gCurTaiexInfo[0][3]);
            printf("            BID : [%ld]\n", gCurTaiexInfo[0][2]);

            printf("=========================================\n");

            AutoBest5Long(gCommodtyInfo.TSMCIdxNo, TSMC);
            AutoBest5Long(gCommodtyInfo.FOXCONNIdxNo, FOXCONN);
            AutoBest5Long(gCommodtyInfo.MediaTekIdxNo, MEDIATEK);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); //  CPU
    }
}

void readConfig()
{
    try
    {
        YAML::Node config = YAML::LoadFile("config.yaml");

        if (config["account"] && config["password"])
        {
            std::string account = config["account"].as<std::string>();
            std::string password = config["password"].as<std::string>();

            DEBUG(DEBUG_LEVEL_INFO, "Account: %s, Password: %s", account.c_str(), password.c_str());

            g_strUserId = account;
            gPwd = password;
        }

        if (config["CLOSING_KEY_PRICE_LEVEL"])
        {
            gStrategyConfig.ClosingKeyPriceLevel = config["CLOSING_KEY_PRICE_LEVEL"].as<LONG>();
        }

        if (config["BID_OFFER_LONG_SHORT_THRESHOLD"])
        {
            gStrategyConfig.BidOfferLongShortThreshold = config["BID_OFFER_LONG_SHORT_THRESHOLD"].as<LONG>();
        }

        if (config["ACTIVITY_POINT"])
        {
            gStrategyConfig.ActivePoint = config["ACTIVITY_POINT"].as<LONG>();
        }

        if (config["MAXIMUM_LOSS"])
        {
            gStrategyConfig.MaximumLoss = config["MAXIMUM_LOSS"].as<DOUBLE>();
        }

        if (config["STRATEGY_MODE"])
        {
            gStrategyConfig.StrategyMode = config["STRATEGY_MODE"].as<LONG>();
        }

        DEBUG(DEBUG_LEVEL_INFO, "Closing Key Price Level: %ld", gStrategyConfig.ClosingKeyPriceLevel);
        DEBUG(DEBUG_LEVEL_INFO, "Bid Offer Long Short Threshold: %ld", gStrategyConfig.BidOfferLongShortThreshold);
        DEBUG(DEBUG_LEVEL_INFO, "Activity Point: %ld", gStrategyConfig.ActivePoint);
        DEBUG(DEBUG_LEVEL_INFO, "Maximum Loss: %f", gStrategyConfig.MaximumLoss);
        DEBUG(DEBUG_LEVEL_INFO, "STRATEGY_MODE: %ld", gStrategyConfig.StrategyMode);
    }
    catch (const YAML::BadFile &e)
    {
        std::cerr << "Failed to load config.yaml: " << e.what() << std::endl;
        system("pause");

        DEBUG(DEBUG_LEVEL_INFO, "Account or Password not found in config.yaml");
        exit(1);
    }
}

int main()
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    readConfig();

    CoInitialize(NULL);

    init();

    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));

    g_nCode = pSKCenterLib->Login(g_strUserId.c_str(), gPwd.c_str());

    pSKCenterLib->PrintfCodeMessage("Center", "Login", g_nCode);

    if (g_nCode != 0)
    {
        return 0;
    }

    SetConsoleMode(hStdin, mode);

    thread tMain(thread_main);
    if (tMain.joinable())
        tMain.detach();

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) // Get SendMessage loop
    {
        DispatchMessageW(&msg);
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "end");

    system("pause");

    return 0;
}
