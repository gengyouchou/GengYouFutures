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

double gMaximumLoss = 5000;

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

void AutoOrder(IN string ProductNum, IN SHORT NewClose, IN SHORT BuySell)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Started");

    long g_nCode = pSKOrderLib->SendFutureOrder(g_strUserId,
                                                false, // bAsyncOrder 是否為非同步委託。
                                                ProductNum,
                                                1,        // IOC
                                                BuySell,  // BuySell
                                                0,        // DayTrade
                                                NewClose, // NewClose // 新平倉，0:新倉 1:平倉 2:自動{新期貨、選擇權使用}
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
    // AutoOrder won’t place a closing order with the wrong amount
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

        double profitAndLoss = 0;

        if (gOpenInterestInfo.buySell == "S")
        {
            profitAndLoss = gOpenInterestInfo.avgCost - static_cast<double>(gCurCommPrice[IdxNo]);
        }
        else
        {
            profitAndLoss = static_cast<double>(gCurCommPrice[IdxNo]) - gOpenInterestInfo.avgCost;
        }

        LOG(DEBUG_LEVEL_INFO, "gCurCommPrice[IdxNo]= %ld, gOpenInterestInfo.avgCost= %f, profit and loss:%f",
            gCurCommPrice[IdxNo], gOpenInterestInfo.avgCost, profitAndLoss);

        if (profitAndLoss >= gMaximumLoss)
        {

            if (gOpenInterestInfo.buySell == "S")
            {
                AutoOrder(gOpenInterestInfo.product,
                          1, // Close
                          0  // Buy
                );
            }
            else
            {
                AutoOrder(gOpenInterestInfo.product,
                          1, // Close
                          1  // Sell
                );
            }
        }
    }
    else
    {
        SKOrderLib->GetOpenInterest(strUserId, 1);
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "End");
}
