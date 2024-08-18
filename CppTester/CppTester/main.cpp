#include "SKCenterLib.h"
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
extern bool gEatOffer;
extern std::unordered_map<long, std::array<long, 3>> gCurCommHighLowPoint;
extern SHORT gCurServerTime[3];
extern std::unordered_map<long, long> gCurCommPrice;
extern std::unordered_map<SHORT, std::array<long, 4>> gCurTaiexInfo;
extern std::unordered_map<long, vector<pair<long, long>>> gBest5BidOffer;
extern COMMODITY_INFO gCommodtyInfo;
extern DAY_AMP_AND_KEY_PRICE gDayAmpAndKeyPrice;
extern OpenInterestInfo gOpenInterestInfo;
extern LONG gBidOfferLongShort;
extern double gCostMovingAverageVal;
extern STRATEGY_CONFIG gStrategyConfig;

// Define the global logger instance
Logger logger("debug.log");

CSKCenterLib *pSKCenterLib;
CSKQuoteLib *pSKQuoteLib;
CSKReplyLib *pSKReplyLib;
CSKOrderLib *pSKOrderLib;

long g_nCode = 0;
extern string g_strUserId;
extern string gPwd;

void AutoConnect()
{
    while (pSKQuoteLib->IsConnected() != 1)
    {
        g_nCode = pSKQuoteLib->EnterMonitorLONG();
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

void AutoGetFutureRights()
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

    g_nCode = pSKOrderLib->GetFutureRights(g_strUserId);

    pSKCenterLib->PrintfCodeMessage("AutoGetFutureRights", "GetFutureRights", g_nCode);

    DEBUG(DEBUG_LEVEL_DEBUG, "GetFutureRights res = %d", g_nCode);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

LONG AutoQuote(IN string ProductNum, short sPageNo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

    g_nCode = pSKQuoteLib->RequestStocks(&sPageNo, ProductNum);
    pSKCenterLib->PrintfCodeMessage("Quote", "RequestStocks", g_nCode);
    DEBUG(DEBUG_LEVEL_INFO, "g_nCode= %d", g_nCode);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");

    return g_nCode;
}

void AutoQuoteTicks(IN string ProductNum, short sPageNo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

    g_nCode = pSKQuoteLib->RequestTicks(&sPageNo, ProductNum);

    pSKCenterLib->PrintfCodeMessage("Quote", "RequestTicks", g_nCode);

    DEBUG(DEBUG_LEVEL_INFO, "g_nCode= %d", g_nCode);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
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
    if (gBest5BidOffer[ProductIdxNo].size() >= 10)
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

        long nClose = 0, nQty = 0;

        if (gBest5BidOffer[ProductIdxNo].size() >= 11)
        {
            nClose = gBest5BidOffer[ProductIdxNo][10].first;
            nQty = gBest5BidOffer[ProductIdxNo][10].second;
        }

        printf("Total Offer: [%ld]\n", TotalOffer);

        printf("Ask5: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][9].first, gBest5BidOffer[ProductIdxNo][9].second);
        printf("Ask4: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][8].first, gBest5BidOffer[ProductIdxNo][8].second);
        printf("Ask3: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][7].first, gBest5BidOffer[ProductIdxNo][7].second);
        printf("Ask2: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][6].first, gBest5BidOffer[ProductIdxNo][6].second);
        printf("Ask1: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][5].first, gBest5BidOffer[ProductIdxNo][5].second);
        if (nClose > 0 && nClose >= gBest5BidOffer[ProductIdxNo][5].first)
        {
            printf("Close: [%ld]: [%ld]\n", nClose, nQty);
        }
        printf("=========================================\n");
        if (nClose > 0 && nClose <= gBest5BidOffer[ProductIdxNo][0].first)
        {
            printf("Close: [%ld]: [%ld]\n", nClose, nQty);
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
        DEBUG(DEBUG_LEVEL_INFO, "gBest5BidOffer[ProductIdxNo].size() < 10");
    }
}

void init()
{
    pSKCenterLib = new CSKCenterLib;
    pSKQuoteLib = new CSKQuoteLib;
    pSKReplyLib = new CSKReplyLib;
    pSKOrderLib = new CSKOrderLib;
}

void release()
{
    delete pSKCenterLib;
    delete pSKQuoteLib;
    delete pSKReplyLib;
    delete pSKOrderLib;

    CoUninitialize();
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

void thread_main()
{
    AutoLogIn();

    AutoConnect();

    // AutoGetFutureRights();

    long res = pSKQuoteLib->RequestServerTime();

    DEBUG(DEBUG_LEVEL_INFO, "pSKQuoteLib->RequestServerTime()=%d", res);

    res = pSKQuoteLib->GetMarketBuySellUpDown();
    DEBUG(DEBUG_LEVEL_INFO, "pSKQuoteLib->GetMarketBuySellUpDown()=%d", res);

    pSKQuoteLib->GetCommodityIdx();

    const int refreshInterval = 1000; // 1000
    auto lastClearTime = std::chrono::steady_clock::now();

    std::string CommList;

    std::ostringstream oss;
    oss << COMMODITY_MAIN << "AM" << "," << COMMODITY_MAIN << "," << "TSEA" << "," << "2330" << "," << "2317";
    CommList = oss.str();

    AutoQuote(CommList, 1);

    AutoQuoteTicks("2330", 2);
    AutoQuoteTicks("2317", 3);

    while (gCurServerTime[0] < 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }

    DEBUG(DEBUG_LEVEL_INFO, "[ServerTime: %d: %d: %d]", gCurServerTime[0], gCurServerTime[1], gCurServerTime[2]);

    long PreHigh = 0, PreLow = 0;

    LONG PrintInfoCount = 0;

    while (true)
    {
        if (gCurServerTime[0] < 0)
        {
            continue;
        }

        if (PreHigh == 0 || PreLow == 0)
        {
            if (gCurServerTime[0] < 8 || gCurServerTime[0] > 14)
            {
                if (gCurCommHighLowPoint.count(gCommodtyInfo.MTXIdxNoAM) != 0)
                {
                    PreHigh = gCurCommHighLowPoint[gCommodtyInfo.MTXIdxNoAM][0] / 100;
                    PreLow = gCurCommHighLowPoint[gCommodtyInfo.MTXIdxNoAM][1] / 100;
                }
            }
            else
            {
                if (gCurCommHighLowPoint.count(gCommodtyInfo.MTXIdxNo) != 0)
                {
                    PreHigh = gCurCommHighLowPoint[gCommodtyInfo.MTXIdxNo][0] / 100;
                    PreLow = gCurCommHighLowPoint[gCommodtyInfo.MTXIdxNo][1] / 100;
                }
            }

            if (PreHigh != 0 && PreLow != 0)
            {
                DEBUG(DEBUG_LEVEL_INFO, "PreHigh: %ld, PreLow: %ld", PreHigh, PreLow);
            }

            continue;
        }

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

        AutoCalcuKeyPrices();

        gCostMovingAverageVal = CountCostMovingAverage();

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClearTime);

        {
            // Strategy start:

            StrategyStopFuturesLoss(g_strUserId, MtxCommodtyInfo);
            StrategyClosePosition(g_strUserId, MtxCommodtyInfo);

            if (gCurServerTime[0] < 9 || (gCurServerTime[0] >= 13 && gCurServerTime[1] >= 30) || gCurServerTime[0] >= 14)
            {
                StrategyNewLongShortPosition(g_strUserId, MtxCommodtyInfo, 1);
                StrategyNewLongShortPosition(g_strUserId, MtxCommodtyInfo, 0);
            }
            else
            {
                if (gBidOfferLongShort >= gStrategyConfig.BidOfferLongShortThreshold)
                {
                    StrategyNewLongShortPosition(g_strUserId, MtxCommodtyInfo, 1);
                }
                else if (-gBidOfferLongShort >= gStrategyConfig.BidOfferLongShortThreshold)
                {
                    StrategyNewLongShortPosition(g_strUserId, MtxCommodtyInfo, 0);
                }
            }

            // StrategyNewIntervalAmpLongShortPosition(g_strUserId, MtxCommodtyInfo, 0);

            // Strategy End:
        }

        if (elapsed.count() >= refreshInterval)
        {
            //
            system("cls");

            //
            lastClearTime = now;

            printf("[CurMtxPrice: %ld], ", gCurCommPrice[MtxCommodtyInfo]);
            printf("[ServerTime: %d: %d: %d], ", gCurServerTime[0], gCurServerTime[1], gCurServerTime[2]);
            printf("[TSEA prices: %ld, Valume: %ld: Buy: %ld Sell: %ld]\n",
                   gCurCommPrice[gCommodtyInfo.TSEAIdxNo], gCurTaiexInfo[0][1], gCurTaiexInfo[0][2], gCurTaiexInfo[0][3]);

            printf("=========================================\n");

            if (gCurCommHighLowPoint.count(MtxCommodtyInfo) > 0)
            {

                long CurHigh = gCurCommHighLowPoint[MtxCommodtyInfo][0] / 100;
                long CurLow = gCurCommHighLowPoint[MtxCommodtyInfo][1] / 100;
                long CostMovingAverage = static_cast<long>(CountCostMovingAverage());

                printf("Open: %ld, CurHigh: %ld, CurLow: %ld, CostMovingAverage: %ld, ", gCurCommHighLowPoint[MtxCommodtyInfo][2], CurHigh, CurLow, CostMovingAverage);

                printf("CurAvg: %ld, CurAmp : %ld\n", (CurHigh + CurLow) / 2, CurHigh - CurLow);

                if (PrintInfoCount == 60)
                {
                    LOG(DEBUG_LEVEL_INFO, "Open: %ld, CurHigh: %ld, CurLow: %ld, CostMovingAverage: %ld, ",
                        gCurCommHighLowPoint[MtxCommodtyInfo][2], CurHigh, CurLow, CostMovingAverage);
                    LOG(DEBUG_LEVEL_INFO, "CurAvg: %ld, CurAmp : %ld\n", (CurHigh + CurLow) / 2, CurHigh - CurLow);
                    PrintInfoCount = 0;
                }
                else
                {
                    ++PrintInfoCount;
                }
            }

            printf("=========================================\n");

            if (gOpenInterestInfo.openPosition != 0 || gOpenInterestInfo.dayTradePosition != 0)
            {
                printf("Open Position: %d, AvgCost:%f, ProfitAndLoss: %f\n",
                       gOpenInterestInfo.openPosition,
                       gOpenInterestInfo.avgCost,
                       gOpenInterestInfo.profitAndLoss);

                printf("=========================================\n");
            }

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

            printf("BidOfferLongShort : %ld\n", gBidOfferLongShort);

            printf("=========================================\n");

            AutoBest5Long(gCommodtyInfo.TSMCIdxNo, "TSMC");
            AutoBest5Long(gCommodtyInfo.HHIdxNo, "HHP");

            StrategyCaluBidOfferLongShort();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); //  CPU

        if (pSKQuoteLib->IsConnected() != 1)
        {
            DEBUG(DEBUG_LEVEL_ERROR, "pSKQuoteLib->IsConnected() != 1");

            // AutoConnect();
            release();
            exit(0);
        }
    }
}

void readConfig()
{
    try
    {
        YAML::Node config = YAML::LoadFile("config.yaml");

        if (config["account"])
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

        DEBUG(DEBUG_LEVEL_INFO, "Closing Key Price Level: %ld", gStrategyConfig.ClosingKeyPriceLevel);
        DEBUG(DEBUG_LEVEL_INFO, "Bid Offer Long Short Threshold: %ld", gStrategyConfig.BidOfferLongShortThreshold);
        DEBUG(DEBUG_LEVEL_INFO, "Activity Point: %ld", gStrategyConfig.ActivePoint);
        DEBUG(DEBUG_LEVEL_INFO, "Maximum Loss: %f", gStrategyConfig.MaximumLoss);
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
