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
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread> // For std::this_thread::sleep_for
#include <unordered_map>
#include <yaml-cpp/yaml.h>

#include "Strategy.h"
#include "StrategyCfd.h"
#include "config.h"
#include <GengYouFuturesUI.h>

#include <nlohmann/json.hpp>
#include <sqlite3.h>
#include <string>

extern std::deque<long> gDaysKlineDiff;
extern std::unordered_map<long, std::array<long, 4>> gCurCommHighLowPoint;
extern SHORT gCurServerTime[3];
extern std::unordered_map<long, long> gCurCommPrice;
extern std::unordered_map<SHORT, std::array<long, 6>> gCurTaiexInfo;
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

// 全局数据库指针
sqlite3 *db = nullptr;

bool InsertCacheRecord(const std::string &timestamp, const std::string &commodityId, long CurLongShort, double CurBidOfferLongShortSlope);
nlohmann::json QueryCacheData(const std::string &commodityId);
void SaveCacheForOrderMachine(const std::string &commodityId, long CurLongShort, double CurBidOfferLongShortSlope);

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

    count = 0;

    while (pSKOsQuoteLib->IsConnected() != 1)
    {
        g_nCode = pSKOsQuoteLib->EnterMonitorLONG();
        pSKCenterLib->PrintfCodeMessage("Quote", "EnterMonitor", g_nCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(3000)); //  CPU
        ++count;

        if (count == 5)
        {
            DEBUG(DEBUG_LEVEL_ERROR, "pSKOsQuoteLib->IsConnected() != 1");
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

    // pSKCenterLib->PrintfCodeMessage("AutoGetFutureRights", "GetFutureRights", g_nCode);

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

void AutoOsQuoteTicks(IN string ProductNum, short sPageNo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

    g_nCode = pSKOsQuoteLib->RequestTicks(&sPageNo, ProductNum);

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

VOID CopyDataToTheOrderMachine(LONG MtxCommodtyInfo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    gMarketDataUI.Updating = TRUE;

    // user account

    gUserAccountUI.g_strUserId = g_strUserId;

    // user strategy config

    gStrategyConfigUI.ClosingKeyPriceLevel = gStrategyConfig.ClosingKeyPriceLevel;
    gStrategyConfigUI.BidOfferLongShortThreshold = gStrategyConfig.BidOfferLongShortThreshold;
    gStrategyConfigUI.BidOfferLongShortExtremeValue = gStrategyConfig.BidOfferLongShortExtremeValue;
    gStrategyConfigUI.BidOfferLongShortAttackSlope = gStrategyConfig.BidOfferLongShortAttackSlope;
    gStrategyConfigUI.ActivePoint = gStrategyConfig.ActivePoint;
    gStrategyConfigUI.MaximumLoss = gStrategyConfig.MaximumLoss;
    gStrategyConfigUI.StrategyMode = gStrategyConfig.StrategyMode;
    gStrategyConfigUI.SpecifyLongShort = gStrategyConfig.SpecifyLongShort;

    // default market config

    gMarketDataUI.MtxPrices = gCurCommPrice[MtxCommodtyInfo] / 100;
    gMarketDataUI.Diff = (gCurCommPrice[MtxCommodtyInfo] - gCurCommPrice[gCommodtyInfo.TSEAIdxNo]) / 100;
    gMarketDataUI.gCurServerTime[0] = gCurServerTime[0];
    gMarketDataUI.gCurServerTime[1] = gCurServerTime[1];
    gMarketDataUI.gCurServerTime[2] = gCurServerTime[2];

    if (gCurCommHighLowPoint.count(MtxCommodtyInfo) > 0)
    {
        long CurHigh = gCurCommHighLowPoint[MtxCommodtyInfo][0] / 100;
        long CurLow = gCurCommHighLowPoint[MtxCommodtyInfo][1] / 100;
        long CostMovingAverage = static_cast<long>(gCostMovingAverageVal);
        long OpenPrice = gCurCommHighLowPoint[MtxCommodtyInfo][2] / 100;
        double ShockLongExtremeValue = gCostMovingAverageVal - EstimatedTodaysAmplitude() / 2;
        double ShockShortExtremeValue = gCostMovingAverageVal + EstimatedTodaysAmplitude() / 2;

        gMarketDataUI.OpenPrice = OpenPrice;
        gMarketDataUI.CurHigh = CurHigh;
        gMarketDataUI.CurLow = CurLow;
        gMarketDataUI.CostMovingAverage = CostMovingAverage;
        gMarketDataUI.CurAmp = CurHigh - CurLow;
        gMarketDataUI.CurAvg = (CurHigh + CurLow) / 2;
        gMarketDataUI.ShockLongExtremeValue = static_cast<long>(ShockLongExtremeValue);
        gMarketDataUI.ShockShortExtremeValue = static_cast<long>(ShockShortExtremeValue);
    }

    {
        gMarketDataUI.gDayAmpAndKeyPrice.LongKey5 = gDayAmpAndKeyPrice.LongKey5;
        gMarketDataUI.gDayAmpAndKeyPrice.LongKey4 = gDayAmpAndKeyPrice.LongKey4;
        gMarketDataUI.gDayAmpAndKeyPrice.LongKey3 = gDayAmpAndKeyPrice.LongKey3;
        gMarketDataUI.gDayAmpAndKeyPrice.LongKey2 = gDayAmpAndKeyPrice.LongKey2;
        gMarketDataUI.gDayAmpAndKeyPrice.LongKey1 = gDayAmpAndKeyPrice.LongKey1;

        gMarketDataUI.gDayAmpAndKeyPrice.ShortKey1 = gDayAmpAndKeyPrice.ShortKey1;
        gMarketDataUI.gDayAmpAndKeyPrice.ShortKey2 = gDayAmpAndKeyPrice.ShortKey2;
        gMarketDataUI.gDayAmpAndKeyPrice.ShortKey3 = gDayAmpAndKeyPrice.ShortKey3;
        gMarketDataUI.gDayAmpAndKeyPrice.ShortKey4 = gDayAmpAndKeyPrice.ShortKey4;
        gMarketDataUI.gDayAmpAndKeyPrice.ShortKey5 = gDayAmpAndKeyPrice.ShortKey5;

        gMarketDataUI.gDayAmpAndKeyPrice.SmallestAmp = gDayAmpAndKeyPrice.SmallestAmp;
        gMarketDataUI.gDayAmpAndKeyPrice.SmallAmp = gDayAmpAndKeyPrice.SmallAmp;
        gMarketDataUI.gDayAmpAndKeyPrice.AvgAmp = gDayAmpAndKeyPrice.AvgAmp;
        gMarketDataUI.gDayAmpAndKeyPrice.LargerAmp = gDayAmpAndKeyPrice.LargerAmp;
        gMarketDataUI.gDayAmpAndKeyPrice.LargestAmp = gDayAmpAndKeyPrice.LargestAmp;

        gMarketDataUI.gNumberOfStocksRisingAndFalling = gNumberOfStocksRisingAndFalling;
        gMarketDataUI.gOsTransactionListLongShort = gOsTransactionListLongShort;
        gMarketDataUI.gTransactionListLongShort = gTransactionListLongShort;
        gMarketDataUI.gBidOfferLongShort = gBidOfferLongShort;
    }

    gMarketDataUI.gLongShort = gLongShort;
    gMarketDataUI.gBidOfferLongShortSlope = gBidOfferLongShortSlope;

    gMarketDataUI.gClosedProfitLoss = gClosedProfitLoss;
    gMarketDataUI.gFutureRight = gFutureRight;

    // open interest info

    gOpenInterestInfoUI.openPosition = gOpenInterestInfo.openPosition;
    gOpenInterestInfoUI.avgCost = gOpenInterestInfo.avgCost;
    gOpenInterestInfoUI.profitAndLoss = gOpenInterestInfo.profitAndLoss;
}
void LoadLongShort(const std::string &commodityId, long &CurLongShort, double &CurBidOfferLongShortSlope)
{
    try
    {
        // 查询指定商品的缓存数据，返回一个 nlohmann::json 数组
        nlohmann::json temp = QueryCacheData(commodityId);

        // 检查返回结果是否为空
        if (!temp.empty())
        {
            // 如果需要确保按时间排序，可以在此处排序（这里假设数据已按时间排序，最新记录在最后）
            nlohmann::json lastRecord = temp.back();

            // 使用 at() 方法获取对应字段的值，若字段不存在则抛出异常
            CurLongShort = lastRecord.at("CurLongShort").get<long>();
            CurBidOfferLongShortSlope = lastRecord.at("CurBidOfferLongShortSlope").get<double>();

            DEBUG(DEBUG_LEVEL_INFO, "COMMODITY: %s, CurLongShort: %ld, CurBidOfferLongShortSlope: %f\n",
                  commodityId.c_str(), CurLongShort, CurBidOfferLongShortSlope);
        }
        else
        {
            DEBUG(DEBUG_LEVEL_INFO, "No cache data found for commodity: %s\n", commodityId.c_str());
        }
    }
    catch (const std::exception &ex)
    {
        DEBUG(DEBUG_LEVEL_ERROR, "Error retrieving market data for %s: %s\n", commodityId.c_str(), ex.what());
    }
}

// 检查文件是否存在的函数
bool FileExists(const std::string &filename)
{
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

// 获取当前时间戳（ISO 8601 格式: YYYY-MM-DD HH:MM:SS）
std::string GetCurrentTimestamp()
{
    std::time_t now = std::time(nullptr);
    std::tm localTime;
    localtime_s(&localTime, &now);
    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// 计算截止时间（当前时间减 7 天），格式为 ISO 8601
std::string GetCutoffTimestamp()
{
    // 使用 chrono 计算7天之前的时间点
    auto now = std::chrono::system_clock::now();
    auto cutoff_time = now - std::chrono::hours(24 * 7);
    std::time_t cutoff_tt = std::chrono::system_clock::to_time_t(cutoff_time);
    std::tm cutoffLocal;
    localtime_s(&cutoffLocal, &cutoff_tt);
    std::ostringstream oss;
    oss << std::put_time(&cutoffLocal, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// 初始化数据库并创建表格（如果不存在），若数据库文件存在则沿用已有数据
bool InitializeDatabase(const std::string &dbPath)
{
    bool dbExists = FileExists(dbPath);

    // 打开数据库，启用全互斥保证线程安全
    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX;
    if (sqlite3_open_v2(dbPath.c_str(), &db, flags, nullptr) != SQLITE_OK)
    {
        std::cerr << "无法打开数据库: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // 如果数据库文件不存在，则创建表格；若已存在，则不重建表格（保留原有数据）
    if (!dbExists)
    {
        const char *createTableSQL =
            "CREATE TABLE IF NOT EXISTS cache_data ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "timestamp TEXT, "
            "CommodityId TEXT, "
            "CurLongShort INTEGER, "
            "CurBidOfferLongShortSlope REAL"
            ");";
        char *errMsg = nullptr;
        if (sqlite3_exec(db, createTableSQL, 0, 0, &errMsg) != SQLITE_OK)
        {
            std::cerr << "创建表失败: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }
    }
    return true;
}

// 插入记录到数据库
bool InsertCacheRecord(const std::string &timestamp, const std::string &commodityId, long CurLongShort, double CurBidOfferLongShortSlope)
{
    const char *insertSQL = "INSERT INTO cache_data (timestamp, CommodityId, CurLongShort, CurBidOfferLongShortSlope) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "准备语句失败: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    sqlite3_bind_text(stmt, 1, timestamp.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, commodityId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 3, CurLongShort);
    sqlite3_bind_double(stmt, 4, CurBidOfferLongShortSlope);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        std::cerr << "执行语句失败: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_finalize(stmt);
    return true;
}

// 删除超过7天的记录
void DeleteOldRecords()
{
    std::string cutoff = GetCutoffTimestamp();
    std::string deleteSQL = "DELETE FROM cache_data WHERE timestamp < ?";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, deleteSQL.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "删除语句准备失败: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
    sqlite3_bind_text(stmt, 1, cutoff.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        std::cerr << "执行删除语句失败: " << sqlite3_errmsg(db) << std::endl;
    }
    else
    {
        std::cout << "已删除早于 " << cutoff << " 的记录" << std::endl;
    }
    sqlite3_finalize(stmt);
}

// 保存缓存数据（使用 SQLite），并确保不会覆盖之前的数据
void SaveCacheForOrderMachine(const std::string &commodityId, long CurLongShort, double CurBidOfferLongShortSlope)
{
    static int syncCounter = 0;
    const int syncThreshold = 12; // 每分钟同步12次（每5秒一次）

    // 获取当前时间戳（ISO 8601 格式）
    std::string timestamp = GetCurrentTimestamp();

    // 插入记录到数据库
    if (!InsertCacheRecord(timestamp, commodityId, CurLongShort, CurBidOfferLongShortSlope))
    {
        std::cerr << "插入记录失败" << std::endl;
    }

    // 每隔一定次数（例如每分钟）清理超过7天的记录
    if (++syncCounter >= syncThreshold)
    {
        syncCounter = 0;
        DeleteOldRecords();
    }
}

// 查询指定商品的快取资料并返回 JSON 格式
nlohmann::json QueryCacheData(const std::string &commodityId)
{
    const char *querySQL = "SELECT timestamp, CommodityId, CurLongShort, CurBidOfferLongShortSlope FROM cache_data WHERE CommodityId = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, querySQL, -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "查询语句准备失败: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }
    sqlite3_bind_text(stmt, 1, commodityId.c_str(), -1, SQLITE_TRANSIENT);

    nlohmann::json result = nlohmann::json::array();
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        std::string timestamp = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
        std::string queriedCommodityId = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        long CurLongShort = sqlite3_column_int64(stmt, 2);
        double CurBidOfferLongShortSlope = sqlite3_column_double(stmt, 3);

        result.push_back({{"timestamp", timestamp},
                          {"commodityId", queriedCommodityId},
                          {"CurLongShort", CurLongShort},
                          {"CurBidOfferLongShortSlope", CurBidOfferLongShortSlope}});
    }
    sqlite3_finalize(stmt);

    std::cout << "Query Result: " << result.dump(4) << std::endl;
    return result;
}

void thread_main()
{
    const int refreshInterval = 1000; // 1000 ms
    std::chrono::steady_clock::time_point lastClearTime = std::chrono::steady_clock::now();
    AutoLogIn();

    AutoConnect();

    AutoGetFutureRights();

    long res = pSKQuoteLib->RequestServerTime();

    DEBUG(DEBUG_LEVEL_INFO, "pSKQuoteLib->RequestServerTime()=%d", res);

    res = pSKQuoteLib->GetMarketBuySellUpDown();
    DEBUG(DEBUG_LEVEL_INFO, "pSKQuoteLib->GetMarketBuySellUpDown()=%d", res);

    gLeadingCommodtyInfo.push_back({"2382", -1});
    gLeadingCommodtyInfo.push_back({"3661", -1});
    gLeadingCommodtyInfo.push_back({"3443", -1});

    pSKQuoteLib->GetCommodityIdx();

    pSKOsQuoteLib->GetCommodityIdx();

    std::string CommList;

    std::ostringstream oss;
    oss << COMMODITY_TX_MAIN << "AM" << "," << COMMODITY_TX_MAIN << "," << "TSEA" << "," << TSMC << "," << MEDIATEK << "," << FOXCONN;
    CommList = oss.str();

    AutoQuote(CommList, -1);

    AutoQuoteTicks(TSMC, -1);
    AutoQuoteTicks(MEDIATEK, -1);
    AutoQuoteTicks(FOXCONN, -1);

    for (int i = 0; i < gLeadingCommodtyInfo.size(); ++i)
    {
        AutoQuoteTicks(gLeadingCommodtyInfo[i].first, -1);
    }

    // For calculate 5MA
    AutoQuoteTicks(COMMODITY_TX_MAIN, -1);
    AutoOsQuoteTicks(COMMODITY_OS_MAIN, -1);
    AutoOsQuoteTicks(COMMODITY_OS_GC, -1);
    AutoOsQuoteTicks(COMMODITY_OS_DX, -1);

    gMarketDataUI.Updating = FALSE;

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));

        system("cls");

        printf("Waiting for host quotation...");

        if (gCurServerTime[0] >= 0 &&
            gCommodtyInfo.MTXIdxNoAM >= 0 &&
            gCommodtyInfo.MTXIdxNo >= 0 &&
            gCommodtyOsInfo.NQIdxNo >= 0 &&
            gCommodtyOsInfo.GCIdxNo >= 0 &&
            gCommodtyOsInfo.DXIdxNo >= 0 &&
            gCurCommHighLowPoint.count(gCommodtyInfo.MTXIdxNoAM) != 0 &&
            gCurCommHighLowPoint.count(gCommodtyInfo.MTXIdxNo) != 0)
        {
            break;
        }
    }

    DEBUG(DEBUG_LEVEL_INFO, "[ServerTime: %d: %d: %d]", gCurServerTime[0], gCurServerTime[1], gCurServerTime[2]);
    LOG(DEBUG_LEVEL_INFO, "[ServerTime: %d: %d: %d]", gCurServerTime[0], gCurServerTime[1], gCurServerTime[2]);

    LoadLongShort(COMMODITY_MAIN, gLongShort, gBidOfferLongShortSlope);
    LoadLongShort(COMMODITY_NAS_MAIN, gCfdTransactionListLongShort[gCommodtyOsInfo.NQIdxNo], gCfdTransactionListLongShortSlope[gCommodtyOsInfo.NQIdxNo]);
    LoadLongShort(COMMODITY_GC_MAIN, gCfdTransactionListLongShort[gCommodtyOsInfo.GCIdxNo], gCfdTransactionListLongShortSlope[gCommodtyOsInfo.GCIdxNo]);
    LoadLongShort(COMMODITY_DX_MAIN, gCfdTransactionListLongShort[gCommodtyOsInfo.DXIdxNo], gCfdTransactionListLongShortSlope[gCommodtyOsInfo.DXIdxNo]);

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
        CfdStrategySwitch();
        gOsTransactionListLongShort = gCfdTransactionListLongShort[gCommodtyOsInfo.NQIdxNo];

        // Ouput start

        if (elapsed.count() >= refreshInterval)
        {
            // GengYouFuturesUI start
            {
                CopyDataToTheOrderMachine(MtxCommodtyInfo);

                SaveCacheForOrderMachine(COMMODITY_MAIN, gLongShort, gBidOfferLongShortSlope);
                SaveCacheForOrderMachine(COMMODITY_NAS_MAIN, gCfdTransactionListLongShort[gCommodtyOsInfo.NQIdxNo], gCfdTransactionListLongShortSlope[gCommodtyOsInfo.NQIdxNo]);
                SaveCacheForOrderMachine(COMMODITY_GC_MAIN, gCfdTransactionListLongShort[gCommodtyOsInfo.GCIdxNo], gCfdTransactionListLongShortSlope[gCommodtyOsInfo.GCIdxNo]);
                SaveCacheForOrderMachine(COMMODITY_DX_MAIN, gCfdTransactionListLongShort[gCommodtyOsInfo.DXIdxNo], gCfdTransactionListLongShortSlope[gCommodtyOsInfo.DXIdxNo]);
            }

            system("cls");
            lastClearTime = now;

            ++CheckConnected;

            if (CheckConnected == 30)
            {
                AutoGetFutureRights();

                if (pSKQuoteLib->IsConnected() != 1 || pSKOsQuoteLib->IsConnected() != 1)
                {
                    DEBUG(DEBUG_LEVEL_ERROR, "pSKQuoteLib->IsConnected() != 1");
                    LOG(DEBUG_LEVEL_INFO, "pSKQuoteLib->IsConnected() != 1");
                    release();
                    exit(0);
                }

                CheckConnected = 0;
            }

            printf("[UserId:%s], [StrategyMode:%ld], [SpecifyLongShort:%d], [ClosingKeyPriceLevel:%ld], [BidOfferLongShortThreshold:%ld], [BidOfferLongShortExtremeValue:%ld], [BidOfferLongShortAttackSlope:%f], [ActivePoint:%ld], [MaximumLoss:%f]\n",
                   g_strUserId.c_str(), gStrategyConfig.StrategyMode, gStrategyConfig.SpecifyLongShort, gStrategyConfig.ClosingKeyPriceLevel,
                   gStrategyConfig.BidOfferLongShortThreshold, gStrategyConfig.BidOfferLongShortExtremeValue, gStrategyConfig.BidOfferLongShortAttackSlope, gStrategyConfig.ActivePoint, gStrategyConfig.MaximumLoss);
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

            printf("EvaluatePosition: %ld, FutureRight: %f, ClosedProfitLoss: %f", gEvaluatePosition, gFutureRight, gClosedProfitLoss);

            if (gOpenInterestInfo.openPosition != 0)
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

            printf("BidOfferLongShortSlope: %f, LongShort: %ld, BidOfferLongShort: %ld, TransactionListLongShort: %ld, OsTransactionListLongShort: %ld, NumberOfStocksRisingAndFalling: %f\n",
                   gBidOfferLongShortSlope, gLongShort, gBidOfferLongShort, gTransactionListLongShort, gOsTransactionListLongShort, gNumberOfStocksRisingAndFalling);

            printf("=========================================\n");

            AutoBest5Long(gCommodtyInfo.TSMCIdxNo, TSMC);
            AutoBest5Long(gCommodtyInfo.FOXCONNIdxNo, FOXCONN);
            AutoBest5Long(gCommodtyInfo.MediaTekIdxNo, MEDIATEK);

            printf("=========================================\n");

            printf("TSEA Total OFFER: [%ld]\n", gCurTaiexInfo[0x00][3]);
            printf("            BID : [%ld]\n", gCurTaiexInfo[0x00][2]);
            printf("TPEX Total OFFER: [%ld]\n", gCurTaiexInfo[0x01][3]);
            printf("            BID : [%ld]\n", gCurTaiexInfo[0x01][2]);

            printf("=========================================\n");

            printf("gCfdTransactionListLongShort[gCommodtyOsInfo.GCIdxNo]: %ld, gCfdTransactionListLongShort[gCommodtyOsInfo.NQIdxNo]: %ld, gCfdTransactionListLongShort[gCommodtyOsInfo.DXIdxNo]: %ld,\n",
                   gCfdTransactionListLongShort[gCommodtyOsInfo.GCIdxNo], gCfdTransactionListLongShort[gCommodtyOsInfo.NQIdxNo], gCfdTransactionListLongShort[gCommodtyOsInfo.DXIdxNo]);

            printf("=========================================\n");

            printf("gCfdTransactionListLongShortSlope[gCommodtyOsInfo.GCIdxNo]: %f, gCfdTransactionListLongShortSlope[gCommodtyOsInfo.NQIdxNo]: %f, gCfdTransactionListLongShortSlope[gCommodtyOsInfo.DXIdxNo]: %f,\n",
                   gCfdTransactionListLongShortSlope[gCommodtyOsInfo.GCIdxNo], gCfdTransactionListLongShortSlope[gCommodtyOsInfo.NQIdxNo], gCfdTransactionListLongShortSlope[gCommodtyOsInfo.DXIdxNo]);

            printf("=========================================\n");
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

        if (config["BID_OFFER_LONG_SHORT_EXTREME_VALUE"])
        {
            gStrategyConfig.BidOfferLongShortExtremeValue = config["BID_OFFER_LONG_SHORT_EXTREME_VALUE"].as<LONG>();
        }

        if (config["BID_OFFER_LONG_SHORT_ATTACK_SLOPE"])
        {
            gStrategyConfig.BidOfferLongShortAttackSlope = config["BID_OFFER_LONG_SHORT_ATTACK_SLOPE"].as<DOUBLE>();
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

        if (config["SPECIFY_LONG_SHORT"])
        {
            gStrategyConfig.SpecifyLongShort = config["SPECIFY_LONG_SHORT"].as<SHORT>();
        }

        DEBUG(DEBUG_LEVEL_INFO, "Closing Key Price Level: %ld", gStrategyConfig.ClosingKeyPriceLevel);
        DEBUG(DEBUG_LEVEL_INFO, "Bid Offer Long Short Threshold: %ld", gStrategyConfig.BidOfferLongShortThreshold);
        DEBUG(DEBUG_LEVEL_INFO, "Bid Offer Long Short Extreme Value: %ld", gStrategyConfig.BidOfferLongShortExtremeValue);
        DEBUG(DEBUG_LEVEL_INFO, "Bid Offer Long Short Attack Slop: %f", gStrategyConfig.BidOfferLongShortAttackSlope);
        DEBUG(DEBUG_LEVEL_INFO, "Activity Point: %ld", gStrategyConfig.ActivePoint);
        DEBUG(DEBUG_LEVEL_INFO, "Maximum Loss: %f", gStrategyConfig.MaximumLoss);
        DEBUG(DEBUG_LEVEL_INFO, "STRATEGY_MODE: %ld", gStrategyConfig.StrategyMode);
        DEBUG(DEBUG_LEVEL_INFO, "SPECIFY_LONG_SHORT: %d", gStrategyConfig.SpecifyLongShort);
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

    std::string dbPath = "cache_data.db";
    if (!InitializeDatabase(dbPath))
    {
        return -1;
    }

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
    {
        tMain.detach();
    }

    std::atomic<bool> isRunning(true);
    std::thread serverThread(startHttpServer, std::ref(isRunning));

    if (isRunning.load())
    {
        DEBUG(DEBUG_LEVEL_INFO, "serverThread running");
    }

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) // Get SendMessage loop
    {
        DispatchMessageW(&msg);
    }

    serverThread.join();

    sqlite3_close(db);

    DEBUG(DEBUG_LEVEL_INFO, "serverThread exit");

    DEBUG(DEBUG_LEVEL_DEBUG, "end");

    return 0;
}
