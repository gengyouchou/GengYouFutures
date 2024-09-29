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
    long RequestStockIndexMap(IN string strStockNo, OUT SKCOMLib::SKFOREIGNLONG *pSKStock);
    VOID GetCommodityIdx(VOID);

    // Events
    void OnConnection(long nKind, long nCode);
    void OnNotifyQuoteLONG(short sMarketNo, long nStockIndex);
    void OnNotifyTicksNineDigitLONG(LONG nStockIndex, LONG nPtr, LONG nDate, LONG lTimehms, LONG nClose, LONG nQty);

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