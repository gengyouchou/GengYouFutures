#pragma once

#include <array>
#include <deque>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

#include "SKCOM_reference.h"
#include "TEventHandler.h"
#include "config.h"

#define COMMODITY_OS_MAIN "CME,NQ0000"

struct COMMODITY_OS_INFO
{
    long NQIdxNo;
};

extern std::unordered_map<long, std::array<long, 6>> gOsTransactionList;
extern std::unordered_map<long, long> gCurOsCommPrice;
extern COMMODITY_OS_INFO gCommodtyOsInfo;

class CSKOSQuoteLib
{
public:
    typedef TEventHandlerNamespace::TEventHandler<CSKOSQuoteLib, SKCOMLib::ISKOSQuoteLib, SKCOMLib::_ISKOSQuoteLibEvents> ISKOSQuoteLibEventHandler;

    CSKOSQuoteLib();
    ~CSKOSQuoteLib();

    // Methods
    long EnterMonitorLONG();
    long IsConnected();
    long LeaveMonitor();
    long RequestStocks(short *psPageNo, string strStockNos);
    long RequestTicks(short *psPageNo, string strStockNos);
    long RequestStockList(short MarketNo);
    long GetStockByIndexLONG(long bStockIndex, SKCOMLib::SKFOREIGNLONG *pSKStock);
    long RequestKLine(string strStockNo);
    long RequestServerTime();
    long RequestStockIndexMap(IN string strStockNo, OUT SKCOMLib::SKFOREIGNLONG *pSKStock);
    long GetMarketBuySellUpDown(VOID);
    void ProcessDaysOrNightCommHighLowPoint();
    VOID GetCommodityIdx(VOID);

    // Events
    void OnConnection(long nKind, long nCode);
    void OnNotifyQuoteLONG(short sMarketNo, long nStockIndex);
    void OnNotifyTicksNineDigitLONG(LONG nStockIndex, LONG nPtr, LONG nDate, LONG lTimehms, LONG nClose, LONG nQty);

    void OnNotifyStockList(long sMarketNo, string strStockData);
    void OnNotifyKLineData(BSTR bstrStockNo, BSTR bstrData);
    void OnNotifyServerTime(SHORT sHour, SHORT sMinute, SHORT sSecond, LONG nTotal);
    void OnNotifyMarketTot(SHORT sMarketNo, SHORT sPtr, LONG nTime, LONG nTotv, LONG nTots, LONG nTotc);
    void OnNotifyMarketBuySell(SHORT sMarketNo, SHORT sPtr, LONG nTime, LONG nBc, LONG nSc, LONG nBs, LONG nSs);

private:
    HRESULT OnEventFiringObjectInvoke(
        ISKOSQuoteLibEventHandler *pEventHandler,
        DISPID dispidMember,
        REFIID riid,
        LCID lcid,
        WORD wFlags,
        DISPPARAMS *pdispparams,
        VARIANT *pvarResult,
        EXCEPINFO *pexcepinfo,
        UINT *puArgErr);

    SKCOMLib::ISKOSQuoteLibPtr m_pSKOSQuoteLib;
    ISKOSQuoteLibEventHandler *m_pSKOSQuoteLibEventHandler;
};

VOID GetCommodityIdx(VOID);
void loadHighLowPoints();
void updateHighLowPoints(const std::string &date, double dayHigh, double dayLow, double nightHigh, double nightLow);

extern std::unordered_map<long, std::array<long, 6>> gTransactionList;
// long nPtr, long nBid, long nAsk, long nClose, long nQty
