#include "Strategy.h"
#include "StrategyCfd.h"
#include "SKCenterLib.h"
#include "SKOSQuoteLib.h"
#include "SKOrderLib.h"
#include "SKQuoteLib.h"
#include "SKReplyLib.h"
#include <Logger.h>
#include <algorithm>
#include <array>
#include <chrono> // For std::chrono::steady_clock
#include <cmath>
#include <conio.h> // For kbhit() and _getch()
#include <cstdlib> // For system("cls")
#include <deque>
#include <iostream>
#include <map>
#include <numeric> // This header is needed for std::accumulate
#include <queue>
#include <set>
#include <thread> // For std::this_thread::sleep_for
#include <unordered_map>
#include <vector>
#include <yaml-cpp/yaml.h>
#include "config.h"

using namespace std;

// Object pointer

extern CSKCenterLib *pSKCenterLib;
extern CSKQuoteLib *pSKQuoteLib;
extern CSKReplyLib *pSKReplyLib;
extern CSKOrderLib *pSKOrderLib;
extern CSKOSQuoteLib *pSKOsQuoteLib;

extern void AutoLogIn();
void AutoOsQuoteTicks(IN string ProductNum, short sPageNo);
extern void release();

// Global variables, initialized by main, and continuously updated by the com server
extern SHORT gCurServerTime[3];

extern COMMODITY_OS_INFO gCommodtyOsInfo;

extern double calculate5MA(std::deque<double> &closePrices);
extern LONG CountOsTransactionListLongShort(LONG nStockidx);

// Global CFD variable

std::unordered_map<long, long> gCfdTransactionListLongShort;
std::unordered_map<long, double> gCfdTransactionListLongShortSlope;

/**
 * @brief Calculate the slope of the long-short position using bid-offer and transaction data.
 *
 * This function calculates the long-short position slope based on the difference in
 * long-short positions from bid-offer and transaction data. It uses a moving average (MA)
 * to smooth the position values and incorporates a PID control-inspired derivative term
 * to enhance responsiveness to rapid changes in the position data. The smoothed difference
 * (delta) is calculated and applied to adjust the overall long-short trend.
 *
 * @note This function updates the global variable `gBidOfferLongShortSlope` to reflect
 *       the slope of long-short positions.
 *
 * The global long-short position (`gLongShort`) is bounded between predefined thresholds
 * to avoid extreme values, and the slope is computed using a deque to store a set of recent
 * data points, which helps in calculating a moving average slope.
 *
 * The function also smooths the derivative term using a weighted average, which mitigates
 * the impact of noise or short-term fluctuations on the slope calculation.
 *
 * @return VOID
 */
VOID CfdBidOfferAndTransactionListLongShortSlope(long nStockidx)
{
    // Deques to store recent values for calculating moving average (MA) and slope
    static std::deque<double> dq, dqSlop;

    // Get the current long-short position by invoking a custom function
    LONG CurLongShort = StrategyCaluLongShort();

    // Store the previous long-short value for calculating the difference
    static LONG PreLongShort = CurLongShort;

    // Calculate the difference between current and previous long-short values
    LONG LongShortDiff = CurLongShort - PreLongShort;
    PreLongShort = CurLongShort;

    if (LongShortDiff == 0)
    {
        return;
    }

    // Update the global long-short position with the smoothed difference
    gLongShort += LongShortDiff;

    // Bound the global long-short position between predefined thresholds to avoid extreme values
    if (gLongShort > 0)
    {
        gLongShort = min(gLongShort, INT_MAX);
    }
    else
    {
        gLongShort = max(gLongShort, INT_MIN);
    }

    // Manage the size of the deque to store the recent long-short values for moving average calculation
    if (dq.size() >= BID_OFFER_SLOPE_LONG_SHORT_COUNT)
    {
        dq.pop_front(); // Remove the oldest value
    }
    dq.push_back(gLongShort); // Add the current long-short value

    // Ensure the deque has enough values to calculate the moving average and slope
    if (dq.size() >= BID_OFFER_SLOPE_LONG_SHORT_COUNT)
    {
        // Calculate the moving average of the deque
        double ma = calculate5MA(dq);

        // Maintain a deque for the moving average values to compute the slope
        if (dqSlop.size() >= BID_OFFER_SLOPE_LONG_SHORT_COUNT)
        {
            dqSlop.pop_front(); // Remove the oldest MA value
        }
        dqSlop.push_back(ma); // Add the new MA value

        // Calculate the slope based on the difference between the first and last values in the deque
        double deltaY = dqSlop.back() - dqSlop.front();
        double MaSlope = deltaY / BID_OFFER_SLOPE_LONG_SHORT_COUNT;

        // Update the global variable that tracks the bid-offer long-short slope
        gBidOfferLongShortSlope = MaSlope;
    }

    return;
}

VOID CfdStrategySwitch()
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    if (gCommodtyOsInfo.GCIdxNo != 0)
    {
        long nStockidx = gCommodtyOsInfo.GCIdxNo;

        gCfdTransactionListLongShort[nStockidx] += CountOsTransactionListLongShort(nStockidx);

        DEBUG(DEBUG_LEVEL_DEBUG, "nStockidx: %ld, gCfdTransactionListLongShort[nStockidx]: %ld\n",
              nStockidx, gCfdTransactionListLongShort[nStockidx]);
    }

    if (gCommodtyOsInfo.NQIdxNo != 0)
    {
        long nStockidx = gCommodtyOsInfo.GCIdxNo;

        gCfdTransactionListLongShort[nStockidx] += CountOsTransactionListLongShort(nStockidx);

        DEBUG(DEBUG_LEVEL_DEBUG, "nStockidx: %ld, gCfdTransactionListLongShort[nStockidx]: %ld\n",
              nStockidx, gCfdTransactionListLongShort[nStockidx]);
    }

    if (gCommodtyOsInfo.DXIdxNo != 0)
    {
        long nStockidx = gCommodtyOsInfo.DXIdxNo;

        gCfdTransactionListLongShort[nStockidx] += CountOsTransactionListLongShort(nStockidx);

        DEBUG(DEBUG_LEVEL_DEBUG, "nStockidx: %ld, gCfdTransactionListLongShort[nStockidx]: %ld\n",
              nStockidx, gCfdTransactionListLongShort[nStockidx]);
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}