#include "Strategy.h"
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

using namespace std;

std::chrono::steady_clock::time_point gLastClearTime = std::chrono::steady_clock::now();

// Define the global logger instance
Logger StrategyLog("Strategy");

// Object pointer

extern CSKCenterLib *pSKCenterLib;
extern CSKQuoteLib *pSKQuoteLib;
extern CSKReplyLib *pSKReplyLib;
extern CSKOrderLib *pSKOrderLib;

// Global variables, initialized by main, and continuously updated by the com server
extern SHORT gCurServerTime[3];
extern std::unordered_map<SHORT, std::array<long, 6>> gCurTaiexInfo;
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

LONG gBidOfferLongShort = 0, gTransactionListLongShort = 0, gOsTransactionListLongShort = 0;
LONG gLongShort = 0;
double gCostMovingAverageVal = 0;
double gMa5 = 0;
double gMa5LongShort = 0;
double gNQMa20 = 0;
double gNQMa20LongShort = 0;

double gBidOfferLongShortSlope = 0;
double gNumberOfStocksRisingAndFalling = 0;

static double gEntryHigh = 0;
static double gEntryLow = 0;

LONG gEvaluatePosition = MAXIMUM_NUMBERS_OF_POSITIONS;

STRATEGY_CONFIG gStrategyConfig = {
    CLOSING_KEY_PRICE_LEVEL,
    BID_OFFER_LONG_SHORT_THRESHOLD,
    BID_OFFER_LONG_SHORT_ATTACK_SLOPE,
    ACTIVITY_POINT,
    MAXIMUM_LOSS,
    STRATEGY_MODE

};

LONG EstimatedLongSideKeyPrice(VOID);
LONG EstimatedShortSideKeyPrice(VOID);
LONG CountBidOfferLongShort(LONG nStockidx);
LONG CountTransactionListLongShort(LONG nStockidx);

// Function to calculate the 5-minute moving average (5MA)
static double calculate5MA(std::deque<double> &closePrices)
{
    if (closePrices.size() <= 0)
    {
        return NAN; // Not enough data to calculate 5MA yet
    }

    double sum = std::accumulate(closePrices.begin(), closePrices.end(), 0.0);
    return sum / closePrices.size();
}

/**
 * @brief Handle new tick data and calculate 5MA slope for opening long/short positions based on 1-minute close prices.
 *
 * This function processes tick data for a specific stock index (nStockidx), computes the 5-minute moving average (5MA)
 * using the closing prices of the past 5 minutes, and determines whether to open long or short positions based on the
 * slope of the 5MA (i.e., the difference between the current and previous 5MA values).
 *
 * The strategy is as follows:
 * - Open a long position when the 5MA slope is positive.
 * - Open a short position when the 5MA slope is negative.
 *
 * The function tracks the last processed minute and updates the closing prices deque when a new minute begins.
 * Only the last tick of the minute is used to represent the closing price for that minute.
 *
 * @param nStockidx The stock index for which tick data is being processed.
 * @return int
 *         - 1 if a long position should be opened.
 *         - 0 if a short position should be opened.
 *         - -1 if no action is taken (e.g., not enough data, or no change in position).
 */
int EarnAtLeastOneStrike(LONG MtxCommodtyInfo, LONG LongShort)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    if (gOpenInterestInfo.NeedToUpdate == TRUE)
    {
        LOG(DEBUG_LEVEL_DEBUG, "gOpenInterestInfo.NeedToUpdate == TRUE");
        return -1;
    }

    double OpenPrice = 0;

    // Check if the commodity information is present
    if (gCurCommHighLowPoint.count(MtxCommodtyInfo) == 0)
    {
        return -1;
    }
    else
    {
        OpenPrice = static_cast<double>(gCurCommHighLowPoint[MtxCommodtyInfo][2]) / 100.0;
    }

    double curPrice = 0;

    // Get the current price for the commodity
    if (gCurCommPrice.count(MtxCommodtyInfo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[MtxCommodtyInfo]) / 100.0;
    }

    double CurAvg = 0;
    double CurAmp = 0;
    double CurHigh = 0, CurLow = 0;

    // Calculate the current high, low, amplitude, and average
    if (gCurCommHighLowPoint.count(MtxCommodtyInfo) > 0)
    {
        CurHigh = gCurCommHighLowPoint[MtxCommodtyInfo][0] / 100.0;
        CurLow = gCurCommHighLowPoint[MtxCommodtyInfo][1] / 100.0;
        CurAmp = CurHigh - CurLow;
        CurAvg = (CurHigh + CurLow) / 2;
    }

    if (curPrice <= 0 || CurHigh <= 0 || CurLow <= 0)
    {
        return -1;
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "curPrice = %f, CurAvg= %f, gCostMovingAverageVal=%f",
          curPrice, CurAvg, gCostMovingAverageVal);

    if (LongShort == 1)
    {
        // Strategy for going long

        if (CurHigh - curPrice >= ONE_STRIKE_PRICES)
        {
            return 1;
        }
    }

    if (LongShort == 0)
    {
        // Strategy for going Short

        if (curPrice - CurLow >= ONE_STRIKE_PRICES)
        {
            return 0;
        }
    }

    return -1;
}

/**
 * @brief Handle new tick data and calculate 5MA slope for opening long/short positions based on 1-minute close prices.
 *
 * This function processes tick data for a specific stock index (nStockidx), computes the 5-minute moving average (5MA)
 * using the closing prices of the past 5 minutes, and determines whether to open long or short positions based on the
 * slope of the 5MA (i.e., the difference between the current and previous 5MA values).
 *
 * The strategy is as follows:
 * - Open a long position when the 5MA slope is positive.
 * - Open a short position when the 5MA slope is negative.
 *
 * The function tracks the last processed minute and updates the closing prices deque when a new minute begins.
 * Only the last tick of the minute is used to represent the closing price for that minute.
 *
 * @param nStockidx The stock index for which tick data is being processed.
 * @return int
 *         - 1  long position should be opened.
 *         - 0  Both long short position should be opened.
 *         - -1 short position should be opened.
 *         - other no action
 */
int LongShortExtremeFitRate(LONG MtxCommodtyInfo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    // Check if the commodity information is present
    if (gCurCommHighLowPoint.count(MtxCommodtyInfo) == 0)
    {
        return 2;
    }

    double curPrice = 0;

    // Get the current price for the commodity
    if (gCurCommPrice.count(MtxCommodtyInfo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[MtxCommodtyInfo]) / 100.0;
    }

    if (curPrice <= 0 || gCostMovingAverageVal <= 0)
    {
        return 2;
    }

    double LongExtremeValue = gCostMovingAverageVal - EstimatedTodaysAmplitude() / 2;
    double ShortExtremeValue = gCostMovingAverageVal + EstimatedTodaysAmplitude() / 2;

    // Strategy for going long

    if (curPrice <= LongExtremeValue)
    {
        return 1;
    }

    // Strategy for going Short

    if (curPrice >= ShortExtremeValue)
    {
        return -1;
    }

    return 0;
}

/**
 * @brief Handle new tick data and calculate 5MA slope for opening long/short positions based on 1-minute close prices.
 *
 * This function processes tick data for a specific stock index (nStockidx), computes the 5-minute moving average (5MA)
 * using the closing prices of the past 5 minutes, and determines whether to open long or short positions based on the
 * slope of the 5MA (i.e., the difference between the current and previous 5MA values).
 *
 * The strategy is as follows:
 * - Open a long position when the 5MA slope is positive.
 * - Open a short position when the 5MA slope is negative.
 *
 * The function tracks the last processed minute and updates the closing prices deque when a new minute begins.
 * Only the last tick of the minute is used to represent the closing price for that minute.
 *
 * @param nStockidx The stock index for which tick data is being processed.
 * @return int
 *         - 1 if a long position should be opened.
 *         - 0 if a short position should be opened.
 *         - -1 if no action is taken (e.g., not enough data, or no change in position).
 */
BOOLEAN TodayAmplitudeHasBeenReached(LONG MtxCommodtyInfo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    if (gOpenInterestInfo.NeedToUpdate == TRUE)
    {
        LOG(DEBUG_LEVEL_DEBUG, "gOpenInterestInfo.NeedToUpdate == TRUE");
        return TRUE;
    }

    double OpenPrice = 0;

    // Check if the commodity information is present
    if (gCurCommHighLowPoint.count(MtxCommodtyInfo) == 0)
    {
        return TRUE;
    }

    double curPrice = 0;

    // Get the current price for the commodity
    if (gCurCommPrice.count(MtxCommodtyInfo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[MtxCommodtyInfo]) / 100.0;
    }

    double CurAvg = 0;
    double CurAmp = 0;
    double CurHigh = 0, CurLow = 0;

    // Calculate the current high, low, amplitude, and average
    if (gCurCommHighLowPoint.count(MtxCommodtyInfo) > 0)
    {
        CurHigh = gCurCommHighLowPoint[MtxCommodtyInfo][0] / 100.0;
        CurLow = gCurCommHighLowPoint[MtxCommodtyInfo][1] / 100.0;
        CurAmp = CurHigh - CurLow;
        CurAvg = (CurHigh + CurLow) / 2;
    }

    if (curPrice <= 0 || CurHigh <= 0 || CurLow <= 0)
    {
        return TRUE;
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "curPrice = %f, CurAvg= %f, gCostMovingAverageVal=%f",
          curPrice, CurAvg, gCostMovingAverageVal);

    // Check if the current amplitude is within the estimated amplitude
    if (CurAmp >= EstimatedTodaysAmplitude())
    {
        return TRUE;
    }

    return FALSE;
}
/**
 * @brief Handle new tick data and calculate 5MA slope for opening long/short positions based on 1-minute close prices.
 *
 * This function processes tick data for a specific stock index (nStockidx), computes the 5-minute moving average (5MA)
 * using the closing prices of the past 5 minutes, and determines whether to open long or short positions based on the
 * slope of the 5MA (i.e., the difference between the current and previous 5MA values).
 *
 * The strategy is as follows:
 * - Open a long position when the 5MA slope is positive.
 * - Open a short position when the 5MA slope is negative.
 *
 * The function tracks the last processed minute and updates the closing prices deque when a new minute begins.
 * Only the last tick of the minute is used to represent the closing price for that minute.
 *
 * @param nStockidx The stock index for which tick data is being processed.
 * @return int
 *         - 1 if a long position should be opened.
 *         - 0 if a short position should be opened.
 *         - -1 if no action is taken (e.g., not enough data, or no change in position).
 */
int Count5MaForNewLongShortPosition(LONG nStockidx)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    // Static variables to retain state between function calls
    static unordered_map<long, long> PrePtr; // Keeps track of the last processed pointer for each stock index
    static std::deque<double> closePrices;   // Stores the last 5 minutes of closing prices
    static int lastMinute = -1;              // Tracks the last processed minute
    static double lastMinutePrice = 0;       // The most recent price in the last minute
    static double lastMa5 = 0;               // Stores the previous 5MA value for slope calculation

    // Ensure that there is data for the given stock index
    if (gTransactionList.count(nStockidx))
    {
        // Retrieve tick data from gTransactionList for the given stock index
        long nPtr = 0, nBid = 0, nAsk = 0, nClose = 0, nQty = 0, nTimehms = 0;

        nPtr = gTransactionList[nStockidx][0];
        nBid = gTransactionList[nStockidx][1];
        nAsk = gTransactionList[nStockidx][2];
        nClose = gTransactionList[nStockidx][3];
        nQty = gTransactionList[nStockidx][4];
        nTimehms = gTransactionList[nStockidx][5];

        DEBUG(DEBUG_LEVEL_DEBUG, "nStockIndex: %ld, nPtr: %ld,nTimehms: %ld,nBid: %ld,nAsk: %ld,nClose: %ld,nQty: %ld\n",
              nStockidx, nPtr, nTimehms, nBid, nAsk, nClose, nQty);

        // Process new tick data only if this is a new pointer (i.e., a new tick)
        if (!PrePtr.count(nStockidx) || PrePtr[nStockidx] != nPtr)
        {
            // Extract the current minute from nTimehms (formatted as hhmmss)
            int currentMinute = nTimehms / 100; // Extract hh:mm (ignores seconds)

            // Convert closing price to double
            double tickPrice = static_cast<double>(nClose) / 100.0;

            // Check if we've entered a new minute
            if (currentMinute != lastMinute)
            {
                // If we have already processed at least one minute
                if (lastMinute != -1)
                {
                    // Push the last minute's closing price to the deque
                    if (closePrices.size() >= MA5)
                    {
                        closePrices.pop_front(); // Remove the oldest closing price
                    }
                    closePrices.push_back(lastMinutePrice); // Push the final price of the last minute
                }

                if (closePrices.size() > 0)
                {
                    // Calculate the 5MA
                    double ma5 = calculate5MA(closePrices);
                    gMa5 = ma5;

                    // Determine the slope of the 5MA
                    double ma5Slope = ma5 - lastMa5;

                    gMa5LongShort = ma5Slope;
                    // Store the current 5MA for the next comparison
                    lastMa5 = ma5;
                }
                else
                {
                    // Store the current 5MA for the next comparison
                    lastMa5 = tickPrice;
                }

                // Update the last processed minute and reset the lastMinutePrice for the new minute
                lastMinute = currentMinute;
            }

            // Update the most recent price in the current minute (i.e., every tick updates the "closing" price)
            lastMinutePrice = tickPrice;

            // Check if we have enough data to calculate the 5MA (at least 5 minutes of closing prices)
            if (closePrices.size() > 0)
            {
                deque<double> TempClosePrices = closePrices;
                TempClosePrices.pop_front();
                TempClosePrices.push_back(tickPrice);

                double CurMa5 = calculate5MA(TempClosePrices);
                gMa5 = CurMa5;

                // Determine the slope of the 5MA
                double ma5Slope = CurMa5 - lastMa5;

                gMa5LongShort = ma5Slope;

                DEBUG(DEBUG_LEVEL_DEBUG, "5MA: %f, lastMa5: %f, Slope: %f", CurMa5, lastMa5, gMa5LongShort);

                // Strategy: Open positions based on the slope of the 5MA
                if (gMa5LongShort > 0)
                {
                    // Go long: 5MA slope is positive
                    DEBUG(DEBUG_LEVEL_DEBUG, "Opening long position. 5MA: %f, Slope: %ld", CurMa5, gMa5LongShort);
                    PrePtr[nStockidx] = nPtr; // Update the last processed pointer
                    return 1;                 // Return 1 for long position
                }
                else if (gMa5LongShort < 0)
                {
                    // Go short: 5MA slope is negative
                    DEBUG(DEBUG_LEVEL_DEBUG, "Opening short position. 5MA: %f, Slope: %ld", CurMa5, gMa5LongShort);
                    PrePtr[nStockidx] = nPtr; // Update the last processed pointer
                    return 0;                 // Return 0 for short position
                }
            }
            else
            {
                lastMa5 = tickPrice;
            }

            // Update the last processed pointer to prevent processing the same tick again
            PrePtr[nStockidx] = nPtr;
        }
    }

    return -1; // No action, return -1
}

/**
 * @brief Handle new tick data and calculate 5MA slope for opening long/short positions based on 1-minute close prices.
 *
 * This function processes tick data for a specific stock index (nStockidx), computes the 5-minute moving average (5MA)
 * using the closing prices of the past 5 minutes, and determines whether to open long or short positions based on the
 * slope of the 5MA (i.e., the difference between the current and previous 5MA values).
 *
 * The strategy is as follows:
 * - Open a long position when the 5MA slope is positive.
 * - Open a short position when the 5MA slope is negative.
 *
 * The function tracks the last processed minute and updates the closing prices deque when a new minute begins.
 * Only the last tick of the minute is used to represent the closing price for that minute.
 *
 * @param nStockidx The stock index for which tick data is being processed.
 * @return int
 *         - 1 if a long position should be opened.
 *         - 0 if a short position should be opened.
 *         - -1 if no action is taken (e.g., not enough data, or no change in position).
 */
int CountOsNQ20MaForNewLongShortPosition(LONG nStockidx)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    // Static variables to retain state between function calls
    static unordered_map<long, long> PrePtr; // Keeps track of the last processed pointer for each stock index
    static std::deque<double> closePrices;   // Stores the last 5 minutes of closing prices
    static int lastMinute = -1;              // Tracks the last processed minute
    static double lastMinutePrice = 0;       // The most recent price in the last minute
    static double lastMa5 = 0;               // Stores the previous 5MA value for slope calculation

    // Ensure that there is data for the given stock index
    if (gOsTransactionList.count(nStockidx))
    {
        // Retrieve tick data from gOsTransactionList for the given stock index
        long nPtr = 0, nClose = 0, nQty = 0, nTimehms = 0;

        nPtr = gOsTransactionList[nStockidx][0];
        nClose = gOsTransactionList[nStockidx][1];
        nQty = gOsTransactionList[nStockidx][2];
        nTimehms = gOsTransactionList[nStockidx][3];

        DEBUG(DEBUG_LEVEL_DEBUG, "nStockIndex: %ld, nPtr: %ld,nTimehms: %ld,nClose: %ld,nQty: %ld\n",
              nStockidx, nPtr, nTimehms, nClose, nQty);

        // Process new tick data only if this is a new pointer (i.e., a new tick)
        if (!PrePtr.count(nStockidx) || PrePtr[nStockidx] != nPtr)
        {
            // Extract the current minute from nTimehms (formatted as hhmmss)
            int currentMinute = nTimehms / 100; // Extract hh:mm (ignores seconds)

            // Convert closing price to double
            double tickPrice = static_cast<double>(nClose) / 100.0;

            // Check if we've entered a new minute
            if (currentMinute != lastMinute)
            {
                // If we have already processed at least one minute
                if (lastMinute != -1)
                {
                    // Push the last minute's closing price to the deque
                    if (closePrices.size() >= MA20)
                    {
                        closePrices.pop_front(); // Remove the oldest closing price
                    }
                    closePrices.push_back(lastMinutePrice); // Push the final price of the last minute
                }

                if (closePrices.size() >= MA20)
                {
                    // Calculate the 5MA
                    double ma5 = calculate5MA(closePrices);
                    gNQMa20 = ma5;

                    // Determine the slope of the 5MA
                    double ma5Slope = ma5 - lastMa5;

                    gNQMa20LongShort = ma5Slope;
                    // Store the current 5MA for the next comparison
                    lastMa5 = ma5;
                }
                else
                {
                    // Store the current 5MA for the next comparison
                    lastMa5 = calculate5MA(closePrices);
                }

                // Update the last processed minute and reset the lastMinutePrice for the new minute
                lastMinute = currentMinute;
            }

            // Update the most recent price in the current minute (i.e., every tick updates the "closing" price)
            lastMinutePrice = tickPrice;

            // Check if we have enough data to calculate the 5MA (at least 5 minutes of closing prices)
            if (closePrices.size() >= MA20)
            {
                deque<double> TempClosePrices = closePrices;
                TempClosePrices.pop_front();
                TempClosePrices.push_back(tickPrice);

                double CurMa5 = calculate5MA(TempClosePrices);
                gNQMa20 = CurMa5;

                // Determine the slope of the 5MA
                double ma5Slope = CurMa5 - lastMa5;

                gNQMa20LongShort = ma5Slope;

                DEBUG(DEBUG_LEVEL_DEBUG, "5MA: %f, lastMa5: %f, Slope: %f", CurMa5, lastMa5, gNQMa20LongShort);

                // Strategy: Open positions based on the slope of the 5MA
                if (gNQMa20LongShort > 0)
                {
                    // Go long: 5MA slope is positive
                    DEBUG(DEBUG_LEVEL_DEBUG, "Opening long position. 5MA: %f, Slope: %ld", CurMa5, gNQMa20LongShort);
                    PrePtr[nStockidx] = nPtr; // Update the last processed pointer
                    return 1;                 // Return 1 for long position
                }
                else if (gNQMa20LongShort < 0)
                {
                    // Go short: 5MA slope is negative
                    DEBUG(DEBUG_LEVEL_DEBUG, "Opening short position. 5MA: %f, Slope: %ld", CurMa5, gNQMa20LongShort);
                    PrePtr[nStockidx] = nPtr; // Update the last processed pointer
                    return 0;                 // Return 0 for short position
                }
            }
            else
            {
                lastMa5 = calculate5MA(closePrices);
            }

            // Update the last processed pointer to prevent processing the same tick again
            PrePtr[nStockidx] = nPtr;
        }
    }

    return -1; // No action, return -1
}
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
VOID BidOfferAndTransactionListLongShortSlope(VOID)
{
    // Deques to store recent values for calculating moving average (MA) and slope
    static std::deque<double> dq, dqSlop;

    // Store the previous moving average difference (used in smoothing)
    static double PreMaDiff = 0;

    LONG CurNumberOfStocksRisingAndFalling = gNumberOfStocksRisingAndFalling;
    static LONG PreNumberOfStocksRisingAndFalling = gNumberOfStocksRisingAndFalling;

    // Get the current long-short position by invoking a custom function
    LONG CurLongShort = StrategyCaluLongShort() +
                        static_cast<long>(NUMBER_OF_STOCKS_RISING_AND_FALLING * (CurNumberOfStocksRisingAndFalling - PreNumberOfStocksRisingAndFalling));

    PreNumberOfStocksRisingAndFalling = CurNumberOfStocksRisingAndFalling;

    // Store the previous long-short value for calculating the difference
    static LONG PreLongShort = CurLongShort;

    // Calculate the difference between current and previous long-short values
    LONG LongShortDiff = CurLongShort - PreLongShort;
    PreLongShort = CurLongShort;

    // Variable to store the previous long-short difference (used in calculating delta)
    static double PreLongShortDiff = LongShortDiff;

    // Calculate the rate of change (derivative term) for the long-short difference
    double deltaDiff = LongShortDiff - PreLongShortDiff;
    PreLongShortDiff = LongShortDiff;

    // Smooth the derivative term using a weighted average of current and previous difference
    double SmoothedDiff = 0.8 * deltaDiff + 0.2 * PreMaDiff;
    PreMaDiff = SmoothedDiff; // Store the smoothed derivative term for future use

    // Update the global long-short position with the smoothed difference
    gLongShort += LongShortDiff + static_cast<long>(SmoothedDiff * BID_OFFER_SLOPE_LONG_SHORT_PID_D_GAIN);

    // Bound the global long-short position between predefined thresholds to avoid extreme values
    if (gLongShort > 0)
    {
        gLongShort = min(gLongShort, gStrategyConfig.BidOfferLongShortThreshold * 2);
    }
    else
    {
        gLongShort = max(gLongShort, -gStrategyConfig.BidOfferLongShortThreshold * 2);
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

VOID CountNumberOfStocksRisingAndFalling(void)
{
    long nUpNoW = gCurTaiexInfo[0][4] + gCurTaiexInfo[1][4];
    long nDownNoW = gCurTaiexInfo[0][5] + gCurTaiexInfo[1][5];

    gNumberOfStocksRisingAndFalling = nUpNoW - nDownNoW;
}

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

    double latestClosingPriceAvg = 0;
    double CurCount = 0;

    if (count != 0)
    {
        gCostMovingAverageVal = LocalCostMovingAverageVal;

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
        }

        if (CurCount != 0 && latestClosingPriceAvg != 0)
        {
            gCostMovingAverageVal = (gCostMovingAverageVal + latestClosingPriceAvg) / (count + CurCount);
        }

        DEBUG(DEBUG_LEVEL_DEBUG, "gCostMovingAverageVal = %f", gCostMovingAverageVal);
    }

    return gCostMovingAverageVal;
}

DOUBLE CountWeeklyAndMonthlyCosts(LONG MtxCommodtyInfo)
{

    static bool init = FALSE;
    static double WeeklyHigh = INT_MIN, WeeklyLow = INT_MAX;

    if (init == FALSE)
    {
        // Calculate CostMovingAverage by gDaysNightAllCommHighLowPoint

        std::deque<pair<double, double>> DaysNightCostHighLow;

        for (const auto &entry : gDaysNightAllCommHighLowPoint) // need ordered by date  from the past to the present
        {
            auto cur = entry.second;

            DEBUG(DEBUG_LEVEL_DEBUG, "Date: %s, High: %f, Low: %f", entry.first, cur.first, cur.second);

            DaysNightCostHighLow.push_back({cur.first, cur.second});

            if (DaysNightCostHighLow.size() > COST_DAY_MA)
            {
                DaysNightCostHighLow.pop_front();
            }
        }

        if (DaysNightCostHighLow.empty())
        {
            return -1;
        }

        for (auto &x : DaysNightCostHighLow)
        {
            DEBUG(DEBUG_LEVEL_DEBUG, "High: %f, Low: %f", x.first, x.second);

            WeeklyHigh = max(WeeklyHigh, x.first);
            WeeklyLow = min(WeeklyLow, x.second);
        }

        // Calculate CostMovingAverage by gDaysCommHighLowPoint

        std::deque<pair<double, double>> DaysCostHighLow;

        for (const auto &entry : gDaysCommHighLowPoint) // need ordered by date  from the past to the present
        {
            auto cur = entry.second;

            DEBUG(DEBUG_LEVEL_DEBUG, "Date: %s, High: %f, Low: %f", entry.first, cur.first, cur.second);

            DaysCostHighLow.push_back({cur.first, cur.second});

            if (DaysCostHighLow.size() > COST_DAY_MA)
            {
                DaysCostHighLow.pop_front();
            }
        }

        if (DaysCostHighLow.empty())
        {
            return -1;
        }

        for (auto &x : DaysCostHighLow)
        {
            DEBUG(DEBUG_LEVEL_DEBUG, "High: %f, Low: %f", x.first, x.second);

            WeeklyHigh = max(WeeklyHigh, x.first);
            WeeklyLow = min(WeeklyLow, x.second);
        }

        init = TRUE;
    }

    if (WeeklyHigh != INT_MIN && WeeklyLow != INT_MAX)
    {
        if (gCostMovingAverageVal != 0)
        {

            if (gCurCommHighLowPoint.count(gCommodtyInfo.MTXIdxNoAM))
            {
                long CurHigh = gCurCommHighLowPoint[gCommodtyInfo.MTXIdxNoAM][0];
                long CurLow = gCurCommHighLowPoint[gCommodtyInfo.MTXIdxNoAM][1];

                WeeklyHigh = max(WeeklyHigh, static_cast<double>(CurHigh) / 100.0);
                WeeklyLow = min(WeeklyLow, static_cast<double>(CurLow) / 100.0);
            }

            if (gCurCommHighLowPoint.count(gCommodtyInfo.MTXIdxNo))
            {
                long CurHigh = gCurCommHighLowPoint[gCommodtyInfo.MTXIdxNo][0];
                long CurLow = gCurCommHighLowPoint[gCommodtyInfo.MTXIdxNo][1];

                WeeklyHigh = max(WeeklyHigh, static_cast<double>(CurHigh) / 100.0);
                WeeklyLow = min(WeeklyLow, static_cast<double>(CurLow) / 100.0);
            }
        }

        if (gCurCommHighLowPoint.count(MtxCommodtyInfo))
        {
            double CurHigh = static_cast<double>(gCurCommHighLowPoint[MtxCommodtyInfo][0]) / 100.0;
            double CurLow = static_cast<double>(gCurCommHighLowPoint[MtxCommodtyInfo][1]) / 100.0;

            double CurAvg = (CurHigh + CurLow) / 2.0;

            gCostMovingAverageVal = ((WeeklyHigh + WeeklyLow) / 2.0 + CurAvg) / 2;
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

    if (NewClose != ORDER_CLOSE_POSITION && gOpenInterestInfo.NeedToUpdate == FALSE)
    {
        LOG(DEBUG_LEVEL_INFO, "gOpenInterestInfo.NeedToUpdate == FALSE");
        gOpenInterestInfo.NeedToUpdate = TRUE;
    }

#ifdef VIRTUAL_ACCOUNT_ORDER
    NewClose = ORDER_CLOSE_POSITION;
#endif

    if (-gClosedProfitLoss >= gStrategyConfig.MaximumLoss)
    {
        NewClose = ORDER_CLOSE_POSITION;
    }

    gClosedProfitLoss += gOpenInterestInfo.profitAndLoss;

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

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

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

            vector<string> vec = {COMMODITY_TX_MAIN, COMMODITY_OTHER};

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

VOID StrategyTakeFuturesProfit(string strUserId, LONG MtxCommodtyInfo)
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

        SHORT CloseBuySell = -1;

        if (gOpenInterestInfo.buySell == "S")
        {
            CloseBuySell = ORDER_BUY_LONG_POSITION; // need to Buy to take short position profit
        }
        else if (gOpenInterestInfo.buySell == "B")
        {
            CloseBuySell = ORDER_SELL_SHORT_POSITION; // need to Sell to take long position profit
        }

        if (gOpenInterestInfo.profitAndLoss >= gStrategyConfig.MaximumLoss)
        {
            LOG(DEBUG_LEVEL_INFO, "Take profit at curPrice = %f, gOpenInterestInfo.avgCost= %f, profit and loss:%f",
                curPrice, gOpenInterestInfo.avgCost, gOpenInterestInfo.profitAndLoss);

            vector<string> vec = {COMMODITY_TX_MAIN, COMMODITY_OTHER};

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

VOID StrategyClosePositionOnDayTrade(string strUserId, LONG MtxCommodtyInfo, SHORT StopHour, SHORT StopMinute)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    if (gCurServerTime[0] != StopHour || gCurServerTime[1] != StopMinute)
    {
        return;
    }

    double curPrice = 0;

    if (gCurCommPrice.count(MtxCommodtyInfo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[MtxCommodtyInfo]) / 100.0;
    }

    if (gOpenInterestInfo.product != "" && gOpenInterestInfo.avgCost != 0)
    {
        LOG(DEBUG_LEVEL_DEBUG, "product: %s", gOpenInterestInfo.product);
        LOG(DEBUG_LEVEL_DEBUG, "buySell: %s", gOpenInterestInfo.buySell);
        LOG(DEBUG_LEVEL_DEBUG, "openPosition: %ld", gOpenInterestInfo.openPosition);
        LOG(DEBUG_LEVEL_DEBUG, "dayTradePosition: %ld", gOpenInterestInfo.dayTradePosition);
        LOG(DEBUG_LEVEL_DEBUG, "avgCost: %f", gOpenInterestInfo.avgCost);

        LOG(DEBUG_LEVEL_DEBUG, "gOpenInterestInfo.avgCost= %f",
            gOpenInterestInfo.avgCost);

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

        if (BuySell == 0 ||
            BuySell == 1)
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

            LOG(DEBUG_LEVEL_INFO, "Close position, curPrice = %f, gCostMovingAverageVal= %f, BidOfferLongShortSlope: %f",
                curPrice, gCostMovingAverageVal, gBidOfferLongShortSlope);
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

        if ((BuySell == 0 && (curPrice >= EstimatedLongSideKeyPrice() - gStrategyConfig.ActivePoint)) ||
            (BuySell == 1 && (curPrice <= EstimatedShortSideKeyPrice() + gStrategyConfig.ActivePoint)))
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

            LOG(DEBUG_LEVEL_INFO, "Close position, curPrice = %f, gCostMovingAverageVal= %f, BidOfferLongShortSlope: %f",
                curPrice, gCostMovingAverageVal, gBidOfferLongShortSlope);
        }
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
}

VOID StrategyCloseIntervalAmpLongShortPosition(string strUserId, LONG MtxCommodtyInfo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    if (gCostMovingAverageVal == 0)
    {
        return;
    }

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

        double ShockLongExtremeValue = gCostMovingAverageVal - EstimatedTodaysAmplitude() / 2.0;
        double ShockShortExtremeValue = gCostMovingAverageVal + EstimatedTodaysAmplitude() / 2.0;

        if ((BuySell == 0 && curPrice >= ShockShortExtremeValue - gStrategyConfig.ActivePoint) ||
            (BuySell == 1 && curPrice <= ShockLongExtremeValue + gStrategyConfig.ActivePoint))
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

            LOG(DEBUG_LEVEL_INFO, "Close position, curPrice = %f, gCostMovingAverageVal= %f, BidOfferLongShortSlope: %f",
                curPrice, gCostMovingAverageVal, gBidOfferLongShortSlope);
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
        return gDayAmpAndKeyPrice.LongKey1;
    }
    case 2:
    {
        return gDayAmpAndKeyPrice.LongKey2;
    }
    case 3:
    {
        return gDayAmpAndKeyPrice.LongKey3;
    }
    case 4:
    {
        return gDayAmpAndKeyPrice.LongKey4;
    }
    case 5:
    {
        return gDayAmpAndKeyPrice.LongKey5;
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
        return gDayAmpAndKeyPrice.ShortKey1;
    }
    case 2:
    {
        return gDayAmpAndKeyPrice.ShortKey2;
    }
    case 3:
    {
        return gDayAmpAndKeyPrice.ShortKey3;
    }
    case 4:
    {
        return gDayAmpAndKeyPrice.ShortKey4;
    }
    case 5:
    {
        return gDayAmpAndKeyPrice.ShortKey5;
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
        return gDayAmpAndKeyPrice.SmallestAmp - gStrategyConfig.ActivePoint;
    }
    case 2:
    {
        return gDayAmpAndKeyPrice.SmallAmp - gStrategyConfig.ActivePoint;
    }
    case 3:
    {
        return gDayAmpAndKeyPrice.AvgAmp - gStrategyConfig.ActivePoint;
    }
    case 4:
    {
        return gDayAmpAndKeyPrice.LargerAmp - gStrategyConfig.ActivePoint;
    }
    case 5:
    {
        return gDayAmpAndKeyPrice.LargestAmp - gStrategyConfig.ActivePoint;
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
        // Only new positions need to be checked
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
        abs(CurAvg - gCostMovingAverageVal) >= MAXIMUM_COST_AVG_BIAS_RATIO ||
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
            (curPrice <= CurAvg + ATTACK_RANGE) &&                        // Use attack range (ATTACK_RANGE) to control the chance of entering the field
            curPrice > CurLow &&                                          // Dont go short at new highs, dont go long at new lows
            (EstimatedLongSideKeyPrice() - curPrice) >= ONE_STRIKE_PRICES // Earn at least one strike price

        )
        {

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

            LOG(DEBUG_LEVEL_INFO, "New Long position, curPrice = %f, gCostMovingAverageVal= %f, CurAvg= %f, BidOfferLongShortSlope: %f",
                curPrice, gCostMovingAverageVal, CurAvg, gBidOfferLongShortSlope);
        }
    }

    // Do Short

    if (LongShort == 0 && gOpenInterestInfo.openPosition >= 0)
    {
        DEBUG(DEBUG_LEVEL_DEBUG, "curPrice = %f, gOpenInterestInfo.avgCost= %f",
              curPrice, gOpenInterestInfo.avgCost);

        if (CurAvg < gCostMovingAverageVal &&
            curPrice <= gCostMovingAverageVal &&
            (curPrice >= CurAvg - ATTACK_RANGE) &&                         // Use attack range(ATTACK_RANGE) to control the chance of entering the field
            curPrice < CurHigh &&                                          // Dont go short at new highs, dont go long at new lows
            (curPrice - EstimatedShortSideKeyPrice()) >= ONE_STRIKE_PRICES // Earn at least one strike price

        )
        {

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

            LOG(DEBUG_LEVEL_INFO, "New Short position, curPrice = %f, gCostMovingAverageVal= %f, CurAvg= %f, BidOfferLongShortSlope: %f",
                curPrice, gCostMovingAverageVal, CurAvg, gBidOfferLongShortSlope);
        }
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
}

LONG CountBidOfferLongShort(LONG nStockidx)
{

    if (gBest5BidOffer.count(nStockidx) <= 0 || gBest5BidOffer[nStockidx].size() < 10)
    {
        return 0;
    }

    long countLong = 0, countShort = 0;

    static unordered_map<long, long> PrePtr;

    if (gTransactionList.count(nStockidx))
    {
        long nPtr = 0, nBid = 0, nAsk = 0, nClose = 0, nQty = 0;

        nPtr = gTransactionList[nStockidx][0];
        nBid = gTransactionList[nStockidx][1];
        nAsk = gTransactionList[nStockidx][2];
        nClose = gTransactionList[nStockidx][3];
        nQty = gTransactionList[nStockidx][4];

        if (nPtr < 1)
        {
            return 0;
        }

        if (!PrePtr.count(nStockidx) || PrePtr[nStockidx] != nPtr)
        {
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

            if (nClose > 0 && nClose <= nBid)
            {
                // Support Bid buying and force everyone to sell Bid.
                // The purpose is to sell Bid in large quantities.
                long AvgBid = totalBid / 5;

                if (totalOffer * 3 <= totalBid * 2)
                {

                    if (nQty >= BIG_ORDER)
                    {
                        countShort -= nQty;
                    }
                }

                for (int i = 0; i < 5; ++i)
                {
                    if (AvgBid * 3 < gBest5BidOffer[nStockidx][i].second)
                    {
                        // Find unusual pending big Bid orders

                        if (nQty >= BIG_ORDER)
                        {
                            countShort -= nQty;
                        }
                    }
                }
            }

            if (nClose > 0 && nClose >= nAsk)
            {
                // Suppress Offer selling and force everyone to buy Offer.
                // The purpose is to buy Offer in large quantities.

                long AvgOffer = totalOffer / 5;

                if (totalBid * 3 <= totalOffer * 2)
                {

                    if (nQty >= BIG_ORDER)
                    {
                        countLong += nQty;
                    }
                }

                for (int i = 5; i < 10; ++i)
                {
                    if (AvgOffer * 3 < gBest5BidOffer[nStockidx][i].second)
                    {
                        // Find unusual pending big Offer orders

                        if (nQty >= BIG_ORDER)
                        {
                            countLong += nQty;
                        }
                    }
                }
            }

            PrePtr[nStockidx] = nPtr;
        }
    }

    LOG(DEBUG_LEVEL_DEBUG, "countLong = %ld, countShort=%ld", countLong, countShort);

    return BID_OFFER_LONG_SHORT_WEIGHT_RATIO * (countLong + countShort);
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

        if (nPtr < 1)
        {
            return 0;
        }

        if (!PrePtr.count(nStockidx) || PrePtr[nStockidx] != nPtr)
        {
            if (nClose > 0 && nClose <= nBid)
            {
                countShort -= nQty;

                if (nQty >= BIG_ORDER)
                {
                    countShort -= nQty;
                }
            }

            if (nClose > 0 && nClose >= nAsk)
            {
                countLong += nQty;

                if (nQty >= BIG_ORDER)
                {
                    countLong += nQty;
                }
            }

            PrePtr[nStockidx] = nPtr;
        }
    }

    LOG(DEBUG_LEVEL_DEBUG, "countLong = %ld, countShort=%ld", countLong, countShort);

    return TRANSACTION_LIST_LONG_SHORT_WEIGHT_RATIO * (countLong + countShort);
}

/**
 * @brief Handle new tick data and calculate 5MA slope for opening long/short positions based on 1-minute close prices.
 *
 * This function processes tick data for a specific stock index (nStockidx), computes the 5-minute moving average (5MA)
 * using the closing prices of the past 5 minutes, and determines whether to open long or short positions based on the
 * slope of the 5MA (i.e., the difference between the current and previous 5MA values).
 *
 * The strategy is as follows:
 * - Open a long position when the 5MA slope is positive.
 * - Open a short position when the 5MA slope is negative.
 *
 * The function tracks the last processed minute and updates the closing prices deque when a new minute begins.
 * Only the last tick of the minute is used to represent the closing price for that minute.
 *
 * @param nStockidx The stock index for which tick data is being processed.
 * @return int
 *         - 1 if a long position should be opened.
 *         - 0 if a short position should be opened.
 *         - -1 if no action is taken (e.g., not enough data, or no change in position).
 */
LONG CountOsTransactionListLongShort(LONG nStockidx)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    static unordered_map<long, long> PrePtr;
    static unordered_map<long, long> PreClosePrices;

    static long BidOfferFlip = 0;

    long countLong = 0,
         countShort = 0;

    // Ensure that there is data for the given stock index
    if (gOsTransactionList.count(nStockidx))
    {
        // Retrieve tick data from gOsTransactionList for the given stock index
        long nPtr = 0, nClose = 0, nQty = 0, nTimehms = 0;

        nPtr = gOsTransactionList[nStockidx][0];
        nClose = gOsTransactionList[nStockidx][1];
        nQty = gOsTransactionList[nStockidx][2];
        nTimehms = gOsTransactionList[nStockidx][3];

        // Process new tick data only if this is a new pointer (i.e., a new tick)
        if (!PrePtr.count(nStockidx) || PrePtr[nStockidx] != nPtr)
        {

            DEBUG(DEBUG_LEVEL_DEBUG, "nStockIndex: %ld, nPtr: %ld,nTimehms: %ld,nClose: %ld,nQty: %ld\n",
                  nStockidx, nPtr, nTimehms, nClose, nQty);

            // 1 2 3 6 6 6 6 5 5 4 6 7 1 2 2
            // + + + + + + + - - - + + + + +

            if (PreClosePrices.count(nStockidx))
            {
                if (nClose > 0 && PreClosePrices[nStockidx] < nClose)
                {
                    BidOfferFlip = 1;
                    DEBUG(DEBUG_LEVEL_DEBUG, "countLong");
                }

                if (nClose > 0 && PreClosePrices[nStockidx] > nClose)
                {
                    BidOfferFlip = 0;
                    DEBUG(DEBUG_LEVEL_DEBUG, "countShort");
                }

                if (BidOfferFlip == 1)
                {
                    countLong += nQty;
                }
                else
                {
                    countShort -= nQty;
                }
            }

            PreClosePrices[nStockidx] = nClose;

            // Update the last processed pointer to prevent processing the same tick again
            PrePtr[nStockidx] = nPtr;
        }
    }

    return NQ_TRANSACTION_LIST_LONG_SHORT_WEIGHT_RATIO * (countLong + countShort);
}

LONG StrategyCaluBidOfferLongShort(VOID)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    if (gBidOfferLongShort >= INT_MAX || gBidOfferLongShort <= INT_MIN)
    {
        return gBidOfferLongShort;
    }

    if (gCommodtyInfo.TSMCIdxNo != 0)
    {
        long nStockidx = gCommodtyInfo.TSMCIdxNo;

        gBidOfferLongShort += TSMC_BID_OFFER_WEIGHT_RATIO * CountBidOfferLongShort(nStockidx);
    }

    if (gCommodtyInfo.MediaTekIdxNo != 0)
    {
        long nStockidx = gCommodtyInfo.MediaTekIdxNo;

        gBidOfferLongShort += MEDIATEK_BID_OFFER_WEIGHT_RATIO * CountBidOfferLongShort(nStockidx);
    }

    if (gCommodtyInfo.FOXCONNIdxNo != 0)
    {
        long nStockidx = gCommodtyInfo.FOXCONNIdxNo;

        gBidOfferLongShort += FOXCONN_BID_OFFER_WEIGHT_RATIO * CountBidOfferLongShort(nStockidx);
    }

    LOG(DEBUG_LEVEL_DEBUG, "LongShort = %ld", gBidOfferLongShort);

    DEBUG(DEBUG_LEVEL_DEBUG, "End");

    if (gBidOfferLongShort > 0)
    {
        gBidOfferLongShort = min(gBidOfferLongShort, INT_MAX);
    }
    else if (gBidOfferLongShort < 0)
    {
        gBidOfferLongShort = max(gBidOfferLongShort, INT_MIN);
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
    // long FOXCONNIdxNo;
    // long TSEAIdxNo;

    if (gCommodtyInfo.TSMCIdxNo != -1)
    {
        long nStockidx = gCommodtyInfo.TSMCIdxNo;

        gTransactionListLongShort += TSMC_BID_OFFER_WEIGHT_RATIO * CountTransactionListLongShort(nStockidx);
    }

    if (gCommodtyInfo.MediaTekIdxNo != -1)
    {
        long nStockidx = gCommodtyInfo.MediaTekIdxNo;

        gTransactionListLongShort += MEDIATEK_BID_OFFER_WEIGHT_RATIO * CountTransactionListLongShort(nStockidx);
    }

    if (gCommodtyInfo.FOXCONNIdxNo != -1)
    {
        long nStockidx = gCommodtyInfo.FOXCONNIdxNo;

        gTransactionListLongShort += FOXCONN_BID_OFFER_WEIGHT_RATIO * CountTransactionListLongShort(nStockidx);
    }

    if (gCommodtyInfo.MTXIdxNo != -1)
    {
        long nStockidx = gCommodtyInfo.MTXIdxNo;

        gTransactionListLongShort += TX_BID_OFFER_WEIGHT_RATIO * CountTransactionListLongShort(nStockidx);
    }

    if (gCommodtyInfo.MTXIdxNoAM != -1)
    {
        long nStockidx = gCommodtyInfo.MTXIdxNoAM;

        gTransactionListLongShort += TX_BID_OFFER_WEIGHT_RATIO * CountTransactionListLongShort(nStockidx);
    }

    for (int i = 0; i < gLeadingCommodtyInfo.size(); ++i)
    {
        long nStockidx = gLeadingCommodtyInfo[i].second;
        DEBUG(DEBUG_LEVEL_DEBUG, "LeadingCommodtyInfo[i].second=%ld", gLeadingCommodtyInfo[i].second);

        gTransactionListLongShort += LEADING_STOCKS_BID_OFFER_WEIGHT_RATIO * CountTransactionListLongShort(nStockidx);
    }

    LOG(DEBUG_LEVEL_DEBUG, "LongShort = %ld", gTransactionListLongShort);

    DEBUG(DEBUG_LEVEL_DEBUG, "End");

    if (gTransactionListLongShort > 0)
    {
        gTransactionListLongShort = min(gTransactionListLongShort, INT_MAX);
    }
    else if (gTransactionListLongShort < 0)
    {
        gTransactionListLongShort = max(gTransactionListLongShort, INT_MIN);
    }

    return gTransactionListLongShort;
}

LONG StrategyCaluOsTransactionListLongShort(VOID)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    if (gOsTransactionListLongShort >= INT_MAX || gOsTransactionListLongShort <= INT_MIN)
    {
        return gOsTransactionListLongShort;
    }

    if (gCommodtyOsInfo.NQIdxNo != 0)
    {
        long nStockidx = gCommodtyOsInfo.NQIdxNo;

        gOsTransactionListLongShort += CountOsTransactionListLongShort(nStockidx);
    }

    LOG(DEBUG_LEVEL_DEBUG, "LongShort = %ld", gOsTransactionListLongShort);

    DEBUG(DEBUG_LEVEL_DEBUG, "End");

    if (gOsTransactionListLongShort > 0)
    {
        gOsTransactionListLongShort = min(gOsTransactionListLongShort, INT_MAX);
    }
    else if (gOsTransactionListLongShort < 0)
    {
        gOsTransactionListLongShort = max(gOsTransactionListLongShort, INT_MIN);
    }

    return gOsTransactionListLongShort;
}

LONG StrategyCaluLongShort(VOID)
{
    return (gTransactionListLongShort + gBidOfferLongShort + gOsTransactionListLongShort) / LONG_AND_SHORT_TARGET_COUNT;
}

VOID StrategyNewIntervalAmpLongShortPosition(string strUserId, LONG MtxCommodtyInfo, LONG LongShort)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    if (gOpenInterestInfo.NeedToUpdate == TRUE)
    {
        // Only new positions need to be checked
        LOG(DEBUG_LEVEL_DEBUG, "gOpenInterestInfo.NeedToUpdate == TRUE");
        return;
    }

    if (gCurCommHighLowPoint.count(MtxCommodtyInfo) == 0)
    {
        return;
    }

    double curPrice = 0;

    if (gCurCommPrice.count(MtxCommodtyInfo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[MtxCommodtyInfo]) / 100.0;
    }

    if (curPrice > 0 && gCostMovingAverageVal > 0)
    {
        // Do Long

        if (LongShort == 1 && gOpenInterestInfo.openPosition <= 0)
        {
            DEBUG(DEBUG_LEVEL_DEBUG, "curPrice = %f, gOpenInterestInfo.avgCost= %f",
                  curPrice, gOpenInterestInfo.avgCost);

            double ShockLongExtremeValue = gCostMovingAverageVal - EstimatedTodaysAmplitude() / 2.0;

            if (curPrice <= ShockLongExtremeValue + gStrategyConfig.ActivePoint &&
                curPrice >= ShockLongExtremeValue - ATTACK_RANGE)
            {
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

                LOG(DEBUG_LEVEL_INFO, "New Long position, curPrice = %f, gCostMovingAverageVal= %f, BidOfferLongShortSlope: %f",
                    curPrice, gCostMovingAverageVal, gBidOfferLongShortSlope);
            }
        }

        // Do Short

        if (LongShort == 0 && gOpenInterestInfo.openPosition >= 0)
        {
            DEBUG(DEBUG_LEVEL_DEBUG, "curPrice = %f, gOpenInterestInfo.avgCost= %f",
                  curPrice, gOpenInterestInfo.avgCost);

            double ShockShortExtremeValue = gCostMovingAverageVal + EstimatedTodaysAmplitude() / 2.0;

            if (curPrice >= ShockShortExtremeValue - gStrategyConfig.ActivePoint &&
                curPrice <= ShockShortExtremeValue + ATTACK_RANGE)
            {
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

                LOG(DEBUG_LEVEL_INFO, "New Short position, curPrice = %f, gCostMovingAverageVal= %f, BidOfferLongShortSlope: %f",
                    curPrice, gCostMovingAverageVal, gBidOfferLongShortSlope);
            }
        }
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
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

VOID StrategyCloseMainForcePassPreHighAndBreakPreLowPosition(string strUserId, LONG MtxCommodtyInfo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    double curPrice = 0;

    if (gCurCommPrice.count(MtxCommodtyInfo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[MtxCommodtyInfo]) / 100.0;
    }

    double CurHigh = 0, CurLow = 0;

    if (gCurCommHighLowPoint.count(MtxCommodtyInfo) > 0)
    {
        CurHigh = gCurCommHighLowPoint[MtxCommodtyInfo][0] / 100.0;
        CurLow = gCurCommHighLowPoint[MtxCommodtyInfo][1] / 100.0;

        if (gOpenInterestInfo.NeedToUpdate == TRUE)
        {
            LOG(DEBUG_LEVEL_DEBUG, "gOpenInterestInfo.NeedToUpdate == TRUE");
            gEntryHigh = CurHigh;
            gEntryLow = CurLow;
            return;
        }
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

        if (gEntryHigh == 0 || gEntryLow == 0)
        {
            return;
        }

        if (gOpenInterestInfo.profitAndLoss < PROFIT_STOP_TICK * DOLLARS_PER_TICK)
        {
            return;
        }

        if ((BuySell == 0 && curPrice > gEntryHigh && gMa5LongShort < 0) ||
            (BuySell == 1 && curPrice < gEntryLow && gMa5LongShort > 0))
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

            LOG(DEBUG_LEVEL_INFO, "Close position, curPrice = %f, gCostMovingAverageVal= %f, BidOfferLongShortSlope: %f",
                curPrice, gCostMovingAverageVal, gBidOfferLongShortSlope);
        }
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
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

VOID StrategyCloseBidOfferLongShortSlope(string strUserId, LONG MtxCommodtyInfo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    double curPrice = 0;

    if (gCurCommPrice.count(MtxCommodtyInfo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[MtxCommodtyInfo]) / 100.0;
    }

    if (gOpenInterestInfo.NeedToUpdate == TRUE)
    {
        LOG(DEBUG_LEVEL_DEBUG, "gOpenInterestInfo.NeedToUpdate == TRUE");
        return;
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

        if ((BuySell == 0 && gBidOfferLongShortSlope <= -CLOSE_BID_OFFER_SLOPE_LONG_SHORT) ||
            (BuySell == 1 && gBidOfferLongShortSlope >= CLOSE_BID_OFFER_SLOPE_LONG_SHORT))
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

            LOG(DEBUG_LEVEL_INFO, "Close position, curPrice = %f, gCostMovingAverageVal= %f, BidOfferLongShortSlope: %f",
                curPrice, gCostMovingAverageVal, gBidOfferLongShortSlope);
        }
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
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

VOID StrategyCloseFixedTakeProfit(string strUserId, LONG MtxCommodtyInfo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    double curPrice = 0;

    if (gCurCommPrice.count(MtxCommodtyInfo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[MtxCommodtyInfo]) / 100.0;
    }

    if (gOpenInterestInfo.NeedToUpdate == TRUE)
    {
        LOG(DEBUG_LEVEL_DEBUG, "gOpenInterestInfo.NeedToUpdate == TRUE");
        return;
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

        if (gOpenInterestInfo.profitAndLoss < abs(gOpenInterestInfo.openPosition) * PROFIT_STOP_TICK * DOLLARS_PER_TICK)
        {
            return;
        }

        bool PrepareToLeaveFirst = (BuySell == 0 && gBidOfferLongShortSlope < 0) ||
                                   (BuySell == 1 && gBidOfferLongShortSlope > 0);

        bool JustMakeOneRound = (BuySell == 0 && gBidOfferLongShortSlope >= CLOSE_BID_OFFER_SLOPE_LONG_SHORT) ||
                                (BuySell == 1 && gBidOfferLongShortSlope <= -CLOSE_BID_OFFER_SLOPE_LONG_SHORT);

        if ((BuySell == 0 && gMa5LongShort < 0) ||
            (BuySell == 1 && gMa5LongShort > 0) ||
            (gOpenInterestInfo.profitAndLoss > 2 * abs(gOpenInterestInfo.openPosition) * PROFIT_STOP_TICK * DOLLARS_PER_TICK && PrepareToLeaveFirst) ||
            JustMakeOneRound)
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

            LOG(DEBUG_LEVEL_INFO, "Close position, curPrice = %f, gCostMovingAverageVal= %f, BidOfferLongShortSlope: %f",
                curPrice, gCostMovingAverageVal, gBidOfferLongShortSlope);
        }
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
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

VOID StrategyCloseOneRoundTakeProfit(string strUserId, LONG MtxCommodtyInfo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    double curPrice = 0;

    if (gCurCommPrice.count(MtxCommodtyInfo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[MtxCommodtyInfo]) / 100.0;
    }

    if (gOpenInterestInfo.NeedToUpdate == TRUE)
    {
        LOG(DEBUG_LEVEL_DEBUG, "gOpenInterestInfo.NeedToUpdate == TRUE");
        return;
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

        bool PrepareToLeaveFirst = (BuySell == 0 &&
                                    gBidOfferLongShortSlope >= gStrategyConfig.BidOfferLongShortAttackSlope * 2.0 &&
                                    gMa5LongShort > MAXIMUM_5MA_BIAS_RATIO * 2.0) ||
                                   (BuySell == 1 &&
                                    -gBidOfferLongShortSlope >= gStrategyConfig.BidOfferLongShortAttackSlope * 2.0 &&
                                    gMa5LongShort < -MAXIMUM_5MA_BIAS_RATIO * 2.0);

        if (PrepareToLeaveFirst)
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

            LOG(DEBUG_LEVEL_INFO, "Close position, curPrice = %f, gCostMovingAverageVal= %f, BidOfferLongShortSlope: %f",
                curPrice, gCostMovingAverageVal, gBidOfferLongShortSlope);
        }
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
}

LONG EvaluateTheMaximumPosition(LONG MtxCommodtyInfo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    static bool CanAddPositions = FALSE;
    static DOUBLE MaxProfit = 0;
    static LONG EvaluatePosition = MAXIMUM_NUMBERS_OF_POSITIONS;

    double curPrice = 0;

    if (gCurCommPrice.count(MtxCommodtyInfo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[MtxCommodtyInfo]) / 100.0;
    }

    if (gOpenInterestInfo.NeedToUpdate == TRUE)
    {
        LOG(DEBUG_LEVEL_DEBUG, "gOpenInterestInfo.NeedToUpdate == TRUE");
        return EvaluatePosition;
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

        if (gOpenInterestInfo.profitAndLoss < EvaluatePosition * PROFIT_STOP_TICK * DOLLARS_PER_TICK && CanAddPositions == FALSE)
        {
            return MAXIMUM_NUMBERS_OF_POSITIONS;
        }

        CanAddPositions = TRUE;

        MaxProfit = max(MaxProfit, gOpenInterestInfo.profitAndLoss);

        if (gOpenInterestInfo.profitAndLoss >= 0 &&
            gOpenInterestInfo.profitAndLoss <= MaxProfit - MaxProfit / 3)
        {
            EvaluatePosition = max(EvaluatePosition, abs(gOpenInterestInfo.openPosition) + 1);
        }
        else
        {
            --EvaluatePosition;
            EvaluatePosition = max(EvaluatePosition, MAXIMUM_NUMBERS_OF_POSITIONS);
        }
    }
    else
    {
        MaxProfit = 0;
        CanAddPositions = FALSE;
        EvaluatePosition = MAXIMUM_NUMBERS_OF_POSITIONS;
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");

    return EvaluatePosition;
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
VOID StrategyNewDailyAmplitudeAchievesReverse(string strUserId, LONG MtxCommodtyInfo, LONG LongShort)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    if (gOpenInterestInfo.NeedToUpdate == TRUE)
    {
        LOG(DEBUG_LEVEL_DEBUG, "gOpenInterestInfo.NeedToUpdate == TRUE");
        return;
    }

    double curPrice = 0;

    // Get the current price for the commodity
    if (gCurCommPrice.count(MtxCommodtyInfo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[MtxCommodtyInfo]) / 100.0;
    }

    double CurAvg = 0;
    double CurAmp = 0;
    double CurHigh = 0, CurLow = 0;

    // Calculate the current high, low, amplitude, and average
    if (gCurCommHighLowPoint.count(MtxCommodtyInfo) > 0)
    {
        CurHigh = gCurCommHighLowPoint[MtxCommodtyInfo][0] / 100.0;
        CurLow = gCurCommHighLowPoint[MtxCommodtyInfo][1] / 100.0;
        CurAmp = CurHigh - CurLow;
        CurAvg = (CurHigh + CurLow) / 2;
    }

    if (curPrice <= 0 || CurHigh <= 0 || CurLow <= 0)
    {
        return;
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "curPrice = %f, CurAvg= %f, gCostMovingAverageVal=%f",
          curPrice, CurAvg, gCostMovingAverageVal);

    // Strategy for going long
    if (LongShort == 1 && gOpenInterestInfo.openPosition <= 0)
    {
        DEBUG(DEBUG_LEVEL_DEBUG, "curPrice = %f, gOpenInterestInfo.avgCost= %f",
              curPrice, gOpenInterestInfo.avgCost);

        if (curPrice - ATTACK_RANGE < EstimatedShortSideKeyPrice() &&
            curPrice + ATTACK_RANGE > EstimatedShortSideKeyPrice() &&
            curPrice > CurLow)
        {
            vector<string> vec = {COMMODITY_OTHER};

            for (size_t i = 0; i < vec.size(); ++i)
            {
                AutoOrder(vec[i], ORDER_AUTO_POSITION, ORDER_BUY_LONG_POSITION);
            }

            // Update position and average cost
            gOpenInterestInfo.product = COMMODITY_OTHER;
            gOpenInterestInfo.buySell = "B";
            gOpenInterestInfo.openPosition += 1;
            gOpenInterestInfo.avgCost = curPrice;

            LOG(DEBUG_LEVEL_INFO, "New Long position, curPrice = %f, gCostMovingAverageVal= %f, CurAvg= %f, BidOfferLongShortSlope: %f",
                curPrice, gCostMovingAverageVal, CurAvg, gBidOfferLongShortSlope);
        }
    }

    // Strategy for going short
    if (LongShort == 0 && gOpenInterestInfo.openPosition >= 0)
    {
        DEBUG(DEBUG_LEVEL_DEBUG, "curPrice = %f, gOpenInterestInfo.avgCost= %f",
              curPrice, gOpenInterestInfo.avgCost);

        if (curPrice - ATTACK_RANGE < EstimatedLongSideKeyPrice() &&
            curPrice + ATTACK_RANGE > EstimatedLongSideKeyPrice() &&
            curPrice < CurHigh)
        {
            vector<string> vec = {COMMODITY_OTHER};

            for (size_t i = 0; i < vec.size(); ++i)
            {
                AutoOrder(vec[i], ORDER_AUTO_POSITION, ORDER_SELL_SHORT_POSITION);
            }

            // Update position and average cost
            gOpenInterestInfo.product = COMMODITY_OTHER;
            gOpenInterestInfo.buySell = "S";
            gOpenInterestInfo.openPosition -= 1;
            gOpenInterestInfo.avgCost = curPrice;

            LOG(DEBUG_LEVEL_INFO, "New Short position, curPrice = %f, gCostMovingAverageVal= %f, CurAvg= %f, BidOfferLongShortSlope: %f",
                curPrice, gCostMovingAverageVal, CurAvg, gBidOfferLongShortSlope);
        }
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
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

int CostAverageBiasBigV(LONG MtxCommodtyInfo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    if (gOpenInterestInfo.NeedToUpdate == TRUE)
    {
        // Only new positions need to be checked
        LOG(DEBUG_LEVEL_DEBUG, "gOpenInterestInfo.NeedToUpdate == TRUE");
        return 2;
    }

    if (gCurCommHighLowPoint.count(MtxCommodtyInfo) == 0)
    {
        return 2;
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

    if (curPrice <= 0 || gCostMovingAverageVal <= 0)
    {
        return 2;
    }

    // Do Long

    if (CurAvg > gCostMovingAverageVal &&
        curPrice <= gCostMovingAverageVal)
    {
        return 1;
    }

    // Do Short

    if (CurAvg < gCostMovingAverageVal &&
        curPrice >= gCostMovingAverageVal)
    {
        return -1;
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");

    return 0;
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

int CostAverageBigLongOrShort(LONG MtxCommodtyInfo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    if (gOpenInterestInfo.NeedToUpdate == TRUE)
    {
        // Only new positions need to be checked
        LOG(DEBUG_LEVEL_DEBUG, "gOpenInterestInfo.NeedToUpdate == TRUE");
        return 2;
    }

    if (gCurCommHighLowPoint.count(MtxCommodtyInfo) == 0)
    {
        return 2;
    }

    double curPrice = 0;

    if (gCurCommPrice.count(MtxCommodtyInfo) != 0)
    {
        curPrice = static_cast<double>(gCurCommPrice[MtxCommodtyInfo]) / 100.0;
    }

    if (curPrice <= 0 || gCostMovingAverageVal <= 0)
    {
        return 2;
    }

    // Do Long

    if (curPrice > gCostMovingAverageVal)
    {
        return 1;
    }

    // Do Short

    if (curPrice < gCostMovingAverageVal)
    {
        return -1;
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");

    return 0;
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

VOID StrategySimpleNewLongShortPosition(string strUserId, LONG MtxCommodtyInfo, LONG LongShort)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    if (gOpenInterestInfo.NeedToUpdate == TRUE)
    {
        // Only new positions need to be checked
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

    // Do Long

    if (LongShort == 1 && gOpenInterestInfo.openPosition < gEvaluatePosition)
    {
        vector<string> vec = {COMMODITY_OTHER};

        for (auto &x : vec)
        {
            AutoOrder(x,
                      ORDER_AUTO_POSITION,    // New
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

    // Do Short

    if (LongShort == 0 && gOpenInterestInfo.openPosition > -gEvaluatePosition)
    {
        vector<string> vec = {COMMODITY_OTHER};

        for (auto &x : vec)
        {
            AutoOrder(x,
                      ORDER_AUTO_POSITION,      // New
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

    LOG(DEBUG_LEVEL_INFO, "New position: %ld, curPrice = %f, Ma5LongShort= %f, BidOfferLongShortSlope: %f, LongShort: %ld",
        gOpenInterestInfo.openPosition, curPrice, gMa5LongShort, gBidOfferLongShortSlope, gLongShort);

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
}

VOID StrategySwitch(IN LONG Mode, IN LONG MtxCommodtyInfo)
{
    Count5MaForNewLongShortPosition(gCommodtyInfo.MTXIdxNo);
    CountOsNQ20MaForNewLongShortPosition(gCommodtyOsInfo.NQIdxNo);

    StrategyCaluBidOfferLongShort();
    StrategyCaluTransactionListLongShort();
    StrategyCaluOsTransactionListLongShort();
    BidOfferAndTransactionListLongShortSlope();
    CountNumberOfStocksRisingAndFalling();

    if (!(gCurServerTime[0] <= 5 || gCurServerTime[0] >= 15) &&
        !(gCurServerTime[0] >= 8 && gCurServerTime[0] < 14))
    {
        return;
    }

    switch (Mode)
    {

    // The middle-aged man's trading method
    // Trend plate (positions can be left), rely on the relative position of the average price and cost price, and the imbalance of the main orders to judge the direction.
    // If today's amplitude does not meet the standard to open a position, at least push it to the checkpoint price before exiting.
    case 1:
    {
        StrategyStopFuturesLoss(g_strUserId, MtxCommodtyInfo);
        StrategyClosePosition(g_strUserId, MtxCommodtyInfo);

        if (gCurServerTime[0] < 8 || gCurServerTime[0] >= 15)
        {
#if NIGHT_TRADING

            StrategyNewLongShortPosition(g_strUserId, MtxCommodtyInfo, 1);
            StrategyNewLongShortPosition(g_strUserId, MtxCommodtyInfo, 0);
#endif
        }
        else
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

        break;
    }

        // Long and short divergence money brushing strategy(current night trading strategy)
        // In shock trading(positions can be left),
        // open a counter - trend position at the opposite checkpoint price and exit at the relative checkpoint price.

    case 2:
    {

        StrategyStopFuturesLoss(g_strUserId, MtxCommodtyInfo);
        StrategyCloseIntervalAmpLongShortPosition(g_strUserId, MtxCommodtyInfo);

        if (gCurServerTime[0] < 8 || gCurServerTime[0] >= 15)
        {
            StrategyNewIntervalAmpLongShortPosition(g_strUserId, MtxCommodtyInfo, 1);
            StrategyNewIntervalAmpLongShortPosition(g_strUserId, MtxCommodtyInfo, 0);
        }

        if (gCurServerTime[0] >= 8 && gCurServerTime[0] < 14)
        {
            StrategyNewIntervalAmpLongShortPosition(g_strUserId, MtxCommodtyInfo, 1);
            StrategyNewIntervalAmpLongShortPosition(g_strUserId, MtxCommodtyInfo, 0);
        }

        break;
    }

    case 4:
    {
        StrategyStopFuturesLoss(g_strUserId, MtxCommodtyInfo);
        StrategyClosePositionOnDayTrade(g_strUserId, MtxCommodtyInfo, 13, 30);
        StrategyClosePosition(g_strUserId, MtxCommodtyInfo);

        if (gCurServerTime[0] >= 8 || gCurServerTime[0] <= 13)
        {
            if (StrategyCaluLongShort() >= gStrategyConfig.BidOfferLongShortThreshold)
            {
                StrategyNewDailyAmplitudeAchievesReverse(g_strUserId, MtxCommodtyInfo, 1);
            }
            else if (-StrategyCaluLongShort() >= gStrategyConfig.BidOfferLongShortThreshold)
            {
                StrategyNewDailyAmplitudeAchievesReverse(g_strUserId, MtxCommodtyInfo, 0);
            }
        }

        break;
    }

        // The main short term strategy is to pass the previous high and break the previous low.
        // A shock or trend order(when used as a day trading order) records the previous high or low of the day,
        // and pulls back or rebounds by more than one execution price,
        // and the current price is not the current low or the current high, and the five levels of pending orders are obviously unbalanced,
        // open a position, and exceed the previous price.Exit low before breaking high

    case 7:
    {
        StrategyStopFuturesLoss(g_strUserId, MtxCommodtyInfo);
        StrategyClosePositionOnDayTrade(g_strUserId, MtxCommodtyInfo, 13, 30);
        StrategyClosePosition(g_strUserId, MtxCommodtyInfo);
        StrategyCloseFixedTakeProfit(g_strUserId, MtxCommodtyInfo);
        StrategyCloseBidOfferLongShortSlope(g_strUserId, MtxCommodtyInfo);

        gEvaluatePosition = EvaluateTheMaximumPosition(MtxCommodtyInfo);
        BOOLEAN ReachTodayAmplitude = TodayAmplitudeHasBeenReached(MtxCommodtyInfo);

        if (ReachTodayAmplitude == TRUE)
        {
            break;
        }

        int LongShortExtremeFit = LongShortExtremeFitRate(MtxCommodtyInfo);

        if (gCurServerTime[0] >= 8 || gCurServerTime[0] <= 13)
        {

            if (StrategyCaluLongShort() >= gStrategyConfig.BidOfferLongShortThreshold &&
                gMa5LongShort > 0 &&
                gBidOfferLongShortSlope > 0 &&
                LongShortExtremeFit != -1)
            {
                StrategySimpleNewLongShortPosition(g_strUserId, MtxCommodtyInfo, 1);
            }
            else if (-StrategyCaluLongShort() >= gStrategyConfig.BidOfferLongShortThreshold &&
                     gMa5LongShort < 0 &&
                     gBidOfferLongShortSlope < 0 &&
                     LongShortExtremeFit != 1)
            {
                StrategySimpleNewLongShortPosition(g_strUserId, MtxCommodtyInfo, 0);
            }
        }

        break;
    }
    case 8:
    {
        StrategyStopFuturesLoss(g_strUserId, MtxCommodtyInfo);
        StrategyClosePositionOnDayTrade(g_strUserId, MtxCommodtyInfo, 13, 30);
        StrategyClosePosition(g_strUserId, MtxCommodtyInfo);
        StrategyCloseOneRoundTakeProfit(g_strUserId, MtxCommodtyInfo);
        StrategyCloseBidOfferLongShortSlope(g_strUserId, MtxCommodtyInfo);

        gEvaluatePosition = EvaluateTheMaximumPosition(MtxCommodtyInfo);
        BOOLEAN ReachTodayAmplitude = TodayAmplitudeHasBeenReached(MtxCommodtyInfo);

        if (ReachTodayAmplitude == TRUE)
        {
            break;
        }

        int LongShortExtremeFit = LongShortExtremeFitRate(MtxCommodtyInfo);

        if (gBidOfferLongShortSlope >= gStrategyConfig.BidOfferLongShortAttackSlope &&
            LongShortExtremeFit != -1)
        {
            StrategySimpleNewLongShortPosition(g_strUserId, MtxCommodtyInfo, 1);
        }
        else if (-gBidOfferLongShortSlope >= gStrategyConfig.BidOfferLongShortAttackSlope &&
                 LongShortExtremeFit != 1)
        {
            StrategySimpleNewLongShortPosition(g_strUserId, MtxCommodtyInfo, 0);
        }

        break;
    }

    case 9:
    {
        StrategyStopFuturesLoss(g_strUserId, MtxCommodtyInfo);
        StrategyTakeFuturesProfit(g_strUserId, MtxCommodtyInfo);
        StrategyClosePosition(g_strUserId, MtxCommodtyInfo);
        StrategyCloseOneRoundTakeProfit(g_strUserId, MtxCommodtyInfo);
        StrategyClosePositionOnDayTrade(g_strUserId, MtxCommodtyInfo, 13, 33);

        gEvaluatePosition = EvaluateTheMaximumPosition(MtxCommodtyInfo);

        BOOLEAN ReachTodayAmplitude = TodayAmplitudeHasBeenReached(MtxCommodtyInfo);

        if (ReachTodayAmplitude == TRUE)
        {
            break;
        }

        if (gMa5LongShort > -TURNING_EXTREME_5MA_BIAS_RATIO &&
            gMa5LongShort < TURNING_EXTREME_5MA_BIAS_RATIO)
        {
            if (gBidOfferLongShortSlope >= gStrategyConfig.BidOfferLongShortAttackSlope &&
                gLongShort >= gStrategyConfig.BidOfferLongShortThreshold)
            {
                StrategySimpleNewLongShortPosition(g_strUserId, MtxCommodtyInfo, 1);
            }
            else if (-gBidOfferLongShortSlope >= gStrategyConfig.BidOfferLongShortAttackSlope &&
                     -gLongShort >= gStrategyConfig.BidOfferLongShortThreshold)
            {
                StrategySimpleNewLongShortPosition(g_strUserId, MtxCommodtyInfo, 0);
            }
        }

        break;
    }

    default:
    {
        break;
    }
    }
}
