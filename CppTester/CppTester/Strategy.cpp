#include "Strategy.h"
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

#define STOP_POINT 200

// Define the global logger instance
Logger StrategyLog("Strategy.log");

// Object pointer

extern CSKCenterLib *pSKCenterLib;
extern CSKQuoteLib *pSKQuoteLib;
extern CSKReplyLib *pSKReplyLib;
extern CSKOrderLib *pSKOrderLib;

// Global variables, initialized by main, and continuously updated by the com server
extern SHORT gCurServerTime[3];
extern std::unordered_map<SHORT, std::array<long, 4>> gCurTaiexInfo;
extern std::deque<long> gDaysKlineDiff;
extern std::unordered_map<long, std::array<long, 3>> gCurCommHighLowPoint;
extern std::unordered_map<long, long> gCurCommPrice;
extern std::unordered_map<long, vector<pair<long, long>>> gBest5BidOffer;

extern OpenInterestInfo gOpenInterestInfo;
extern OpenInterestInfo gLocalOpenInterestInfo;

extern string g_strUserId;

extern COMMODITY_INFO gCommodtyInfo;

DAY_AMP_AND_KEY_PRICE gDayAmpAndKeyPrice = {0};

void AutoKLineData(IN string ProductNum)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

    long g_nCode = pSKQuoteLib->RequestKLine(ProductNum);

    DEBUG(DEBUG_LEVEL_DEBUG, "g_nCode=%ld", g_nCode);

    pSKCenterLib->PrintfCodeMessage("Quote", "RequestKLine", g_nCode);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

VOID AutoCalcuKeyPrices(LONG nStockidx)
{
    if (gDaysKlineDiff.size() < 20)
    {
        AutoKLineData(COMMODITY_TX_MAIN);

        pSKQuoteLib->ProcessDaysOrNightCommHighLowPoint();

        long accu = 0;
        long AvgAmp = 0, LargestAmp = LONG_MIN, SmallestAmp = LONG_MAX, LargerAmp = 0, SmallAmp = 0;

        for (int i = 0; i < gDaysKlineDiff.size(); ++i)
        {
            DEBUG(DEBUG_LEVEL_INFO, "Diff = %ld ", gDaysKlineDiff[i]);

            accu += gDaysKlineDiff[i];

            LargestAmp = max(LargestAmp, gDaysKlineDiff[i]);
            SmallestAmp = min(SmallestAmp, gDaysKlineDiff[i]);
        }

        AvgAmp = accu / DayMA;

        LargerAmp = (AvgAmp + LargestAmp) / 2;
        SmallAmp = (AvgAmp + SmallestAmp) / 2;

        DEBUG(DEBUG_LEVEL_INFO, "SmallestAmp : %ld", SmallestAmp);
        DEBUG(DEBUG_LEVEL_INFO, "SmallAmp : %ld", SmallAmp);
        DEBUG(DEBUG_LEVEL_INFO, "AvgAmp : %ld", AvgAmp);
        DEBUG(DEBUG_LEVEL_INFO, "LargerAmp : %ld", LargerAmp);
        DEBUG(DEBUG_LEVEL_INFO, "LargestAmp : %ld", LargestAmp);

        gDayAmpAndKeyPrice.SmallestAmp = SmallestAmp;
        gDayAmpAndKeyPrice.SmallAmp = SmallAmp;
        gDayAmpAndKeyPrice.AvgAmp = AvgAmp;
        gDayAmpAndKeyPrice.LargerAmp = LargerAmp;
        gDayAmpAndKeyPrice.LargestAmp = LargestAmp;
    }

    if (gCurCommHighLowPoint.count(nStockidx) > 0)
    {
        long CurHigh = gCurCommHighLowPoint[nStockidx][0] / 100;
        long CurLow = gCurCommHighLowPoint[nStockidx][1] / 100;

        DEBUG(DEBUG_LEVEL_DEBUG, "MTXIdxNo: %ld. High: %ld, Low: %ld", nStockidx, CurHigh, CurLow);

        gDayAmpAndKeyPrice.LongKey5 = CurLow + gDayAmpAndKeyPrice.LargestAmp;
        gDayAmpAndKeyPrice.LongKey4 = CurLow + gDayAmpAndKeyPrice.LargerAmp;
        gDayAmpAndKeyPrice.LongKey3 = CurLow + gDayAmpAndKeyPrice.AvgAmp;
        gDayAmpAndKeyPrice.LongKey2 = CurLow + gDayAmpAndKeyPrice.SmallAmp;
        gDayAmpAndKeyPrice.LongKey1 = CurLow + gDayAmpAndKeyPrice.SmallestAmp;

        gDayAmpAndKeyPrice.ShortKey5 = CurHigh - gDayAmpAndKeyPrice.LargestAmp;
        gDayAmpAndKeyPrice.ShortKey4 = CurHigh - gDayAmpAndKeyPrice.LargerAmp;
        gDayAmpAndKeyPrice.ShortKey3 = CurHigh - gDayAmpAndKeyPrice.AvgAmp;
        gDayAmpAndKeyPrice.ShortKey2 = CurHigh - gDayAmpAndKeyPrice.SmallAmp;
        gDayAmpAndKeyPrice.ShortKey1 = CurHigh - gDayAmpAndKeyPrice.SmallestAmp;
    }
}

/**
 * @brief
 *
 * struct FUTUREORDER
{
    BSTR bstrFullAccount; // Full account number, branch code + 7-digit account number
    BSTR bstrStockNo;     // Future option code
    SHORT sTradeType;     // 0: ROD, 1: IOC, 2: FOK
    SHORT sBuySell;       // 0: Buy, 1: Sell
    SHORT sDayTrade;      // Day trade, 0: No, 1: Yes. Refer to exchange regulations for tradable items.
    SHORT sNewClose;      // New position or close position, 0: New position, 1: Close position, 2: Auto (used for new futures and options)
    BSTR bstrPrice;       // Order price (for IOC and FOK, use "M" for market price, "P" for range market price)
    LONG nQty;            // Number of contracts
    SHORT sReserved;      // {For SendFutureOrderCLR futures order} Trading session, 0: Intraday (T session and T+1 session), 1: T session reservation
};

 * long CSKOrderLib::SendFutureOrder(
 * string strLogInID,
 * bool bAsyncOrder,
 * string strStockNo,
 * short sTradeType,
 * short sBuySell,
 * short sDayTrade,
 * short sNewClose,
 * string strPrice,
 * long nQty,
 * short sReserved)
 * @param[in]
 * @param[in]
 * @return
 * @exception
 */

LONG AutoOrder(IN string ProductNum, IN SHORT NewClose, IN SHORT BuySell)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

    long g_nCode = 0;
    // = pSKOrderLib->SendFutureOrder(g_strUserId,
    //                                             false, // bAsyncOrder
    //                                             ProductNum,
    //                                             1,        // IOC
    //                                             BuySell,  // BuySell
    //                                             0,        // DayTrade
    //                                             NewClose, // NewClose // 0: 1: 2:{}
    //                                             "P",
    //                                             1,
    //                                             0);

    pSKCenterLib->PrintfCodeMessage("AutoOrder", "SendFutureOrder", g_nCode);

    LOG(DEBUG_LEVEL_INFO, "SendFutureOrder res = %d, ProductNum=%s, NewClose=%d, BuySell=%d",
        g_nCode, ProductNum, NewClose, BuySell);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");

    return g_nCode;
}

VOID StrategyStopFuturesLoss(string strUserId)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    // if over loss then do GetOpenInterest again in order to make sure
    // AutoOrder wont place a closing order with the wrong amount
    // else continue to calculate profit and loss and update to global variables
    double profitAndLoss = 0;
    double curPrice = 0;

    if (gCurCommPrice.count(gCommodtyInfo.MTXIdxNo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[gCommodtyInfo.MTXIdxNo]) / 100;
    }

    if (gOpenInterestInfo.product != "" && gOpenInterestInfo.avgCost != 0 && curPrice > 0)
    {
        LOG(DEBUG_LEVEL_INFO, "product: %s", gOpenInterestInfo.product);
        LOG(DEBUG_LEVEL_INFO, "buySell: %s", gOpenInterestInfo.buySell);
        LOG(DEBUG_LEVEL_INFO, "openPosition: %ld", gOpenInterestInfo.openPosition);
        LOG(DEBUG_LEVEL_INFO, "dayTradePosition: %ld", gOpenInterestInfo.dayTradePosition);
        LOG(DEBUG_LEVEL_INFO, "avgCost: %f", gOpenInterestInfo.avgCost);

        SHORT BuySell = -1;

        if (gOpenInterestInfo.buySell == "S")
        {
            profitAndLoss = (gOpenInterestInfo.avgCost - curPrice) * DOLLARS_PER_TICK;
            BuySell = 1; // short position
        }
        else
        {
            profitAndLoss = (curPrice - gOpenInterestInfo.avgCost) * DOLLARS_PER_TICK;
            BuySell = 0; // long position
        }

        LOG(DEBUG_LEVEL_INFO, "curPrice = %f, gOpenInterestInfo.avgCost= %f, profit and loss:%f",
            curPrice, gOpenInterestInfo.avgCost, profitAndLoss);

        if (profitAndLoss >= MAXIMUM_LOSS)
        {
            vector<string> vec = {COMMODITY_MAIN, COMMODITY_OTHER};

            for (auto &x : vec)
            {
                AutoOrder(x,
                          ORDER_CLOSE_POSITION, // Close
                          BuySell               // Buy or sell
                );
            }

            {
                gOpenInterestInfo.product = "";
                gOpenInterestInfo.buySell = "";
                gOpenInterestInfo.openPosition = 0;
                gOpenInterestInfo.avgCost = 0;
            }

            pSKOrderLib->GetOpenInterest(strUserId, 1);
        }
    }
    else
    {
        pSKOrderLib->GetOpenInterest(strUserId, 1);
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
}

VOID StrategyClosePosition(string strUserId)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    double curPrice = 0;

    if (gCurCommPrice.count(gCommodtyInfo.MTXIdxNo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[gCommodtyInfo.MTXIdxNo]) / 100;
    }

    if (gOpenInterestInfo.product != "" && gOpenInterestInfo.avgCost != 0 && curPrice > 0)
    {
        LOG(DEBUG_LEVEL_INFO, "product: %s", gOpenInterestInfo.product);
        LOG(DEBUG_LEVEL_INFO, "buySell: %s", gOpenInterestInfo.buySell);
        LOG(DEBUG_LEVEL_INFO, "openPosition: %ld", gOpenInterestInfo.openPosition);
        LOG(DEBUG_LEVEL_INFO, "dayTradePosition: %ld", gOpenInterestInfo.dayTradePosition);
        LOG(DEBUG_LEVEL_INFO, "avgCost: %f", gOpenInterestInfo.avgCost);

        LOG(DEBUG_LEVEL_INFO, "curPrice = %f, gOpenInterestInfo.avgCost= %f",
            curPrice, gOpenInterestInfo.avgCost);

        SHORT BuySell = -1;

        if (gOpenInterestInfo.buySell == "S")
        {
            BuySell = 1; // short position
        }
        else
        {
            BuySell = 0; // long position
        }

        if ((BuySell == 0 && gDayAmpAndKeyPrice.LongKey3 > 0 && curPrice >= gDayAmpAndKeyPrice.LongKey3) ||
            (BuySell == 1 && gDayAmpAndKeyPrice.ShortKey3 > 0 && curPrice <= gDayAmpAndKeyPrice.ShortKey3))
        {
            vector<string> vec = {COMMODITY_OTHER};

            for (auto &x : vec)
            {
                AutoOrder(x,
                          ORDER_CLOSE_POSITION, // Close
                          BuySell               // Buy or sell
                );
            }

            {
                gOpenInterestInfo.product = "";

                gOpenInterestInfo.buySell = "";

                gOpenInterestInfo.openPosition = 0;
                gOpenInterestInfo.avgCost = 0;
            }

            pSKOrderLib->GetOpenInterest(strUserId, 1);
        }
    }
    else
    {
        pSKOrderLib->GetOpenInterest(strUserId, 1);
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
}

VOID StrategyNewPosition(string strUserId)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    double OpenPrice = 0;

    if (gCurCommHighLowPoint.count(gCommodtyInfo.MTXIdxNo) == 0)
    {
        return;
    }
    else
    {
        OpenPrice = static_cast<double>(gCurCommHighLowPoint[gCommodtyInfo.MTXIdxNo][2]) / 100;
    }

    double curPrice = 0;

    if (gCurCommPrice.count(gCommodtyInfo.MTXIdxNo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[gCommodtyInfo.MTXIdxNo]) / 100;
    }

    if (gOpenInterestInfo.product == "" &&
        gOpenInterestInfo.avgCost == 0 &&
        gOpenInterestInfo.openPosition == 0 &&
        gOpenInterestInfo.dayTradePosition == 0 &&
        curPrice > 0)
    {
        DEBUG(DEBUG_LEVEL_DEBUG, "curPrice = %f, gOpenInterestInfo.avgCost= %f",
              curPrice, gOpenInterestInfo.avgCost);

        SHORT BuySell = -1;
        BOOL LongShortKStickMatch = FALSE;

        if (gDayAmpAndKeyPrice.LongKey1 > 0 && curPrice >= gDayAmpAndKeyPrice.LongKey1 && curPrice <= gDayAmpAndKeyPrice.LongKey1 + STOP_POINT)
        {

            BuySell = 0; // Long position

            if (curPrice > OpenPrice)
            {
                LOG(DEBUG_LEVEL_INFO, "curPrice = %f > Open price: %f", curPrice, OpenPrice);
                LOG(DEBUG_LEVEL_INFO, "New Long position, curPrice = %f, gDayAmpAndKeyPrice.LongKey1= %ld",
                    curPrice, gDayAmpAndKeyPrice.LongKey1);
                LongShortKStickMatch = TRUE;
            }
        }
        else if (gDayAmpAndKeyPrice.ShortKey1 > 0 && curPrice <= gDayAmpAndKeyPrice.ShortKey1 && curPrice >= gDayAmpAndKeyPrice.ShortKey1 - STOP_POINT)
        {

            BuySell = 1; // Short position

            if (curPrice < OpenPrice)
            {
                LOG(DEBUG_LEVEL_INFO, "curPrice = %f < Open price: %f", curPrice, OpenPrice);
                LOG(DEBUG_LEVEL_INFO, "New Short position, curPrice = %f, gDayAmpAndKeyPrice.ShortKey1= %ld",
                    curPrice, gDayAmpAndKeyPrice.ShortKey1);

                LongShortKStickMatch = TRUE;
            }
        }

        if (BuySell != -1 && LongShortKStickMatch == TRUE)
        {
            vector<string> vec = {COMMODITY_OTHER};

            for (auto &x : vec)
            {
                AutoOrder(x,
                          ORDER_CLOSE_POSITION, // Close
                          BuySell               // Buy or sell
                );
            }

            {
                gOpenInterestInfo.product = COMMODITY_OTHER;

                if (BuySell == 1)
                {
                    gOpenInterestInfo.buySell = "S";
                }
                else if (BuySell == 0)
                {
                    gOpenInterestInfo.buySell = "B";
                }
                else
                {
                    gOpenInterestInfo.buySell = "";
                }

                gOpenInterestInfo.openPosition += 1;
                gOpenInterestInfo.avgCost = curPrice;
            }

            pSKOrderLib->GetOpenInterest(strUserId, 1);
        }
    }
    else
    {
        pSKOrderLib->GetOpenInterest(strUserId, 1);
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
}
