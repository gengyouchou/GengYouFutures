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
extern std::unordered_map<long, std::array<long, 2>> gCurCommHighLowPoint;
extern std::unordered_map<long, long> gCurCommPrice;
extern std::unordered_map<long, vector<pair<long, long>>> gBest5BidOffer;

extern OpenInterestInfo gOpenInterestInfo;

extern string g_strUserId;

extern COMMODITY_INFO gCommodtyInfo;

DAY_AMP_AND_KEY_PRICE gDayAmpAndKeyPrice = {0};

/**
 * @brief
 *
 *  struct FUTUREORDER
 * {
 *  BSTR bstrFullAccount; // 7
 *  BSTR bstrStockNo;     //
 *  SHORT sTradeType;     // 0:ROD  1:IOC  2:FOK
 *  SHORT sBuySell;       // 0: 1:
 *  SHORT sDayTrade;      // 0: 1:
 *  SHORT sNewClose;      // 0: 1: 2:{}
 *  BSTR bstrPrice;       // (IOC and FOKMP)
 *  LONG nQty;            //
 *  SHORT sReserved;      //{SendFutureOrderCLR}0:(TT+1)1:T
 * };
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

void AutoOrder(IN string ProductNum, IN SHORT NewClose, IN SHORT BuySell)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

    long g_nCode = pSKOrderLib->SendFutureOrder(g_strUserId,
                                                false, // bAsyncOrder
                                                ProductNum,
                                                1,        // IOC
                                                BuySell,  // BuySell
                                                0,        // DayTrade
                                                NewClose, // NewClose // 0: 1: 2:{}
                                                "P",
                                                1,
                                                0);

    pSKCenterLib->PrintfCodeMessage("AutoOrder", "SendFutureOrder", g_nCode);

    LOG(DEBUG_LEVEL_INFO, "SendFutureOrder res = %d, ProductNum=%s, NewClose=%d, BuySell=%d",
        g_nCode, ProductNum, NewClose, BuySell);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

VOID StrategyStopFuturesLoss(CSKOrderLib *SKOrderLib, string strUserId)
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

        if (gOpenInterestInfo.buySell == "S")
        {
            profitAndLoss = gOpenInterestInfo.avgCost - curPrice;
        }
        else
        {
            profitAndLoss = curPrice - gOpenInterestInfo.avgCost;
        }

        LOG(DEBUG_LEVEL_INFO, "curPrice = %f, gOpenInterestInfo.avgCost= %f, profit and loss:%f",
            curPrice, gOpenInterestInfo.avgCost, profitAndLoss);

        if (profitAndLoss >= MAXIMUM_LOSS)
        {
            vector<string> vec = {COMMODITY_MAIN, COMMODITY_OTHER};

            for (auto &x : vec)
            {
                if (gOpenInterestInfo.buySell == "S")
                {
                    AutoOrder(x,
                              1, // Close
                              0  // Buy
                    );
                }
                else
                {
                    AutoOrder(x,
                              1, // Close
                              1  // Sell
                    );
                }
            }

            SKOrderLib->GetOpenInterest(strUserId, 1);
        }
    }
    else
    {
        SKOrderLib->GetOpenInterest(strUserId, 1);
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
}
