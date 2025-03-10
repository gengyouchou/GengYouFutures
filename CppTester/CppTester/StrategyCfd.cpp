#include "httplib.h"
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
#include "sqlite3.h"
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>

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
 * @return double
 */
double CfdBidOfferAndTransactionListLongShortSlope(long nStockidx)
{
    // Deques to store recent values for calculating moving average (MA) and slope
    static std::unordered_map<long, deque<double>> dq, dqSlop;

    static unordered_map<long, long> PreLongShort;

    // Get the current long-short position by invoking a custom function
    LONG CurLongShort = gCfdTransactionListLongShort[nStockidx];

    if (!PreLongShort.count(nStockidx))
    {
        // Store the previous long-short value for calculating the difference
        PreLongShort[nStockidx] = CurLongShort;
    }

    // Calculate the difference between current and previous long-short values
    LONG LongShortDiff = CurLongShort - PreLongShort[nStockidx];
    PreLongShort[nStockidx] = CurLongShort;

    if (LongShortDiff == 0)
    {
        return gCfdTransactionListLongShortSlope[nStockidx];
    }

    // Manage the size of the deque to store the recent long-short values for moving average calculation
    if (dq[nStockidx].size() >= BID_OFFER_SLOPE_LONG_SHORT_COUNT)
    {
        dq[nStockidx].pop_front(); // Remove the oldest value
    }
    dq[nStockidx].push_back(CurLongShort); // Add the current long-short value

    // Ensure the deque has enough values to calculate the moving average and slope
    if (dq[nStockidx].size() >= BID_OFFER_SLOPE_LONG_SHORT_COUNT)
    {
        // Calculate the moving average of the deque
        double ma = calculate5MA(dq[nStockidx]);

        // Maintain a deque for the moving average values to compute the slope
        if (dqSlop[nStockidx].size() >= BID_OFFER_SLOPE_LONG_SHORT_COUNT)
        {
            dqSlop[nStockidx].pop_front(); // Remove the oldest MA value
        }
        dqSlop[nStockidx].push_back(ma); // Add the new MA value

        // Calculate the slope based on the difference between the first and last values in the deque
        double deltaY = dqSlop[nStockidx].back() - dqSlop[nStockidx].front();
        double MaSlope = deltaY / BID_OFFER_SLOPE_LONG_SHORT_COUNT;

        // Update the global variable that tracks the bid-offer long-short slope
        return MaSlope;
    }

    return 0;
}

VOID CfdStrategySwitch()
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    if (gCommodtyOsInfo.GCIdxNo != 0)
    {
        long nStockidx = gCommodtyOsInfo.GCIdxNo;

        gCfdTransactionListLongShort[nStockidx] += CountOsTransactionListLongShort(nStockidx);
        gCfdTransactionListLongShortSlope[nStockidx] = CfdBidOfferAndTransactionListLongShortSlope(nStockidx);

        DEBUG(DEBUG_LEVEL_DEBUG, "nStockidx: %ld, gCfdTransactionListLongShort[nStockidx]: %ld\n",
              nStockidx, gCfdTransactionListLongShort[nStockidx]);
    }

    if (gCommodtyOsInfo.NQIdxNo != 0)
    {
        long nStockidx = gCommodtyOsInfo.NQIdxNo;

        gCfdTransactionListLongShort[nStockidx] += CountOsTransactionListLongShort(nStockidx);
        gCfdTransactionListLongShortSlope[nStockidx] = CfdBidOfferAndTransactionListLongShortSlope(nStockidx);

        DEBUG(DEBUG_LEVEL_DEBUG, "nStockidx: %ld, gCfdTransactionListLongShort[nStockidx]: %ld\n",
              nStockidx, gCfdTransactionListLongShort[nStockidx]);
    }

    if (gCommodtyOsInfo.DXIdxNo != 0)
    {
        long nStockidx = gCommodtyOsInfo.DXIdxNo;

        gCfdTransactionListLongShort[nStockidx] += CountOsTransactionListLongShort(nStockidx);
        gCfdTransactionListLongShortSlope[nStockidx] = CfdBidOfferAndTransactionListLongShortSlope(nStockidx);

        DEBUG(DEBUG_LEVEL_DEBUG, "nStockidx: %ld, gCfdTransactionListLongShort[nStockidx]: %ld\n",
              nStockidx, gCfdTransactionListLongShort[nStockidx]);
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

// 假設全局變數 db 為 SQLite3 資料庫指標
extern sqlite3 *db;

// 假設 SendOrderJsonSignal 已經實作，以下是範例實作（若您已有實作，可直接呼叫）
void SendOrderJsonSignal(const std::string &orderJson)
{
    httplib::Client cli("http://127.0.0.1", 1688);
    cli.set_connection_timeout(5, 0); // 連線超時 5 秒

    auto res = cli.Post("/createPosition", orderJson, "application/json");

    if (res && res->status == 200)
    {
        DEBUG(DEBUG_LEVEL_DEBUG, "SendOrderJsonSignal succeeded. Response: %s\n", res->body.c_str());
        std::cout << "SendOrderJsonSignal succeeded. Response: " << res->body << std::endl;
    }
    else
    {
        if (res)
        {
            DEBUG(DEBUG_LEVEL_DEBUG, "SendOrderJsonSignal failed. HTTP status: %d\n", res->status);
            std::cout << "SendOrderJsonSignal failed. HTTP status: " << res->status << std::endl;
        }
        else
        {
            DEBUG(DEBUG_LEVEL_DEBUG, "SendOrderJsonSignal failed. No response received.\n");
            std::cout << "SendOrderJsonSignal failed. No response received." << std::endl;
        }
    }
}

// 新增策略函數：檢查資料庫中的歷史資料，並根據條件建立多單 JSON 信號
void CheckAndPlaceLongOrder(const std::string &commodityId)
{
    // SQL 查詢：查詢過去 3 天該商品的歷史資料（請根據實際資料表名稱及欄位調整）
    std::string query =
        "SELECT timestamp, CurBidOfferLongShortSlope "
        "FROM cache_data "
        "WHERE CommodityId = ? AND timestamp >= datetime('now','-3 days') "
        "ORDER BY timestamp ASC;";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
    sqlite3_bind_text(stmt, 1, commodityId.c_str(), -1, SQLITE_TRANSIENT);

    int turningPoints = 0;
    bool isBelow = false;
    double latestSlope = 0.0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        double slope = sqlite3_column_double(stmt, 1);
        latestSlope = slope; // 更新最新數值
        if (!isBelow && slope < -50.0)
        {
            isBelow = true;
        }
        else if (isBelow && slope >= 0.0)
        {
            turningPoints++;
            isBelow = false;
        }
    }
    sqlite3_finalize(stmt);

    std::cout << "[CheckAndPlaceLongOrder] Commodity: " << commodityId
              << ", TurningPoints: " << turningPoints
              << ", LatestSlope: " << latestSlope << std::endl;
    DEBUG(DEBUG_LEVEL_DEBUG, "[CheckAndPlaceLongOrder] Commodity: %s, TurningPoints: %d, LatestSlope: %f\n",
          commodityId.c_str(), turningPoints, latestSlope);

    // 條件判斷：如果至少有 3 次拐點且最新 slope > 50，則建立多單 JSON 信號
    if (turningPoints >= 3 && latestSlope > 50.0)
    {
        nlohmann::json order;
        order["CommodityId"] = commodityId;
        order["Lots"] = 0.1;              // 下單手數（可根據策略調整）
        order["OrderType"] = "BaseOrder"; // 表示開單
        order["LongShort"] = 1;           // 1 表多頭
        order["NewOrClosedPosition"] = 1; // 新單
        std::string orderJson = order.dump();

        std::cout << "[CheckAndPlaceLongOrder] Generated Long Order Signal: " << orderJson << std::endl;
        DEBUG(DEBUG_LEVEL_DEBUG, "[CheckAndPlaceLongOrder] Generated Long Order Signal: %s\n", orderJson.c_str());

        // 呼叫 SendOrderJsonSignal 發送下單訊號
        SendOrderJsonSignal(orderJson);
    }
    else
    {
        std::cout << "[CheckAndPlaceLongOrder] Conditions not met, no order generated." << std::endl;
        DEBUG(DEBUG_LEVEL_DEBUG, "[CheckAndPlaceLongOrder] Conditions not met, no order generated.\n");
    }
}
