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

extern std::deque<long> gDaysKlineDiff;
extern bool gEatOffer;
extern std::unordered_map<long, std::array<long, 3>> gCurCommHighLowPoint;
extern SHORT gCurServerTime[3];
extern std::unordered_map<long, long> gCurCommPrice;
extern std::unordered_map<SHORT, std::array<long, 4>> gCurTaiexInfo;
extern std::unordered_map<long, vector<pair<long, long>>> gBest5BidOffer;
extern COMMODITY_INFO gCommodtyInfo;
extern DAY_AMP_AND_KEY_PRICE gDayAmpAndKeyPrice;

// Define the global logger instance
Logger logger("debug.log");

CSKCenterLib *pSKCenterLib;
CSKQuoteLib *pSKQuoteLib;
CSKReplyLib *pSKReplyLib;
CSKOrderLib *pSKOrderLib;

long g_nCode = 0;
extern string g_strUserId;

void AutoConnect()
{
    while (pSKQuoteLib->IsConnected() != 1)
    {
        g_nCode = pSKQuoteLib->EnterMonitorLONG();
        pSKCenterLib->PrintfCodeMessage("Quote", "EnterMonitor", g_nCode);
        std::this_thread::sleep_for(std::chrono::milliseconds(3000)); //  CPU
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
    long curPrice = 0;

    if (gCurCommHighLowPoint.count(ProductIdxNo) > 0)
    {
        long CurHigh = gCurCommHighLowPoint[ProductIdxNo][0];
        long CurLow = gCurCommHighLowPoint[ProductIdxNo][1];

        DEBUG(DEBUG_LEVEL_DEBUG, "IdxNo: %ld. High: %ld, Low: %ld", ProductIdxNo, CurHigh, CurLow);

        curPrice = gCurCommPrice[ProductIdxNo];

        printf("%s : %ld, ", ProductName.c_str(), gCurCommPrice[ProductIdxNo]);

        printf("CurHigh: %ld, CurLow: %ld\n", CurHigh, CurLow);
    }

    if (gBest5BidOffer[ProductIdxNo].size() >= 10)
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

        string Offer1Deal = "", Offer2Deal = "", Offer3Deal = "", Offer4Deal = "", Offer5Deal = "";
        string Bid1Deal = "", Bid2Deal = "", Bid3Deal = "", Bid4Deal = "", Bid5Deal = "";

        if (curPrice == gBest5BidOffer[ProductIdxNo][9].first)
        {
            Offer5Deal = "*";
        }
        else if (curPrice == gBest5BidOffer[ProductIdxNo][8].first)
        {
            Offer4Deal = "*";
        }
        else if (curPrice == gBest5BidOffer[ProductIdxNo][7].first)
        {
            Offer3Deal = "*";
        }
        else if (curPrice == gBest5BidOffer[ProductIdxNo][6].first)
        {
            Offer2Deal = "*";
        }
        else if (curPrice == gBest5BidOffer[ProductIdxNo][5].first)
        {
            Offer1Deal = "*";
        }
        else if (curPrice == gBest5BidOffer[ProductIdxNo][0].first)
        {
            Bid1Deal = "*";
        }
        else if (curPrice == gBest5BidOffer[ProductIdxNo][1].first)
        {
            Bid2Deal = "*";
        }
        else if (curPrice == gBest5BidOffer[ProductIdxNo][2].first)
        {
            Bid3Deal = "*";
        }
        else if (curPrice == gBest5BidOffer[ProductIdxNo][3].first)
        {
            Bid4Deal = "*";
        }
        else if (curPrice == gBest5BidOffer[ProductIdxNo][4].first)
        {
            Bid5Deal = "*";
        }

        printf("Total Offer: [%ld]\n", TotalOffer);

        printf("Ask5: [%ld]: [%ld]%s\n", gBest5BidOffer[ProductIdxNo][9].first, gBest5BidOffer[ProductIdxNo][9].second, Offer5Deal.c_str());
        printf("Ask4: [%ld]: [%ld]%s\n", gBest5BidOffer[ProductIdxNo][8].first, gBest5BidOffer[ProductIdxNo][8].second, Offer4Deal.c_str());
        printf("Ask3: [%ld]: [%ld]%s\n", gBest5BidOffer[ProductIdxNo][7].first, gBest5BidOffer[ProductIdxNo][7].second, Offer3Deal.c_str());
        printf("Ask2: [%ld]: [%ld]%s\n", gBest5BidOffer[ProductIdxNo][6].first, gBest5BidOffer[ProductIdxNo][6].second, Offer2Deal.c_str());
        printf("Ask1: [%ld]: [%ld]%s\n", gBest5BidOffer[ProductIdxNo][5].first, gBest5BidOffer[ProductIdxNo][5].second, Offer1Deal.c_str());
        printf("=========================================\n");
        printf("Bid1: [%ld]: [%ld]%s\n", gBest5BidOffer[ProductIdxNo][0].first, gBest5BidOffer[ProductIdxNo][0].second, Bid1Deal.c_str());
        printf("Bid2: [%ld]: [%ld]%s\n", gBest5BidOffer[ProductIdxNo][1].first, gBest5BidOffer[ProductIdxNo][1].second, Bid2Deal.c_str());
        printf("Bid3: [%ld]: [%ld]%s\n", gBest5BidOffer[ProductIdxNo][2].first, gBest5BidOffer[ProductIdxNo][2].second, Bid3Deal.c_str());
        printf("Bid4: [%ld]: [%ld]%s\n", gBest5BidOffer[ProductIdxNo][3].first, gBest5BidOffer[ProductIdxNo][3].second, Bid4Deal.c_str());
        printf("Bid5: [%ld]: [%ld]%s\n", gBest5BidOffer[ProductIdxNo][4].first, gBest5BidOffer[ProductIdxNo][4].second, Bid5Deal.c_str());

        printf("Total Bid:   [%ld]\n", TotalBid);

        printf("=========================================\n");
    }
    else
    {
        DEBUG(DEBUG_LEVEL_INFO, "gBest5BidOffer[ProductIdxNo].size() < 10");
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
    AutoLogIn();

    AutoConnect();

    // AutoGetFutureRights();

    long res = pSKQuoteLib->RequestServerTime();

    DEBUG(DEBUG_LEVEL_INFO, "pSKQuoteLib->RequestServerTime()=%d", res);

    res = pSKQuoteLib->GetMarketBuySellUpDown();
    DEBUG(DEBUG_LEVEL_INFO, "pSKQuoteLib->GetMarketBuySellUpDown()=%d", res);

    pSKQuoteLib->GetCommodityIdx();

    //
    const int refreshInterval = 1000; // 1000
    auto lastClearTime = std::chrono::steady_clock::now();

    AutoCalcuKeyPrices();

    std::string CommList;

    std::ostringstream oss;
    oss << COMMODITY_MAIN << "," << "TSEA" << "," << "2330" << "," << "2317";
    CommList = oss.str();

    AutoQuote(CommList, 1);

    AutoQuoteTicks("2330", 2);
    AutoQuoteTicks("2317", 3);

    while (true)
    {
        if (gCurServerTime[0] < 0)
        {
            continue;
        }

        AutoCalcuKeyPrices();
        //
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClearTime);

        {
            // Strategy start:

            StrategyStopFuturesLoss(g_strUserId);
            StrategyClosePosition(g_strUserId);
            StrategyNewPosition(g_strUserId);

            // Strategy End:
        }

        //
        if (elapsed.count() >= refreshInterval)
        {
            //
            system("cls");

            //
            lastClearTime = now;

            printf("[CurMtxPrice: %ld, ", gCurCommPrice[gCommodtyInfo.MTXIdxNo]);
            printf("ServerTime: %d: %d: %d ]", gCurServerTime[0], gCurServerTime[1], gCurServerTime[2]);
            printf("[TSEA prices: %ld, Valume: %ld: Buy: %ld Sell: %ld]\n",
                   gCurCommPrice[gCommodtyInfo.TSEAIdxNo], gCurTaiexInfo[0][1], gCurTaiexInfo[0][2], gCurTaiexInfo[0][3]);

            printf("=========================================\n");

            if (gCurCommHighLowPoint.count(gCommodtyInfo.MTXIdxNo) > 0)
            {

                long CurHigh = gCurCommHighLowPoint[gCommodtyInfo.MTXIdxNo][0] / 100;
                long CurLow = gCurCommHighLowPoint[gCommodtyInfo.MTXIdxNo][1] / 100;

                printf("CurHigh: %ld, CurLow: %ld, ", CurHigh, CurLow);

                printf("CurAmp : %d\n", CurHigh - CurLow);
            }

            printf("=========================================\n");

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

            AutoBest5Long(gCommodtyInfo.TSMCIdxNo, "TSMC");
            AutoBest5Long(gCommodtyInfo.HHIdxNo, "HHP");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); //  CPU

        if (pSKQuoteLib->IsConnected() != 1)
        {
            DEBUG(DEBUG_LEVEL_ERROR, "pSKQuoteLib->IsConnected() != 1");

            // AutoConnect();
            release();
            exit(0);
        }
    }
}

int main()
{

    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    CoInitialize(NULL);

    init();

    // printf("");
    // cin >> g_strUserId;

    g_strUserId = "F129305651";

    // printf("");
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
