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
Logger logger("debug.log");

CSKCenterLib *pSKCenterLib;
CSKQuoteLib *pSKQuoteLib;
CSKReplyLib *pSKReplyLib;
CSKOrderLib *pSKOrderLib;

long g_nCode = 0;
string g_strUserId;

void AutoConnect()
{
    while (pSKQuoteLib->IsConnected() != 1)
    {
        g_nCode = pSKQuoteLib->EnterMonitorLONG();
        pSKCenterLib->PrintfCodeMessage("Quote", "EnterMonitor", g_nCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(3000)); // 短暂休眠，避免过度占用 CPU
    }
}

void AutoLogIn()
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

    // 初始化
    g_nCode = pSKOrderLib->Initialize();
    pSKCenterLib->PrintfCodeMessage("AutoLogIn", "Initialize", g_nCode);

    // 讀取憑證
    g_nCode = pSKOrderLib->ReadCertByID(g_strUserId);
    pSKCenterLib->PrintfCodeMessage("AutoLogIn", "ReadCertByID", g_nCode);

    // 取得帳號
    g_nCode = pSKOrderLib->GetUserAccount();
    pSKCenterLib->PrintfCodeMessage("AutoLogIn", "GetUserAccount", g_nCode);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

/**
 * @brief
 *
 *  struct FUTUREORDER
 * {
 *  BSTR bstrFullAccount; // 期貨帳號，分公司代碼＋帳號7碼
 *  BSTR bstrStockNo;     // 委託期權代號
 *  SHORT sTradeType;     // 0:ROD  1:IOC  2:FOK
 *  SHORT sBuySell;       // 0:買進 1:賣出
 *  SHORT sDayTrade;      // 當沖0:否 1:是，可當沖商品請參考交易所規定。
 *  SHORT sNewClose;      // 新平倉，0:新倉 1:平倉 2:自動{新期貨、選擇權使用}
 *  BSTR bstrPrice;       // 委託價格(IOC and FOK，可用「M」表示市價，「P」表示範圍市價)　　　　　　　　　　　　
 *  LONG nQty;            // 交易口數
 *  SHORT sReserved;      //{期貨委託SendFutureOrderCLR適用}盤別，0:盤中(T盤及T+1盤)；1:T盤預約
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

void AutoOrderMTX()
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

    g_nCode = pSKOrderLib->SendFutureOrder(g_strUserId,
                                           false,
                                           "MTX00",
                                           1, // IOC
                                           1, // sell
                                           0, // DayTrade
                                           2, // NewClose
                                           "P",
                                           1,
                                           0);
    pSKCenterLib->PrintfCodeMessage("AutoOrderMTX", "SendFutureOrder", g_nCode);

    g_nCode = pSKOrderLib->SendFutureOrder(g_strUserId,
                                           false,
                                           "MTX00",
                                           1, // IOC
                                           1, // sell
                                           0, // DayTrade
                                           2, // NewClose
                                           "P",
                                           1,
                                           0);
    pSKCenterLib->PrintfCodeMessage("AutoOrderMTX", "SendFutureOrder", g_nCode);

    DEBUG(DEBUG_LEVEL_DEBUG, "SendFutureOrder res = %d", g_nCode);

    g_nCode = pSKOrderLib->SendFutureOrder(g_strUserId,
                                           false,
                                           "MTX00",
                                           1, // IOC
                                           0, // buy
                                           0, // DayTrade
                                           2, // NewClose
                                           "P",
                                           1,
                                           0);

    pSKCenterLib->PrintfCodeMessage("AutoOrderMTX", "SendFutureOrder", g_nCode);

    DEBUG(DEBUG_LEVEL_DEBUG, "SendFutureOrder res = %d", g_nCode);

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

void AutoQuote(IN string ProductNum, short sPageNo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

    g_nCode = pSKQuoteLib->RequestStocks(&sPageNo, ProductNum);
    pSKCenterLib->PrintfCodeMessage("Quote", "RequestStocks", g_nCode);
    DEBUG(DEBUG_LEVEL_DEBUG, "g_nCode= %d", g_nCode);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

void AutoQuoteTicks(IN string ProductNum, short sPageNo)
{
    DEBUG(DEBUG_LEVEL_INFO, "Started");

    g_nCode = pSKQuoteLib->RequestTicks(&sPageNo, ProductNum);

    pSKCenterLib->PrintfCodeMessage("Quote", "RequestTicks", g_nCode);

    DEBUG(DEBUG_LEVEL_DEBUG, "g_nCode= %d", g_nCode);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

void AutoKLineData(IN string ProductNum)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

    g_nCode = pSKQuoteLib->RequestKLine(ProductNum);

    DEBUG(DEBUG_LEVEL_DEBUG, "g_nCode=%ld", g_nCode);

    pSKCenterLib->PrintfCodeMessage("Quote", "RequestKLine", g_nCode);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
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

extern std::deque<long> gDaysKlineDiff;
extern bool gEatOffer;
extern std::unordered_map<long, std::array<long, 2>> gCurCommHighLowPoint;
extern SHORT gCurServerTime[3];
extern std::unordered_map<long, long> gCurCommPrice;
extern std::unordered_map<SHORT, std::array<long, 4>> gCurTaiexInfo;
extern std::unordered_map<long, vector<pair<long, long>>> gBest5BidOffer;

// To do list:
// 日夜盤都要算振福關卡價 (done)
// Estimated trading volume
// need VIX index
// current time (done)
// Estimated trading volume
// Instant profit and loss
// Add open position query.
// Add stop loss and profit stop mechanism

// Bug:
// The price will be unstable at the beginning and will change from high to low.

// To do list:
// 日夜盤都要算振福關卡價
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

    AutoKLineData("TX00");

    pSKQuoteLib->ProcessDaysOrNightCommHighLowPoint();

    long long accu = 0;
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

    long res = pSKQuoteLib->RequestServerTime();

    DEBUG(DEBUG_LEVEL_INFO, "pSKQuoteLib->RequestServerTime()=%d", res);

    res = pSKQuoteLib->GetMarketBuySellUpDown();
    DEBUG(DEBUG_LEVEL_INFO, "pSKQuoteLib->GetMarketBuySellUpDown()=%d", res);

    SKCOMLib::SKSTOCKLONG skStock;

    res = pSKQuoteLib->RequestStockIndexMap("MTX00", &skStock);

    DEBUG(DEBUG_LEVEL_INFO, "pSKQuoteLib->RequestStockIndexMap()=%d", res);

    long MTXIdxNo = skStock.nStockIdx;

    res = pSKQuoteLib->RequestStockIndexMap("2330", &skStock);

    DEBUG(DEBUG_LEVEL_INFO, "pSKQuoteLib->RequestStockIndexMap()=%d", res);

    long TSMCIdxNo = skStock.nStockIdx;

    res = pSKQuoteLib->RequestStockIndexMap("2317", &skStock);

    DEBUG(DEBUG_LEVEL_INFO, "pSKQuoteLib->RequestStockIndexMap()=%d", res);

    res = pSKQuoteLib->RequestStockIndexMap("2454", &skStock);

    DEBUG(DEBUG_LEVEL_INFO, "pSKQuoteLib->RequestStockIndexMap()=%d", res);

    // 设置定期清屏的时间间隔（以毫秒为单位）
    const int refreshInterval = 1000; // 1000毫秒
    auto lastClearTime = std::chrono::steady_clock::now();

    AutoQuoteTicks("2330", 1);

    AutoQuoteTicks("MTX00", 2);

    while (true)
    {
        // 获取当前时间
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClearTime);

        // 检查是否需要清屏
        if (elapsed.count() >= refreshInterval)
        {
            // 清屏
            system("cls");

            // 更新最后清屏时间
            lastClearTime = now;

            printf("CurMtxPrice: %ld    ", gCurCommPrice[MTXIdxNo]);
            printf("ServerTime: %d: %d: %d\n", gCurServerTime[0], gCurServerTime[1], gCurServerTime[2]);
            printf("Time: %ld: Valume: %ld: Buy: %ld Sell: %ld\n",
                   gCurTaiexInfo[0][0], gCurTaiexInfo[0][1], gCurTaiexInfo[0][2], gCurTaiexInfo[0][3]);

            printf("=========================================\n");

            if (gCurCommHighLowPoint.count(MTXIdxNo) > 0)
            {
                long CurHigh = gCurCommHighLowPoint[MTXIdxNo][0] / 100;
                long CurLow = gCurCommHighLowPoint[MTXIdxNo][1] / 100;

                DEBUG(DEBUG_LEVEL_DEBUG, "MTXIdxNo: %ld. High: %ld, Low: %ld", MTXIdxNo, CurHigh, CurLow);

                printf("CurHigh: %ld, CurLow: %ld\n\n", CurHigh, CurLow);

                printf("=========================================\n");

                printf("Long Key 5: %ld\n", CurLow + LargestAmp);
                printf("Long Key 4: %ld\n", CurLow + LargerAmp);
                printf("Long Key 3: %ld\n", CurLow + AvgAmp);
                printf("Long Key 2: %ld\n", CurLow + SmallAmp);
                printf("Long Key 1: %ld\n", CurLow + SmallestAmp);
                printf("=========================================\n");
                printf("Short Key 1: %ld\n", CurHigh - SmallestAmp);
                printf("Short Key 2: %ld\n", CurHigh - SmallAmp);
                printf("Short Key 3: %ld\n", CurHigh - AvgAmp);
                printf("Short Key 4: %ld\n", CurHigh - LargerAmp);
                printf("Short Key 5: %ld\n", CurHigh - LargestAmp);

                printf("\n");
                printf("CurAmp : %d\n", CurHigh - CurLow);

                printf("=========================================\n");
            }

            printf("SmallestAmp : %ld\n", SmallestAmp);
            printf("SmallAmp : %ld\n", SmallAmp);
            printf("AvgAmp : %ld\n", AvgAmp);
            printf("LargerAmp : %ld\n", LargerAmp);
            printf("LargestAmp : %ld\n", LargestAmp);

            printf("=========================================\n");

            if (gCurCommHighLowPoint.count(TSMCIdxNo) > 0)
            {
                long CurHigh = gCurCommHighLowPoint[TSMCIdxNo][0] / 100;
                long CurLow = gCurCommHighLowPoint[TSMCIdxNo][1] / 100;

                DEBUG(DEBUG_LEVEL_DEBUG, "TSMCIdxNo: %ld. High: %ld, Low: %ld", TSMCIdxNo, CurHigh, CurLow);

                printf("TSMCIdxNo : CurHigh: %ld, CurLow: %ld\n\n", CurHigh, CurLow);
            }

            if (gBest5BidOffer[TSMCIdxNo].size() >= 10)
            {
                long curPrice = gCurCommPrice[TSMCIdxNo];

                printf("TSMC: %ld    ", gCurCommPrice[MTXIdxNo]);

                long TotalBid = gBest5BidOffer[TSMCIdxNo][0].second +
                                gBest5BidOffer[TSMCIdxNo][1].second +
                                gBest5BidOffer[TSMCIdxNo][2].second +
                                gBest5BidOffer[TSMCIdxNo][3].second +
                                gBest5BidOffer[TSMCIdxNo][4].second;
                long TotalOffer = gBest5BidOffer[TSMCIdxNo][9].second +
                                  gBest5BidOffer[TSMCIdxNo][8].second +
                                  gBest5BidOffer[TSMCIdxNo][7].second +
                                  gBest5BidOffer[TSMCIdxNo][6].second +
                                  gBest5BidOffer[TSMCIdxNo][5].second;

                printf("Total Offer: [%ld]\n", TotalOffer);

                printf("Ask5: [%ld]: [%ld]", gBest5BidOffer[TSMCIdxNo][9].first, gBest5BidOffer[TSMCIdxNo][9].second);
                if (curPrice == gBest5BidOffer[TSMCIdxNo][9].first)
                {
                    printf("*\n");
                }
                printf("Ask4: [%ld]: [%ld]", gBest5BidOffer[TSMCIdxNo][8].first, gBest5BidOffer[TSMCIdxNo][8].second);
                if (curPrice == gBest5BidOffer[TSMCIdxNo][8].first)
                {
                    printf("*\n");
                }
                printf("Ask3: [%ld]: [%ld]", gBest5BidOffer[TSMCIdxNo][7].first, gBest5BidOffer[TSMCIdxNo][7].second);
                if (curPrice == gBest5BidOffer[TSMCIdxNo][7].first)
                {
                    printf("*\n");
                }
                printf("Ask2: [%ld]: [%ld]", gBest5BidOffer[TSMCIdxNo][6].first, gBest5BidOffer[TSMCIdxNo][6].second);
                if (curPrice == gBest5BidOffer[TSMCIdxNo][6].first)
                {
                    printf("*\n");
                }
                printf("Ask1: [%ld]: [%ld]", gBest5BidOffer[TSMCIdxNo][5].first, gBest5BidOffer[TSMCIdxNo][5].second);
                if (curPrice = gBest5BidOffer[TSMCIdxNo][5].first)
                {
                    printf("*\n");
                }
                printf("=========================================\n");
                printf("Bid1: [%ld]: [%ld]", gBest5BidOffer[TSMCIdxNo][0].first, gBest5BidOffer[TSMCIdxNo][0].second);
                if (curPrice == gBest5BidOffer[TSMCIdxNo][0].first)
                {
                    printf("*\n");
                }
                printf("Bid2: [%ld]: [%ld]", gBest5BidOffer[TSMCIdxNo][1].first, gBest5BidOffer[TSMCIdxNo][1].second);
                if (curPrice == gBest5BidOffer[TSMCIdxNo][1].first)
                {
                    printf("*\n");
                }
                printf("Bid3: [%ld]: [%ld]", gBest5BidOffer[TSMCIdxNo][2].first, gBest5BidOffer[TSMCIdxNo][2].second);
                if (curPrice == gBest5BidOffer[TSMCIdxNo][2].first)
                {
                    printf("*\n");
                }
                printf("Bid4: [%ld]: [%ld]", gBest5BidOffer[TSMCIdxNo][3].first, gBest5BidOffer[TSMCIdxNo][3].second);
                if (curPrice == gBest5BidOffer[TSMCIdxNo][3].first)
                {
                    printf("*\n");
                }
                printf("Bid5: [%ld]: [%ld]", gBest5BidOffer[TSMCIdxNo][4].first, gBest5BidOffer[TSMCIdxNo][4].second);
                if (curPrice == gBest5BidOffer[TSMCIdxNo][4].first)
                {
                    printf("*\n");
                }

                printf("Total Bid:   [%ld]\n", TotalBid);

                printf("=========================================\n");
            }
            else
            {
                DEBUG(DEBUG_LEVEL_INFO, "gBest5BidOffer[TSMCIdxNo].size() < 10");
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 短暂休眠，避免过度占用 CPU

        if (pSKQuoteLib->IsConnected() != 1)
        {
            DEBUG(DEBUG_LEVEL_ERROR, "pSKQuoteLib->IsConnected() != 1");

            AutoConnect();
            // release();
            // exit(0);
        }
    }
}

int main()
{

    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    CoInitialize(NULL);

    init();

    // printf("請輸入身分證字號：");
    // cin >> g_strUserId;

    g_strUserId = "F129305651";

    // printf("請輸入密碼：");
    string pwd;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
    pwd = "youlose1A!";
    // cout << endl;

    // cout << g_strUserId << " " << pwd << endl;

    g_nCode = pSKCenterLib->Login(g_strUserId.c_str(), pwd.c_str());

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
