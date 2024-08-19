#pragma once

#include "SKCOM_reference.h"
#include "TEventHandler.h"

#define COMMODITY_MAIN "MTX00"
#define COMMODITY_OTHER "TM0000"
#define COMMODITY_TX_MAIN "TX00"

#define DayMA 20
#define COST_DAY_MA 1

struct COMMODITY_INFO
{
    long MTXIdxNo;
    long MTXIdxNoAM;
    long TSMCIdxNo;
    long HHIdxNo;
    long TSEAIdxNo;
};

class CSKQuoteLib
{
public:
    typedef TEventHandlerNamespace::TEventHandler<CSKQuoteLib, SKCOMLib::ISKQuoteLib, SKCOMLib::_ISKQuoteLibEvents> ISKQuoteLibEventHandler;

    CSKQuoteLib();
    ~CSKQuoteLib();

    // Methods
    long EnterMonitorLONG();
    long IsConnected();
    long LeaveMonitor();
    long RequestStocks(short *psPageNo, string strStockNos);
    long RequestTicks(short *psPageNo, string strStockNos);
    long RequestStockList(short MarketNo);
    long GetStockByIndexLONG(short sMarketNo, long bStockIndex, SKCOMLib::SKSTOCKLONG *pSKStock);
    long RequestKLine(string strStockNo);
    long RequestServerTime();
    long RequestStockIndexMap(IN string strStockNo, OUT SKCOMLib::SKSTOCKLONG *pSKStock);
    long GetMarketBuySellUpDown(VOID);
    void ProcessDaysOrNightCommHighLowPoint();
    VOID GetCommodityIdx(VOID);

    // Events
    void OnConnection(long nKind, long nCode);
    void OnNotifyQuoteLONG(short sMarketNo, long nStockIndex);
    void OnNotifyTicksLONG(long nStockIndex, long nPtr, long nDate, long lTimehms, long nBid, long nAsk, long nClose, long nQty, long nSimulate);
    void OnNotifyHistoryTicksLONG(long nStockIndex, long nPtr, long nDate, long lTimehms, long nBid, long nAsk, long nClose, long nQty, long nSimulate);
    void OnNotifyBest5LONG(
        SHORT sMarketNo, LONG nStockidx,
        long nBestBid1, long nBestBidQty1,
        long nBestBid2, long nBestBidQty2,
        long nBestBid3, long nBestBidQty3,
        long nBestBid4, long nBestBidQty4,
        long nBestBid5, long nBestBidQty5,
        LONG nExtendBid, LONG nExtendBidQty,
        long nBestAsk1, long nBestAskQty1,
        long nBestAsk2, long nBestAskQty2,
        long nBestAsk3, long nBestAskQty3,
        long nBestAsk4, long nBestAskQty4,
        long nBestAsk5, long nBestAskQty5,
        LONG nExtendAsk, LONG nExtendAskQty,
        LONG nSimulate);

    void OnNotifyStockList(long sMarketNo, string strStockData);
    void OnNotifyKLineData(BSTR bstrStockNo, BSTR bstrData);
    void OnNotifyServerTime(SHORT sHour, SHORT sMinute, SHORT sSecond, LONG nTotal);
    void OnNotifyMarketTot(SHORT sMarketNo, SHORT sPtr, LONG nTime, LONG nTotv, LONG nTots, LONG nTotc);
    void OnNotifyMarketBuySell(SHORT sMarketNo, SHORT sPtr, LONG nTime, LONG nBc, LONG nSc, LONG nBs, LONG nSs);

private:
    HRESULT OnEventFiringObjectInvoke(
        ISKQuoteLibEventHandler *pEventHandler,
        DISPID dispidMember,
        REFIID riid,
        LCID lcid,
        WORD wFlags,
        DISPPARAMS *pdispparams,
        VARIANT *pvarResult,
        EXCEPINFO *pexcepinfo,
        UINT *puArgErr);

    SKCOMLib::ISKQuoteLibPtr m_pSKQuoteLib;
    ISKQuoteLibEventHandler *m_pSKQuoteLibEventHandler;
};

VOID GetCommodityIdx(VOID);
void loadHighLowPoints();
void updateHighLowPoints(const std::string &date, double dayHigh, double dayLow, double nightHigh, double nightLow);
