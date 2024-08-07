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
#include "Strategy.h"

// Define the global logger instance
Logger StrategyLog("Strategy.log");

// Global variables, initialized by main, and continuously updated by the com server
extern SHORT gCurServerTime[3];
extern std::unordered_map<SHORT, std::array<long, 4>> gCurTaiexInfo;
extern std::deque<long> gDaysKlineDiff;
extern std::unordered_map<long, std::array<long, 2>> gCurCommHighLowPoint;
extern std::unordered_map<long, long> gCurCommPrice;
extern std::unordered_map<long, vector<pair<long, long>>> gBest5BidOffer;

extern OpenInterestInfo gOpenInterestInfo;

VOID StrategyStopFuturesLoss(CSKOrderLib *SKOrderLib, string strUserId)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    SKOrderLib->GetOpenInterest(strUserId, 1);

    // AutoOrderMTX(0); // new
    // AutoOrderMTX(1); // close

    // if over loss then do GetOpenInterest again in order to make sure
    // AutoOrderMTX(1) won’t place a closing order with the wrong amount
    // else continue to calculate profit and loss and update to global variables

    if (gOpenInterestInfo.product != "")
    {
        LOG(DEBUG_LEVEL_INFO, "product: %s", gOpenInterestInfo.product);
        LOG(DEBUG_LEVEL_INFO, "buySell: %s", gOpenInterestInfo.buySell);
        LOG(DEBUG_LEVEL_INFO, "openPosition: %ld", gOpenInterestInfo.openPosition);
        LOG(DEBUG_LEVEL_INFO, "dayTradePosition: %ld", gOpenInterestInfo.dayTradePosition);
        LOG(DEBUG_LEVEL_INFO, "avgCost: %f", gOpenInterestInfo.avgCost);

        SKCOMLib::SKSTOCKLONG skStock;

        long res = pSKQuoteLib->RequestStockIndexMap(gOpenInterestInfo.product, &skStock);

        DEBUG(DEBUG_LEVEL_INFO, "pSKQuoteLib->RequestStockIndexMap()=%d", res);

        long IdxNo = skStock.nStockIdx;

        double profitAndLoss = gOpenInterestInfo.avgCost - gCurCommPrice[IdxNo];

        LOG(DEBUG_LEVEL_INFO, "profit and loss:%f", profitAndLoss);
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
}
