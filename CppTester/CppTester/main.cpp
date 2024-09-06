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

#include "socketServer.h"

extern std::deque<long> gDaysKlineDiff;
extern std::unordered_map<long, std::array<long, 4>> gCurCommHighLowPoint;
extern SHORT gCurServerTime[3];
extern std::unordered_map<long, long> gCurCommPrice;
extern std::unordered_map<SHORT, std::array<long, 4>> gCurTaiexInfo;
extern std::unordered_map<long, vector<pair<long, long>>> gBest5BidOffer;
extern std::unordered_map<long, std::array<long, 5>> gTransactionList;
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

long g_nCode = 0;

extern string g_strUserId;
extern string gPwd;

extern char buffer[10240] ;

void release();

void AutoConnect()
{
    long count = 0;

    while (pSKQuoteLib->IsConnected() != 1)
    {
        g_nCode = pSKQuoteLib->EnterMonitorLONG();
        pSKCenterLib->PrintfCodeMessage("Quote", "EnterMonitor", g_nCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(3000)); //  CPU
        ++count;

        if (count == 5)
        {
            DEBUG(DEBUG_LEVEL_ERROR, "pSKQuoteLib->IsConnected() != 1");
            release();
            exit(0);
        }
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

        sprintf(buffer+strlen(buffer),"%s : %ld, ", ProductName.c_str(), gCurCommPrice[ProductIdxNo]);

        sprintf(buffer+strlen(buffer),"Open: %ld, CurHigh: %ld, CurLow: %ld\n", Open, CurHigh, CurLow);
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
                      




        sprintf(buffer+strlen(buffer),"Total Offer: [%ld]\n", TotalOffer);
        sprintf(buffer+strlen(buffer),"Ask5: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][9].first, gBest5BidOffer[ProductIdxNo][9].second);
        sprintf(buffer+strlen(buffer),"Ask4: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][8].first, gBest5BidOffer[ProductIdxNo][8].second);
        sprintf(buffer+strlen(buffer),"Ask3: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][7].first, gBest5BidOffer[ProductIdxNo][7].second);
        sprintf(buffer+strlen(buffer),"Ask2: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][6].first, gBest5BidOffer[ProductIdxNo][6].second);
        sprintf(buffer+strlen(buffer),"Ask1: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][5].first, gBest5BidOffer[ProductIdxNo][5].second);

       
        (buffer+strlen(buffer),"%s : %ld Open: %ld, CurHigh: %ld, CurLow: %ld\nTotal Offer: [%ld]\nAsk5: [%ld]: [%ld]\nAsk4: [%ld]: [%ld]\nAsk3: [%ld]: [%ld]\nAsk2: [%ld]: [%ld]\nAsk1: [%ld]: [%ld]\n",ProductName.c_str(), gCurCommPrice[ProductIdxNo],Open, CurHigh, CurLow,TotalOffer,gBest5BidOffer[ProductIdxNo][9].first, gBest5BidOffer[ProductIdxNo][9].second,gBest5BidOffer[ProductIdxNo][8].first, gBest5BidOffer[ProductIdxNo][8].second,gBest5BidOffer[ProductIdxNo][7].first, gBest5BidOffer[ProductIdxNo][7].second,gBest5BidOffer[ProductIdxNo][6].first, gBest5BidOffer[ProductIdxNo][6].second,gBest5BidOffer[ProductIdxNo][5].first, gBest5BidOffer[ProductIdxNo][5].second);    
        if (nClose > 0 && nClose >= nAsk)
        {
            sprintf(buffer+strlen(buffer),"============================Close: [%ld]: [%ld]============\n", nClose, nQty);
        }
        sprintf(buffer+strlen(buffer),"=========================================\n");
        if (nClose > 0 && nClose <= nBid)
        {
            sprintf(buffer+strlen(buffer),"============================Close: [%ld]: [%ld]============\n", nClose, nQty);
        }

     
        sprintf(buffer+strlen(buffer),"Bid1: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][0].first, gBest5BidOffer[ProductIdxNo][0].second);
        sprintf(buffer+strlen(buffer),"Bid2: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][1].first, gBest5BidOffer[ProductIdxNo][1].second);
        sprintf(buffer+strlen(buffer),"Bid3: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][2].first, gBest5BidOffer[ProductIdxNo][2].second);
        sprintf(buffer+strlen(buffer),"Bid4: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][3].first, gBest5BidOffer[ProductIdxNo][3].second);
        sprintf(buffer+strlen(buffer),"Bid5: [%ld]: [%ld]\n", gBest5BidOffer[ProductIdxNo][4].first, gBest5BidOffer[ProductIdxNo][4].second);
        sprintf(buffer+strlen(buffer),"Total Bid:   [%ld]\n", TotalBid);
        sprintf(buffer+strlen(buffer),"=========================================\n");

       
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
    const int refreshInterval = 1000; // 1000 ms
    std::chrono::steady_clock::time_point lastClearTime = std::chrono::steady_clock::now();
    AutoLogIn();

    AutoConnect();

    // AutoGetFutureRights();

    long res = pSKQuoteLib->RequestServerTime();

    DEBUG(DEBUG_LEVEL_INFO, "pSKQuoteLib->RequestServerTime()=%d", res);

    res = pSKQuoteLib->GetMarketBuySellUpDown();
    DEBUG(DEBUG_LEVEL_INFO, "pSKQuoteLib->GetMarketBuySellUpDown()=%d", res);

    pSKQuoteLib->GetCommodityIdx();

    std::string CommList;

    std::ostringstream oss;
    oss << COMMODITY_MAIN << "AM" << "," << COMMODITY_MAIN << "," << "TSEA" << "," << "2330" << "," << "2317";
    CommList = oss.str();

    AutoQuote(CommList, 1);

    AutoQuoteTicks("2330", 2);
    AutoQuoteTicks("2317", 3);

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));

        system("cls");

        sprintf(buffer+strlen(buffer),"Waiting for host quotation...");

        if (gCurServerTime[0] >= 0 &&
            gCommodtyInfo.MTXIdxNoAM >= 0 &&
            gCommodtyInfo.MTXIdxNo >= 0 &&
            gCurCommHighLowPoint.count(gCommodtyInfo.MTXIdxNoAM) != 0 &&
            gCurCommHighLowPoint.count(gCommodtyInfo.MTXIdxNo) != 0 &&
            gCurCommPrice.count(gCommodtyInfo.MTXIdxNoAM) != 0 &&
            gCurCommPrice.count(gCommodtyInfo.MTXIdxNo) != 0)
        {
            break;
        }
    }

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

        {
            // Strategy start:

            StrategyCaluBidOfferLongShort();
            StrategyCaluTransactionListLongShort();

            StrategyStopFuturesLoss(g_strUserId, MtxCommodtyInfo);
            StrategyClosePosition(g_strUserId, MtxCommodtyInfo);

#if NIGHT_TRADING

            if (gCurServerTime[0] < 8 || gCurServerTime[0] >= 22)
            {
#if STRATEGY_1 == 1
                StrategyNewLongShortPosition(g_strUserId, MtxCommodtyInfo, 1);
                StrategyNewLongShortPosition(g_strUserId, MtxCommodtyInfo, 0);
#endif

#if STRATEGY_2 == 1

                StrategyNewIntervalAmpLongShortPosition(g_strUserId, MtxCommodtyInfo, 1);
                StrategyNewIntervalAmpLongShortPosition(g_strUserId, MtxCommodtyInfo, 0);
#endif
            }
            else
#endif
            {
                if (StrategyCaluLongShort() >= gStrategyConfig.BidOfferLongShortThreshold)
                {
                    StrategyNewLongShortPosition(g_strUserId, MtxCommodtyInfo, 1);
                }
                else if (-StrategyCaluLongShort() >= gStrategyConfig.BidOfferLongShortThreshold)
                {
                    StrategyNewLongShortPosition(g_strUserId, MtxCommodtyInfo, 0);
                }
            }

            // StrategyNewIntervalAmpLongShortPosition(g_strUserId, MtxCommodtyInfo, 0);

            // Strategy End:
        }

        // Ouput start

        if (elapsed.count() >= refreshInterval)
        {
            system("cls");
            lastClearTime = now;

            ++CheckConnected;

            if (CheckConnected == 30)
            {
                if (pSKQuoteLib->IsConnected() != 1)
                {
                    DEBUG(DEBUG_LEVEL_ERROR, "pSKQuoteLib->IsConnected() != 1");
                    release();
                    exit(0);
                }

                CheckConnected = 0;
            }

            sprintf(buffer+strlen(buffer),"[UserId:%s], [LongShortThreshold:%ld], [BidOfferLongShortThreshold:%ld], [ActivePoint:%ld], [MaximumLoss:%f]\n",
                   g_strUserId.c_str(), gStrategyConfig.ClosingKeyPriceLevel, gStrategyConfig.BidOfferLongShortThreshold, gStrategyConfig.ActivePoint, gStrategyConfig.MaximumLoss);
            sprintf(buffer+strlen(buffer),"=========================================\n");
            sprintf(buffer+strlen(buffer),"[CurMtxPrice: %ld] ", gCurCommPrice[MtxCommodtyInfo] / 100);
            sprintf(buffer+strlen(buffer),"[TSEA prices: %ld, Valume: %ld] ",
                   gCurCommPrice[gCommodtyInfo.TSEAIdxNo] / 100, gCurTaiexInfo[0][1]);
            sprintf(buffer+strlen(buffer),"[Diff: %d] ", (gCurCommPrice[MtxCommodtyInfo] - gCurCommPrice[gCommodtyInfo.TSEAIdxNo]) / 100);
            sprintf(buffer+strlen(buffer),"[ServerTime: %d: %d: %d]\n", gCurServerTime[0], gCurServerTime[1], gCurServerTime[2]);

            sprintf(buffer+strlen(buffer),"=========================================\n");

            long CurHigh = gCurCommHighLowPoint[MtxCommodtyInfo][0] / 100;
            long CurLow = gCurCommHighLowPoint[MtxCommodtyInfo][1] / 100;
            long CostMovingAverage = static_cast<long>(gCostMovingAverageVal);
            if (gCurCommHighLowPoint.count(MtxCommodtyInfo) > 0)
            {

                // long CurHigh = gCurCommHighLowPoint[MtxCommodtyInfo][0] / 100;
                // long CurLow = gCurCommHighLowPoint[MtxCommodtyInfo][1] / 100;
                // long CostMovingAverage = static_cast<long>(gCostMovingAverageVal);
                sprintf(buffer+strlen(buffer),"Open: %ld, CurHigh: %ld, CurLow: %ld, CostMovingAverage: %ld, ", gCurCommHighLowPoint[MtxCommodtyInfo][2], CurHigh, CurLow, CostMovingAverage);
                sprintf(buffer+strlen(buffer),"CurAvg: %ld, CurAmp : %ld\n", (CurHigh + CurLow) / 2, CurHigh - CurLow);
            }

            sprintf(buffer+strlen(buffer),"=========================================\n");


           
            if (gOpenInterestInfo.NeedToUpdate == FALSE && gOpenInterestInfo.openPosition != 0)
            {
                sprintf(buffer+strlen(buffer),"Open Position: %d, AvgCost:%f, ProfitAndLoss: %f\n",
                       gOpenInterestInfo.openPosition,
                       gOpenInterestInfo.avgCost,
                       gOpenInterestInfo.profitAndLoss);

                sprintf(buffer+strlen(buffer),"=========================================\n");
            }

            sprintf(buffer+strlen(buffer),"Long Key 5: %ld\n", gDayAmpAndKeyPrice.LongKey5);
            sprintf(buffer+strlen(buffer),"Long Key 4: %ld\n", gDayAmpAndKeyPrice.LongKey4);
            sprintf(buffer+strlen(buffer),"Long Key 3: %ld\n", gDayAmpAndKeyPrice.LongKey3);
            sprintf(buffer+strlen(buffer),"Long Key 2: %ld\n", gDayAmpAndKeyPrice.LongKey2);
            sprintf(buffer+strlen(buffer),"Long Key 1: %ld\n", gDayAmpAndKeyPrice.LongKey1);
            sprintf(buffer+strlen(buffer),"=========================================\n");
            sprintf(buffer+strlen(buffer),"Short Key 1: %ld\n", gDayAmpAndKeyPrice.ShortKey1);
            sprintf(buffer+strlen(buffer),"Short Key 2: %ld\n", gDayAmpAndKeyPrice.ShortKey2);
            sprintf(buffer+strlen(buffer),"Short Key 3: %ld\n", gDayAmpAndKeyPrice.ShortKey3);
            sprintf(buffer+strlen(buffer),"Short Key 4: %ld\n", gDayAmpAndKeyPrice.ShortKey4);
            sprintf(buffer+strlen(buffer),"Short Key 5: %ld\n", gDayAmpAndKeyPrice.ShortKey5);

            sprintf(buffer+strlen(buffer),"=========================================\n");

            sprintf(buffer+strlen(buffer),"SmallestAmp : %ld, ", gDayAmpAndKeyPrice.SmallestAmp);
            sprintf(buffer+strlen(buffer),"SmallAmp : %ld, ", gDayAmpAndKeyPrice.SmallAmp);
            sprintf(buffer+strlen(buffer),"AvgAmp : %ld, ", gDayAmpAndKeyPrice.AvgAmp);
            sprintf(buffer+strlen(buffer),"LargerAmp : %ld, ", gDayAmpAndKeyPrice.LargerAmp);
            sprintf(buffer+strlen(buffer),"LargestAmp : %ld\n", gDayAmpAndKeyPrice.LargestAmp);

            sprintf(buffer+strlen(buffer),"=========================================\n");

            sprintf(buffer+strlen(buffer),"[LongShortThreshold:%ld], StrategyCaluLongShort:%ld, BidOfferLongShort:%ld, TransactionListLongShort:%ld\n",
                   gStrategyConfig.BidOfferLongShortThreshold, StrategyCaluLongShort(), gBidOfferLongShort, gTransactionListLongShort);

            sprintf(buffer+strlen(buffer),"=========================================\n");

            // snsprintf(buffer+strlen(buffer),buffer, sizeof(buffer),"[UserId:%s], [LongShortThreshold:%ld], [BidOfferLongShortThreshold:%ld], [ActivePoint:%ld], [MaximumLoss:%f]\n=========================================\n[CurMtxPrice: %ld],[TSEA prices: %ld, Valume: %ld],[Diff: %d],[ServerTime: %d: %d: %d]\n=========================================\nOpen: %ld, CurHigh: %ld, CurLow: %ld, CostMovingAverage: %ld,CurAvg: %ld, CurAmp : %ld\n=========================================\nOpen Position: %d, AvgCost:%f, ProfitAndLoss: %f\n=========================================\nLong Key 5: %ld\nLong Key 4: %ld\nLong Key 3: %ld\nLong Key 2: %ld\nLong Key 1: %ld\n=========================================\nShort Key 1: %ld\nShort Key 2: %ld\nShort Key 3: %ld\nShort Key 4: %ld\nShort Key 5: %ld\n=========================================\nSmallestAmp : %ld, SmallAmp : %ld,AvgAmp : %ld, LargerAmp : %ld,LargestAmp : %ld\n=========================================\n[LongShortThreshold:%ld], StrategyCaluLongShort:%ld, BidOfferLongShort:%ld, TransactionListLongShort:%ld\n=========================================\n",
            //                                                 g_strUserId.c_str(), gStrategyConfig.ClosingKeyPriceLevel, gStrategyConfig.BidOfferLongShortThreshold, gStrategyConfig.ActivePoint, gStrategyConfig.MaximumLoss,gCurCommPrice[MtxCommodtyInfo] / 100,gCurCommPrice[gCommodtyInfo.TSEAIdxNo] / 100, gCurTaiexInfo[0][1],(gCurCommPrice[MtxCommodtyInfo] - gCurCommPrice[gCommodtyInfo.TSEAIdxNo]) / 100,gCurServerTime[0], gCurServerTime[1], gCurServerTime[2],gCurCommHighLowPoint[MtxCommodtyInfo][2], CurHigh, CurLow, CostMovingAverage,(CurHigh + CurLow) / 2, CurHigh - CurLow,gOpenInterestInfo.openPosition,
            //                                 gOpenInterestInfo.avgCost,
            //                                 gOpenInterestInfo.profitAndLoss,gDayAmpAndKeyPrice.LongKey5,gDayAmpAndKeyPrice.LongKey4,gDayAmpAndKeyPrice.LongKey3,gDayAmpAndKeyPrice.LongKey2,gDayAmpAndKeyPrice.LongKey1,gDayAmpAndKeyPrice.ShortKey1,gDayAmpAndKeyPrice.ShortKey2,gDayAmpAndKeyPrice.ShortKey3,gDayAmpAndKeyPrice.ShortKey4,gDayAmpAndKeyPrice.ShortKey5,gDayAmpAndKeyPrice.SmallestAmp,gDayAmpAndKeyPrice.SmallAmp,gDayAmpAndKeyPrice.AvgAmp,gDayAmpAndKeyPrice.LargerAmp,gDayAmpAndKeyPrice.LargestAmp,gStrategyConfig.BidOfferLongShortThreshold, StrategyCaluLongShort(), gBidOfferLongShort, gTransactionListLongShort);
            AutoBest5Long(gCommodtyInfo.TSMCIdxNo, "TSMC");
            AutoBest5Long(gCommodtyInfo.HHIdxNo, "HHP");
          
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

    thread tMain2(thread_socket);
    if (tMain2.joinable())
        tMain2.detach();


    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) // Get SendMessage loop
    {
        DispatchMessageW(&msg);
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "end");

    system("pause");

    return 0;
}
