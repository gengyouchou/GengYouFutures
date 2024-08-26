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
#include <map>
#include <thread> // For std::this_thread::sleep_for
#include <unordered_map>

// Define the global logger instance
Logger StrategyLog("Strategy");

// Object pointer

extern CSKCenterLib *pSKCenterLib;
extern CSKQuoteLib *pSKQuoteLib;
extern CSKReplyLib *pSKReplyLib;
extern CSKOrderLib *pSKOrderLib;

// Global variables, initialized by main, and continuously updated by the com server
extern SHORT gCurServerTime[3];
extern std::unordered_map<SHORT, std::array<long, 4>> gCurTaiexInfo;
extern std::deque<long> gDaysKlineDiff;
extern std::deque<long> gCostMovingAverage;
extern std::unordered_map<long, std::array<long, 4>> gCurCommHighLowPoint;
extern std::unordered_map<long, long> gCurCommPrice;
extern std::unordered_map<long, vector<pair<long, long>>> gBest5BidOffer;

extern OpenInterestInfo gOpenInterestInfo;

extern string g_strUserId;

extern COMMODITY_INFO gCommodtyInfo;

extern std::map<string, pair<double, double>> gDaysCommHighLowPoint;         // Max len: DAY_NIGHT_HIGH_LOW_K_LINE
extern std::map<string, pair<double, double>> gDaysNightAllCommHighLowPoint; // Max len: DAY_NIGHT_HIGH_LOW_K_LINE

DAY_AMP_AND_KEY_PRICE gDayAmpAndKeyPrice = {0};
BID_OFFER_LONG_AND_SHORT gBidOfferLongAndShort = {0};
LONG gBidOfferLongShort = 0, gTransactionListLongShort = 0;
double gCostMovingAverageVal = 0;

STRATEGY_CONFIG gStrategyConfig = {
    CLOSING_KEY_PRICE_LEVEL,
    BID_OFFER_LONG_SHORT_THRESHOLD,
    ACTIVITY_POINT,
    MAXIMUM_LOSS

};

LONG EstimatedLongSideKeyPrice(VOID);
LONG EstimatedShortSideKeyPrice(VOID);

void AutoKLineData(IN string ProductNum)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

    long g_nCode = pSKQuoteLib->RequestKLine(ProductNum);

    DEBUG(DEBUG_LEVEL_DEBUG, "g_nCode=%ld", g_nCode);

    pSKCenterLib->PrintfCodeMessage("Quote", "RequestKLine", g_nCode);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

DOUBLE CountCostMovingAverage(VOID)
{

    static bool init = FALSE;
    static double LocalCostMovingAverageVal = 0;
    static double count = 0;

    if (init == FALSE)
    {

        // Calculate CostMovingAverage

        for (const auto &entry : gDaysNightAllCommHighLowPoint) // need ordered by date  from the past to the present
        {
            auto cur = entry.second;

            long Avg = static_cast<long>((cur.first + cur.second) / 2.0);

            DEBUG(DEBUG_LEVEL_DEBUG, "Date: %s, High: %f, Low: %f, Avg: %ld", entry.first, cur.first, cur.second, Avg);

            gCostMovingAverage.push_back(Avg);

            if (gCostMovingAverage.size() > COST_DAY_MA)
            {
                gCostMovingAverage.pop_front();
            }
        }

        std::deque<long> tempCostMovingAverage;

        for (const auto &entry : gDaysCommHighLowPoint) // need ordered by date  from the past to the present
        {
            auto cur = entry.second;

            long Avg = static_cast<long>((cur.first + cur.second) / 2.0);

            DEBUG(DEBUG_LEVEL_DEBUG, "Date: %s, High: %f, Low: %f, Avg: %ld", entry.first, cur.first, cur.second, Avg);

            tempCostMovingAverage.push_back(Avg);

            if (tempCostMovingAverage.size() > COST_DAY_MA)
            {
                tempCostMovingAverage.pop_front();
            }
        }

        if (gCostMovingAverage.empty() || tempCostMovingAverage.empty())
        {
            return -1;
        }

        for (auto &Avg : gCostMovingAverage)
        {
            DEBUG(DEBUG_LEVEL_INFO, "Avg = %ld", Avg);

            ++count;

            LocalCostMovingAverageVal += static_cast<double>(Avg);
        }

        for (auto &Avg : tempCostMovingAverage)
        {
            DEBUG(DEBUG_LEVEL_INFO, "Avg = %ld", Avg);

            ++count;

            LocalCostMovingAverageVal += static_cast<double>(Avg);
        }

        init = TRUE;
    }

    if (count != 0)
    {
        double latestClosingPriceAvg = 0;
        gCostMovingAverageVal = LocalCostMovingAverageVal;
        double CurCount = 0;

        if (gCostMovingAverageVal != 0)
        {
            if (gCurCommPrice.count(gCommodtyInfo.MTXIdxNoAM))
            {
                latestClosingPriceAvg = latestClosingPriceAvg + static_cast<double>(gCurCommPrice[gCommodtyInfo.MTXIdxNoAM]) / 100.0;
                ++CurCount;
            }

            if (gCurCommPrice.count(gCommodtyInfo.MTXIdxNo))
            {
                latestClosingPriceAvg = latestClosingPriceAvg + static_cast<double>(gCurCommPrice[gCommodtyInfo.MTXIdxNo]) / 100.0;
                ++CurCount;
            }

            latestClosingPriceAvg = latestClosingPriceAvg / CurCount;
        }

        if (CurCount != 0)
        {
            gCostMovingAverageVal = (gCostMovingAverageVal + latestClosingPriceAvg) / (count + 1);
        }

        DEBUG(DEBUG_LEVEL_DEBUG, "gCostMovingAverageVal = %f", gCostMovingAverageVal);
    }

    return gCostMovingAverageVal;
}

VOID AutoCalcuKeyPrices(VOID)
{

    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

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

    size_t len = gDaysKlineDiff.size();

    if (len == 0)
    {
        pSKQuoteLib->ProcessDaysOrNightCommHighLowPoint();

        len = gDaysKlineDiff.size();

        if (len == 0)
        {
            std::cerr << "Failed to get key prices" << std::endl;
            system("pause");
            return;
        }

        long accu = 0;
        long AvgAmp = 0, LargestAmp = LONG_MIN, SmallestAmp = LONG_MAX, LargerAmp = 0, SmallAmp = 0;

        for (size_t i = 0; i < len; ++i)
        {
            DEBUG(DEBUG_LEVEL_INFO, "Diff = %ld ", gDaysKlineDiff[i]);

            accu += gDaysKlineDiff[i];

            LargestAmp = max(LargestAmp, gDaysKlineDiff[i]);
            SmallestAmp = min(SmallestAmp, gDaysKlineDiff[i]);
        }

        AvgAmp = accu / static_cast<long>(len);

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

    if (gCurCommHighLowPoint.count(MtxCommodtyInfo) > 0)
    {
        long CurHigh = gCurCommHighLowPoint[MtxCommodtyInfo][0] / 100;
        long CurLow = gCurCommHighLowPoint[MtxCommodtyInfo][1] / 100;

        DEBUG(DEBUG_LEVEL_DEBUG, "MTXIdxNo: %ld. High: %ld, Low: %ld", MtxCommodtyInfo, CurHigh, CurLow);

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

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
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

    if (NewClose == ORDER_NEW_POSITION && gOpenInterestInfo.NeedToUpdate == FALSE)
    {
        LOG(DEBUG_LEVEL_INFO, "gOpenInterestInfo.NeedToUpdate == FALSE");
        gOpenInterestInfo.NeedToUpdate = TRUE;
    }

#ifdef VIRTUAL_ACCOUNT_ORDER
    NewClose = ORDER_CLOSE_POSITION;
#endif

    long g_nCode = pSKOrderLib->SendFutureOrder(g_strUserId,
                                                false, // bAsyncOrder
                                                ProductNum,
                                                1,        // IOC
                                                BuySell,  // BuySell
                                                0,        // DayTrade
                                                NewClose, // New position or close position, 0: New position, 1: Close position, 2: Auto (used for new futures and options)
                                                "P",      //"P" for range market price
                                                1,        // Number of contracts
                                                0         // 0: Intraday (T session and T+1 session)
    );

    pSKCenterLib->PrintfCodeMessage("AutoOrder", "SendFutureOrder", g_nCode);

    LOG(DEBUG_LEVEL_INFO, "SendFutureOrder res = %d, ProductNum=%s, NewClose=%d, BuySell=%d",
        g_nCode, ProductNum, NewClose, BuySell);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");

    return g_nCode;
}

VOID StrategyStopFuturesLoss(string strUserId, LONG MtxCommodtyInfo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    // if over loss then do GetOpenInterest again in order to make sure
    // AutoOrder wont place a closing order with the wrong amount
    // else continue to calculate profit and loss and update to global variables
    double profitAndLoss = 0;
    double curPrice = 0;

#ifndef VIRTUAL_ACCOUNT_ORDER

    long res = pSKOrderLib->GetOpenInterest(strUserId, 1);

    if (res != 0)
    {
        DEBUG(DEBUG_LEVEL_DEBUG, "pSKOrderLib->GetOpenInterest(strUserId, 1)=%ld", res);
    }

#endif

    if (gCurCommPrice.count(MtxCommodtyInfo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[MtxCommodtyInfo]) / 100.0;
    }

    if (gOpenInterestInfo.product != "" && gOpenInterestInfo.avgCost != 0 && curPrice > 0)
    {
        LOG(DEBUG_LEVEL_DEBUG, "product: %s", gOpenInterestInfo.product);
        LOG(DEBUG_LEVEL_DEBUG, "buySell: %s", gOpenInterestInfo.buySell);
        LOG(DEBUG_LEVEL_DEBUG, "openPosition: %ld", gOpenInterestInfo.openPosition);
        LOG(DEBUG_LEVEL_DEBUG, "dayTradePosition: %ld", gOpenInterestInfo.dayTradePosition);
        LOG(DEBUG_LEVEL_DEBUG, "avgCost: %f", gOpenInterestInfo.avgCost);

        SHORT CloseBuySell = -1;

        if (gOpenInterestInfo.buySell == "S")
        {
            profitAndLoss = (gOpenInterestInfo.avgCost - curPrice) * DOLLARS_PER_TICK;
            CloseBuySell = ORDER_BUY_LONG_POSITION; // need to Buy to stop short position  loss
        }
        else if (gOpenInterestInfo.buySell == "B")

        {
            profitAndLoss = (curPrice - gOpenInterestInfo.avgCost) * DOLLARS_PER_TICK;
            CloseBuySell = ORDER_SELL_SHORT_POSITION; // need to Sell to stop long position loss
        }

        profitAndLoss = profitAndLoss * abs(static_cast<double>(gOpenInterestInfo.openPosition));

        gOpenInterestInfo.profitAndLoss = profitAndLoss; // assign new gOpenInterestInfo.profitAndLoss form local profitAndLoss

        LOG(DEBUG_LEVEL_DEBUG, "curPrice = %f, gOpenInterestInfo.avgCost= %f, profit and loss:%f",
            curPrice, gOpenInterestInfo.avgCost, gOpenInterestInfo.profitAndLoss);

        if (-profitAndLoss >= gStrategyConfig.MaximumLoss)
        {
            LOG(DEBUG_LEVEL_INFO, "STOP Loss at curPrice = %f, gOpenInterestInfo.avgCost= %f, profit and loss:%f",
                curPrice, gOpenInterestInfo.avgCost, profitAndLoss);

            vector<string> vec = {COMMODITY_MAIN, COMMODITY_OTHER};

            for (auto &x : vec)
            {
                AutoOrder(x,
                          ORDER_CLOSE_POSITION, // Close
                          CloseBuySell          // Buy or sell
                );
            }

#ifdef VIRTUAL_ACCOUNT_ORDER
            gOpenInterestInfo = {
                "",  // product
                "",  // Buy/Sell Indicator
                0,   // openPosition 0
                0,   // dayTradePosition 0
                0.0, // avgCost 0.0
                0.0, // profitAndLoss
                TRUE

            };

#endif
        }
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
}

VOID StrategyClosePosition(string strUserId, LONG MtxCommodtyInfo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    double curPrice = 0;

    if (gCurCommPrice.count(MtxCommodtyInfo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[MtxCommodtyInfo]) / 100.0;
    }

    if (gOpenInterestInfo.product != "" && gOpenInterestInfo.avgCost != 0 && curPrice > 0)
    {
        LOG(DEBUG_LEVEL_DEBUG, "product: %s", gOpenInterestInfo.product);
        LOG(DEBUG_LEVEL_DEBUG, "buySell: %s", gOpenInterestInfo.buySell);
        LOG(DEBUG_LEVEL_DEBUG, "openPosition: %ld", gOpenInterestInfo.openPosition);
        LOG(DEBUG_LEVEL_DEBUG, "dayTradePosition: %ld", gOpenInterestInfo.dayTradePosition);
        LOG(DEBUG_LEVEL_DEBUG, "avgCost: %f", gOpenInterestInfo.avgCost);

        LOG(DEBUG_LEVEL_DEBUG, "curPrice = %f, gOpenInterestInfo.avgCost= %f",
            curPrice, gOpenInterestInfo.avgCost);

        SHORT CloseBuySell = -1, BuySell = -1;

        if (gOpenInterestInfo.buySell == "S")
        {
            BuySell = 1;
            CloseBuySell = ORDER_BUY_LONG_POSITION; // short position
        }
        else if (gOpenInterestInfo.buySell == "B")
        {
            BuySell = 0;
            CloseBuySell = ORDER_SELL_SHORT_POSITION; // long position
        }

        if ((BuySell == 0 && curPrice >= EstimatedLongSideKeyPrice()) ||
            (BuySell == 1 && curPrice <= EstimatedShortSideKeyPrice()))
        {
            vector<string> vec = {COMMODITY_OTHER};

            for (auto &x : vec)
            {
                AutoOrder(x,
                          ORDER_CLOSE_POSITION, // Close
                          CloseBuySell          // Buy or sell
                );
            }

#ifdef VIRTUAL_ACCOUNT_ORDER
            gOpenInterestInfo = {
                "",  // product
                "",  // Buy/Sell Indicator
                0,   // openPosition 0
                0,   // dayTradePosition 0
                0.0, // avgCost 0.0
                0.0, // profitAndLoss
                TRUE

            };
#endif

            LOG(DEBUG_LEVEL_INFO, "Close position, curPrice = %f, gCostMovingAverageVal= %f, BidOfferLongShort: %ld",
                curPrice, gCostMovingAverageVal, StrategyCaluLongShort());
        }
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
}

LONG EstimatedLongSideKeyPrice(VOID)
{
    if (gDayAmpAndKeyPrice.LongKey1 == 0)
    {
        return LONG_MIN / 2;
    }

    switch (gStrategyConfig.ClosingKeyPriceLevel)
    {
    case 1:
    {
        return gDayAmpAndKeyPrice.LongKey1 + gStrategyConfig.ActivePoint;
    }
    case 2:
    {
        return gDayAmpAndKeyPrice.LongKey2 + gStrategyConfig.ActivePoint;
    }
    case 3:
    {
        return gDayAmpAndKeyPrice.LongKey3 + gStrategyConfig.ActivePoint;
    }
    case 4:
    {
        return gDayAmpAndKeyPrice.LongKey4 + gStrategyConfig.ActivePoint;
    }
    case 5:
    {
        return gDayAmpAndKeyPrice.LongKey5 + gStrategyConfig.ActivePoint;
    }

    default:
    {
        // Code for other cases
        return LONG_MIN / 2;
        break;
    }
    }
}

LONG EstimatedShortSideKeyPrice(VOID)
{
    if (gDayAmpAndKeyPrice.ShortKey1 == 0)
    {
        return LONG_MAX / 2;
    }

    switch (gStrategyConfig.ClosingKeyPriceLevel)
    {
    case 1:
    {
        return gDayAmpAndKeyPrice.ShortKey1 - gStrategyConfig.ActivePoint;
    }
    case 2:
    {
        return gDayAmpAndKeyPrice.ShortKey2 - gStrategyConfig.ActivePoint;
    }
    case 3:
    {
        return gDayAmpAndKeyPrice.ShortKey3 - gStrategyConfig.ActivePoint;
    }
    case 4:
    {
        return gDayAmpAndKeyPrice.ShortKey4 - gStrategyConfig.ActivePoint;
    }
    case 5:
    {
        return gDayAmpAndKeyPrice.ShortKey5 - gStrategyConfig.ActivePoint;
    }

    default:
    {
        // Code for other cases
        return LONG_MAX / 2;
        break;
    }
    }
}

LONG EstimatedTodaysAmplitude(VOID)
{
    if (gDayAmpAndKeyPrice.LongKey1 == 0)
    {
        return LONG_MIN / 2;
    }

    switch (gStrategyConfig.ClosingKeyPriceLevel)
    {
    case 1:
    {
        return gDayAmpAndKeyPrice.SmallestAmp + gStrategyConfig.ActivePoint;
    }
    case 2:
    {
        return gDayAmpAndKeyPrice.SmallAmp + gStrategyConfig.ActivePoint;
    }
    case 3:
    {
        return gDayAmpAndKeyPrice.AvgAmp + gStrategyConfig.ActivePoint;
    }
    case 4:
    {
        return gDayAmpAndKeyPrice.LargerAmp + gStrategyConfig.ActivePoint;
    }
    case 5:
    {
        return gDayAmpAndKeyPrice.LargestAmp + gStrategyConfig.ActivePoint;
    }

    default:
    {
        // Code for other cases
        return LONG_MIN / 2;
        break;
    }
    }
}

/**
 * @brief Implements a futures trading strategy based on the relationship between the cost line and the moving average line.
 *
 * Strategy Overview:
 *
 * 1. Cost Line Above the Moving Average Line:
 *    - Bearish Bias: Short the market when the price falls below the cost line, with the moving average line trending downward.
 *    - If the price rebounds and breaks through the moving average line, followed by a breakthrough of the cost line, switch to a bullish bias.
 *
 * 2. Cost Line Below the Moving Average Line:
 *    - Bullish Bias: Go long when the price breaks above the cost line, with the moving average line trending upward.
 *    - If the price declines, breaking below the moving average line, and continues to fall below the cost line, switch to a bearish bias.
 *
 * 3. Cost Line Equals the Moving Average Line:
 *    - Range-Bound Market: The market is considered range-bound, with the price oscillating between the cost line and the moving average line.
 *    - If the distance between the two lines is less than 20 points, refrain from trading.
 *
 * @param costLine The cost line value.
 * @param movingAverageLine The moving average line value.
 * @param currentPrice The current market price.
 * @return The trading action to be taken (e.g., buy, sell, hold).
 */

VOID StrategyNewLongShortPosition(string strUserId, LONG MtxCommodtyInfo, LONG LongShort)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    if (gOpenInterestInfo.NeedToUpdate == TRUE)
    {
        LOG(DEBUG_LEVEL_DEBUG, "gOpenInterestInfo.NeedToUpdate == TRUE");
        return;
    }

    double OpenPrice = 0;

    if (gCurCommHighLowPoint.count(MtxCommodtyInfo) == 0)
    {
        return;
    }
    else
    {
        OpenPrice = static_cast<double>(gCurCommHighLowPoint[MtxCommodtyInfo][2]) / 100.0;
    }

    double curPrice = 0;

    if (gCurCommPrice.count(MtxCommodtyInfo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[MtxCommodtyInfo]) / 100.0;
    }

    double CurAvg = 0;
    double CurAmp = 0;

    double CurHigh = 0, CurLow = 0;

    if (gCurCommHighLowPoint.count(MtxCommodtyInfo) > 0)
    {
        CurHigh = gCurCommHighLowPoint[MtxCommodtyInfo][0] / 100.0;
        CurLow = gCurCommHighLowPoint[MtxCommodtyInfo][1] / 100.0;
        CurAmp = CurHigh - CurLow;
        CurAvg = (CurHigh + CurLow) / 2;
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "curPrice = %f, CurAvg= %f, gCostMovingAverageVal=%f",
          curPrice, CurAvg, gCostMovingAverageVal);

    if (abs(CurAvg - gCostMovingAverageVal) <= SWING_POINTS ||
        CurAmp > EstimatedTodaysAmplitude())
    {
        return;
    }

    // Do Long

    if (LongShort == 1 && gOpenInterestInfo.openPosition <= 0)
    {
        DEBUG(DEBUG_LEVEL_DEBUG, "curPrice = %f, gOpenInterestInfo.avgCost= %f",
              curPrice, gOpenInterestInfo.avgCost);

        if (CurAvg > gCostMovingAverageVal &&
            curPrice >= gCostMovingAverageVal &&
            curPrice <= CurAvg &&
            curPrice <= EstimatedLongSideKeyPrice() &&
            curPrice > CurLow &&                                          // Don’t go short at new highs, don’t go long at new lows
            (EstimatedLongSideKeyPrice() - curPrice) >= ONE_STRIKE_PRICES // Earn at least one strike price

        )
        {

            LOG(DEBUG_LEVEL_INFO, "New Long position, curPrice = %f, gCostMovingAverageVal= %f, BidOfferLongShort: %ld",
                curPrice, gCostMovingAverageVal, gBidOfferLongShort);

            vector<string> vec = {COMMODITY_OTHER};

            for (auto &x : vec)
            {
                AutoOrder(x,
                          ORDER_NEW_POSITION,     // New
                          ORDER_BUY_LONG_POSITION // Buy or sell
                );
            }

            // Greedy assumptions always have positions

            {
                gOpenInterestInfo.product = COMMODITY_OTHER;
                gOpenInterestInfo.buySell = "B";
                gOpenInterestInfo.openPosition += 1;
                gOpenInterestInfo.avgCost = curPrice;
            }
        }
    }

    // Do Short

    if (LongShort == 0 && gOpenInterestInfo.openPosition >= 0)
    {
        DEBUG(DEBUG_LEVEL_DEBUG, "curPrice = %f, gOpenInterestInfo.avgCost= %f",
              curPrice, gOpenInterestInfo.avgCost);

        if (CurAvg < gCostMovingAverageVal &&
            curPrice <= gCostMovingAverageVal &&
            curPrice >= CurAvg &&
            curPrice >= EstimatedShortSideKeyPrice() &&
            curPrice < CurHigh &&                                         // Don’t go short at new highs, don’t go long at new lows
            (curPrice - EstimatedLongSideKeyPrice()) >= ONE_STRIKE_PRICES // Earn at least one strike price

        )
        {

            LOG(DEBUG_LEVEL_INFO, "New Short position, curPrice = %f, gCostMovingAverageVal= %f, BidOfferLongShort: %ld",
                curPrice, gCostMovingAverageVal, StrategyCaluLongShort());

            vector<string> vec = {COMMODITY_OTHER};

            for (auto &x : vec)
            {
                AutoOrder(x,
                          ORDER_NEW_POSITION,       // New
                          ORDER_SELL_SHORT_POSITION // Buy or sell
                );
            }

            // Greedy assumptions always have positions

            {
                gOpenInterestInfo.product = COMMODITY_OTHER;
                gOpenInterestInfo.buySell = "S";
                gOpenInterestInfo.openPosition -= 1;
                gOpenInterestInfo.avgCost = curPrice;
            }
        }
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
}

std::chrono::steady_clock::time_point gLastClearTime = std::chrono::steady_clock::now();

LONG CountBidOfferLongShort(LONG nStockidx)
{

    if (gBest5BidOffer[nStockidx].size() < 10)
    {
        return 0;
    }

    long countLong = 0, countShort = 0;

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - gLastClearTime);

    const int refreshInterval = 100; // 100 ms

    if (elapsed.count() >= refreshInterval)
    {
        gLastClearTime = now;

        long totalBid = 0;

        for (int i = 0; i < 5; ++i)
        {
            totalBid += gBest5BidOffer[nStockidx][i].second;
        }

        long totalOffer = 0;

        for (int i = 5; i < 10; ++i)
        {
            totalOffer += gBest5BidOffer[nStockidx][i].second;
        }

        if (totalBid * 3 <= totalOffer * 2)
        {
            ++countLong;
        }

        if (totalOffer * 3 <= totalBid * 2)
        {
            --countShort;
        }
    }

    LOG(DEBUG_LEVEL_DEBUG, "countLong = %ld, countShort=%ld", countLong, countShort);

    return gBidOfferLongShort += (countLong + countShort);
}

LONG CountTransactionListLongShort(LONG nStockidx)
{
    static unordered_map<long, long> PrePtr;

    long countLong = 0, countShort = 0;

    // extern std::unordered_map<long, std::array<long, 5>> gTransactionList;
    // long nPtr, long nBid, long nAsk, long nClose, long nQty,

    if (gTransactionList.count(nStockidx))
    {
        long nPtr = 0, nBid = 0, nAsk = 0, nClose = 0, nQty = 0;

        nPtr = gTransactionList[nStockidx][0];
        nBid = gTransactionList[nStockidx][1];
        nAsk = gTransactionList[nStockidx][2];
        nClose = gTransactionList[nStockidx][3];
        nQty = gTransactionList[nStockidx][4];

        if (!PrePtr.count(nStockidx) || PrePtr[nStockidx] != nPtr)
        {
            if (nClose > 0 && nClose <= nBid && nQty >= 10)
            {
                countShort -= nQty;
            }

            if (nClose > 0 && nClose >= nAsk && nQty >= 10)
            {
                countLong += nQty;
            }

            PrePtr[nStockidx] = nPtr;
        }
    }

    LOG(DEBUG_LEVEL_DEBUG, "countLong = %ld, countShort=%ld", countLong, countShort);

    return countLong + countShort;
}

LONG StrategyCaluBidOfferLongShort(VOID)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    if (gBidOfferLongShort >= INT_MAX || gBidOfferLongShort <= INT_MIN)
    {
        return gBidOfferLongShort;
    }

    // long MTXIdxNo;
    // long MTXIdxNoAM;
    // long TSMCIdxNo;
    // long HHIdxNo;
    // long TSEAIdxNo;

    if (gCommodtyInfo.TSMCIdxNo != 0)
    {
        long nStockidx = gCommodtyInfo.TSMCIdxNo;

        CountBidOfferLongShort(nStockidx);
    }

    if (gCommodtyInfo.HHIdxNo != 0)
    {
        long nStockidx = gCommodtyInfo.HHIdxNo;

        CountBidOfferLongShort(nStockidx);
    }

    LOG(DEBUG_LEVEL_DEBUG, "LongShort = %ld", gBidOfferLongShort);

    DEBUG(DEBUG_LEVEL_DEBUG, "End");

    if (gBidOfferLongShort > 0)
    {
        gBidOfferLongShort = min(gBidOfferLongShort, gStrategyConfig.BidOfferLongShortThreshold);
    }
    else if (gBidOfferLongShort < 0)
    {
        gBidOfferLongShort = max(gBidOfferLongShort, -gStrategyConfig.BidOfferLongShortThreshold);
    }

    return gBidOfferLongShort;
}

LONG StrategyCaluTransactionListLongShort(VOID)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    if (gTransactionListLongShort >= INT_MAX || gTransactionListLongShort <= INT_MIN)
    {
        return gTransactionListLongShort;
    }

    // long MTXIdxNo;
    // long MTXIdxNoAM;
    // long TSMCIdxNo;
    // long HHIdxNo;
    // long TSEAIdxNo;

    if (gCommodtyInfo.TSMCIdxNo != 0)
    {
        long nStockidx = gCommodtyInfo.TSMCIdxNo;

        gTransactionListLongShort += CountTransactionListLongShort(nStockidx);
    }

    if (gCommodtyInfo.HHIdxNo != 0)
    {
        long nStockidx = gCommodtyInfo.HHIdxNo;

        gTransactionListLongShort += CountTransactionListLongShort(nStockidx);
    }

    LOG(DEBUG_LEVEL_DEBUG, "LongShort = %ld", gTransactionListLongShort);

    DEBUG(DEBUG_LEVEL_DEBUG, "End");

    if (gTransactionListLongShort > 0)
    {
        gTransactionListLongShort = min(gTransactionListLongShort, gStrategyConfig.BidOfferLongShortThreshold);
    }
    else if (gTransactionListLongShort < 0)
    {
        gTransactionListLongShort = max(gTransactionListLongShort, -gStrategyConfig.BidOfferLongShortThreshold);
    }

    return gTransactionListLongShort;
}

LONG StrategyCaluLongShort(VOID)
{
    if (gCurServerTime[0] < 9 || (gCurServerTime[0] >= 13 && gCurServerTime[1] >= 30) || gCurServerTime[0] >= 14)
    {
        return 0;
    }

    return (gTransactionListLongShort + gBidOfferLongShort) / 2;
}

VOID StrategyNewIntervalAmpLongShortPosition(string strUserId, LONG MtxCommodtyInfo, LONG LongShort)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    if (gOpenInterestInfo.NeedToUpdate == TRUE)
    {
        LOG(DEBUG_LEVEL_DEBUG, "gOpenInterestInfo.NeedToUpdate == TRUE");
        return;
    }

    double CurAmp = 0;

    if (gCurCommHighLowPoint.count(MtxCommodtyInfo) == 0)
    {
        return;
    }
    else
    {
        CurAmp = static_cast<double>(gCurCommHighLowPoint[MtxCommodtyInfo][0]) / 100.0 -
                 static_cast<double>(gCurCommHighLowPoint[MtxCommodtyInfo][1]) / 100.0;
    }

    double curPrice = 0;

    if (gCurCommPrice.count(MtxCommodtyInfo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[MtxCommodtyInfo]) / 100.0;
    }

    if (CurAmp > EstimatedTodaysAmplitude())
    {
        return;
    }

    if (curPrice > 0)
    {
        DEBUG(DEBUG_LEVEL_DEBUG, "curPrice = %f, gOpenInterestInfo.avgCost= %f",
              curPrice, gOpenInterestInfo.avgCost);

        SHORT BuySell = -1;

        if (LongShort == 1 && gOpenInterestInfo.openPosition <= 0)
        {
            if (gDayAmpAndKeyPrice.ShortKey2 > 0 && curPrice <= gDayAmpAndKeyPrice.ShortKey2 && curPrice >= gDayAmpAndKeyPrice.ShortKey1 - gStrategyConfig.ActivePoint)
            {
                BuySell = ORDER_BUY_LONG_POSITION; // Long position
            }
        }
        else if (LongShort == 0 && gOpenInterestInfo.openPosition >= 0)
        {
            if (gDayAmpAndKeyPrice.LongKey2 > 0 && curPrice >= gDayAmpAndKeyPrice.LongKey2 && curPrice <= gDayAmpAndKeyPrice.LongKey2 + gStrategyConfig.ActivePoint)
            {
                BuySell = ORDER_SELL_SHORT_POSITION; // Short position
            }
        }

        if (BuySell != -1)
        {
            vector<string> vec = {COMMODITY_OTHER};

            for (auto &x : vec)
            {
                AutoOrder(x,
                          ORDER_NEW_POSITION, // New
                          BuySell             // Buy or sell
                );
            }

            {
                gOpenInterestInfo.product = COMMODITY_OTHER;

                if (BuySell == 1)
                {
                    gOpenInterestInfo.buySell = "S";
                    gOpenInterestInfo.openPosition -= 1;

                    LOG(DEBUG_LEVEL_INFO, "New Short position, curPrice = %f",
                        curPrice);
                }

                if (BuySell == 0)
                {
                    gOpenInterestInfo.buySell = "B";
                    gOpenInterestInfo.openPosition += 1;

                    LOG(DEBUG_LEVEL_INFO, "New Long position, curPrice = %f",
                        curPrice);
                }

                gOpenInterestInfo.avgCost = curPrice;
            }
        }
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
}
