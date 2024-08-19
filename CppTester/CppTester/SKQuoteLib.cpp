#include "SKQuoteLib.h"
#include <array>
#include <deque>
#include <iostream>
#include <map>
#include <unordered_map>

#define SK_SUBJECT_CONNECTION_CONNECTED 3001
#define SK_SUBJECT_CONNECTION_DISCONNECT 3002
#define SK_SUBJECT_CONNECTION_STOCKS_READY 3003

std::unordered_map<long, std::array<long, 3>> gCurCommHighLowPoint; // {High, Low, Open}
std::deque<long> gDaysKlineDiff;
std::deque<long> gCostMovingAverage;

std::map<string, pair<double, double>> gDaysCommHighLowPoint;
std::map<string, pair<double, double>> gDaysNightAllCommHighLowPoint;

std::unordered_map<long, long> gCurCommPrice;
std::unordered_map<SHORT, std::array<long, 4>> gCurTaiexInfo;
std::unordered_map<long, vector<pair<long, long>>> gBest5BidOffer;

SHORT gCurServerTime[3] = {-1, -1, -1};

COMMODITY_INFO gCommodtyInfo = {0, 0, 0, 0, 0};

long CalculateDiff(const std::string &data);
void CaluCurCommHighLowPoint(IN long nStockIndex, IN long nClose, IN long nSimulate, IN long lTimehms);
void GetCurPrice(IN long nStockIndex, IN long nClose, IN long nSimulate);
void parseAndProcessData(const string &data);
void ProcessDaysOrNightCommHighLowPoint();

CSKQuoteLib::CSKQuoteLib()
{
    m_pSKQuoteLib.CreateInstance(__uuidof(SKCOMLib::SKQuoteLib));
    m_pSKQuoteLibEventHandler = new ISKQuoteLibEventHandler(*this, m_pSKQuoteLib, &CSKQuoteLib::OnEventFiringObjectInvoke);
}

CSKQuoteLib::~CSKQuoteLib()
{
    if (m_pSKQuoteLibEventHandler)
    {
        m_pSKQuoteLibEventHandler->ShutdownConnectionPoint();
        m_pSKQuoteLibEventHandler->Release();
        m_pSKQuoteLibEventHandler = NULL;
    }

    if (m_pSKQuoteLib)
    {
        m_pSKQuoteLib->Release();
    }
}

HRESULT CSKQuoteLib::OnEventFiringObjectInvoke(
    ISKQuoteLibEventHandler *pEventHandler,
    DISPID dispidMember,
    REFIID riid,
    LCID lcid,
    WORD wFlags,
    DISPPARAMS *pdispparams,
    VARIANT *pvarResult,
    EXCEPINFO *pexcepinfo,
    UINT *puArgErr)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "dispidMember == %d", dispidMember);

    VARIANT varlValue;
    VariantInit(&varlValue);
    VariantClear(&varlValue);

    switch (dispidMember)
    {
    case 1: // OnConnection
    {
        long nKind = V_I4(&(pdispparams->rgvarg)[1]);
        long nCode = V_I4(&(pdispparams->rgvarg)[0]);
        OnConnection(nKind, nCode);
        break;
    }
    case 16: // RequestStockList
    {
        short sMarketNo = V_I2(&(pdispparams->rgvarg)[1]);
        _bstr_t bstrStockData = V_BSTR(&(pdispparams->rgvarg)[0]);
        OnNotifyStockList(sMarketNo, string(bstrStockData));
        break;
    }
    case 19: // OnNotifyQuoteLONG
    {
        short sMarketNo = V_I2(&(pdispparams->rgvarg)[1]);
        long nStockIndex = V_I4(&(pdispparams->rgvarg)[0]);
        OnNotifyQuoteLONG(sMarketNo, nStockIndex);
        break;
    }
    case 20: // OnNotifyHistoryTicksLONG
    {
        long nStockIndex = V_I4(&(pdispparams->rgvarg)[9]);
        long nPtr = V_I4(&(pdispparams->rgvarg)[8]);
        long nDate = V_I4(&(pdispparams->rgvarg)[7]);
        long lTimehms = V_I4(&(pdispparams->rgvarg)[6]);
        long nBid = V_I4(&(pdispparams->rgvarg)[4]);
        long nAsk = V_I4(&(pdispparams->rgvarg)[3]);
        long nClose = V_I4(&(pdispparams->rgvarg)[2]);
        long nQty = V_I4(&(pdispparams->rgvarg)[1]);
        long nSimulate = V_I4(&(pdispparams->rgvarg)[0]);
        OnNotifyHistoryTicksLONG(nStockIndex, nPtr, nDate, lTimehms, nBid, nAsk, nClose, nQty, nSimulate);
        break;
    }
    case 21: // OnNotifyTicksLONG
    {
        long nStockIndex = V_I4(&(pdispparams->rgvarg)[9]);
        long nPtr = V_I4(&(pdispparams->rgvarg)[8]);
        long nDate = V_I4(&(pdispparams->rgvarg)[7]);
        long lTimehms = V_I4(&(pdispparams->rgvarg)[6]);
        long nBid = V_I4(&(pdispparams->rgvarg)[4]);
        long nAsk = V_I4(&(pdispparams->rgvarg)[3]);
        long nClose = V_I4(&(pdispparams->rgvarg)[2]);
        long nQty = V_I4(&(pdispparams->rgvarg)[1]);
        long nSimulate = V_I4(&(pdispparams->rgvarg)[0]);
        OnNotifyTicksLONG(nStockIndex, nPtr, nDate, lTimehms, nBid, nAsk, nClose, nQty, nSimulate);
        break;
    }
    case 22: // OnNotifyBest5LONG
    {
        // from last to first
        LONG nSimulate = V_I4(&(pdispparams->rgvarg)[0]);
        // LONG nExtendAskQty = V_I4(&(pdispparams->rgvarg)[1]);
        LONG nExtendAsk = V_I4(&(pdispparams->rgvarg)[2]);
        LONG nBestAskQty5 = V_I4(&(pdispparams->rgvarg)[3]);
        LONG nBestAsk5 = V_I4(&(pdispparams->rgvarg)[4]);
        LONG nBestAskQty4 = V_I4(&(pdispparams->rgvarg)[5]);
        LONG nBestAsk4 = V_I4(&(pdispparams->rgvarg)[6]);
        LONG nBestAskQty3 = V_I4(&(pdispparams->rgvarg)[7]);
        LONG nBestAsk3 = V_I4(&(pdispparams->rgvarg)[8]);
        LONG nBestAskQty2 = V_I4(&(pdispparams->rgvarg)[9]);
        LONG nBestAsk2 = V_I4(&(pdispparams->rgvarg)[10]);
        LONG nBestAskQty1 = V_I4(&(pdispparams->rgvarg)[11]);
        LONG nBestAsk1 = V_I4(&(pdispparams->rgvarg)[12]);
        // LONG nExtendBidQty = V_I4(&(pdispparams->rgvarg)[13]);
        // LONG nExtendBid = V_I4(&(pdispparams->rgvarg)[14]);
        LONG nBestBidQty5 = V_I4(&(pdispparams->rgvarg)[15]);
        LONG nBestBid5 = V_I4(&(pdispparams->rgvarg)[16]);
        LONG nBestBidQty4 = V_I4(&(pdispparams->rgvarg)[17]);
        LONG nBestBid4 = V_I4(&(pdispparams->rgvarg)[18]);
        LONG nBestBidQty3 = V_I4(&(pdispparams->rgvarg)[19]);
        LONG nBestBid3 = V_I4(&(pdispparams->rgvarg)[20]);
        LONG nBestBidQty2 = V_I4(&(pdispparams->rgvarg)[21]);
        LONG nBestBid2 = V_I4(&(pdispparams->rgvarg)[22]);
        LONG nBestBidQty1 = V_I4(&(pdispparams->rgvarg)[23]);
        LONG nBestBid1 = V_I4(&(pdispparams->rgvarg)[24]);
        LONG nStockidx = V_I4(&(pdispparams->rgvarg)[25]);
        SHORT sMarketNo = V_I2(&(pdispparams->rgvarg)[26]);

        OnNotifyBest5LONG(sMarketNo,
                          nStockidx,
                          nBestBid1,
                          nBestBidQty1,
                          nBestBid2,
                          nBestBidQty2,
                          nBestBid3,
                          nBestBidQty3,
                          nBestBid4,
                          nBestBidQty4,
                          nBestBid5,
                          nBestBidQty5,
                          0,
                          0,
                          nBestAsk1,
                          nBestAskQty1,
                          nBestAsk2,
                          nBestAskQty2,
                          nBestAsk3,
                          nBestAskQty3,
                          nBestAsk4,
                          nBestAskQty4,
                          nBestAsk5,
                          nBestAskQty5,
                          0,
                          0,
                          nSimulate);

        break;
    }

    case 6:
    {
        BSTR bstrStockNo = pdispparams->rgvarg[1].bstrVal;
        BSTR bstrData = pdispparams->rgvarg[0].bstrVal;

        OnNotifyKLineData(bstrStockNo, bstrData);

        break;
    }
    case 7:
    {

        short sHour = V_I2(&(pdispparams->rgvarg)[3]);
        short sMinute = V_I2(&(pdispparams->rgvarg)[2]);
        short sSecond = V_I2(&(pdispparams->rgvarg)[1]);
        LONG nTotal = V_I4(&(pdispparams->rgvarg)[0]);

        OnNotifyServerTime(sHour, sMinute, sSecond, nTotal);

        break;
    }

    case 8:
    {

        SHORT sMarketNo = V_I2(&(pdispparams->rgvarg)[5]);
        LONG nTime = V_I4(&(pdispparams->rgvarg)[3]);
        LONG nTotv = V_I4(&(pdispparams->rgvarg)[2]);

        OnNotifyMarketTot(sMarketNo, 0, nTime, nTotv, 0, 0);

        break;
    }
    case 9:
    {
        SHORT sMarketNo = V_I2(&(pdispparams->rgvarg)[6]);
        LONG nTime = V_I4(&(pdispparams->rgvarg)[4]);
        LONG BuyCount = V_I4(&(pdispparams->rgvarg)[1]);
        LONG SellCount = V_I4(&(pdispparams->rgvarg)[0]);

        OnNotifyMarketBuySell(sMarketNo, 0, nTime, 0, 0, BuyCount, SellCount);

        break;
    }
    case 10:
    case 17:
    {
        break;
    }

    default:
        // Code for other cases
        break;
    }

    return S_OK;
}

// Methods
long CSKQuoteLib::EnterMonitorLONG()
{
    return m_pSKQuoteLib->SKQuoteLib_EnterMonitorLONG();
}

long CSKQuoteLib::IsConnected()
{
    return m_pSKQuoteLib->SKQuoteLib_IsConnected();
}

long CSKQuoteLib::LeaveMonitor()
{
    return m_pSKQuoteLib->SKQuoteLib_LeaveMonitor();
}

long CSKQuoteLib::RequestStocks(short *psPageNo, string strStockNos)
{
    return m_pSKQuoteLib->SKQuoteLib_RequestStocks(psPageNo, _bstr_t(strStockNos.c_str()));
}

long CSKQuoteLib::GetStockByIndexLONG(short sMarketNo, long nStockIndex, SKCOMLib::SKSTOCKLONG *pSKStock)
{
    return m_pSKQuoteLib->SKQuoteLib_GetStockByIndexLONG(sMarketNo, nStockIndex, pSKStock);
}

long CSKQuoteLib::RequestTicks(short *psPageNo, string strStockNos)
{
    // SKQuoteLib_RequestLiveTick
    return m_pSKQuoteLib->SKQuoteLib_RequestTicks(psPageNo, _bstr_t(strStockNos.c_str()));
}

long CSKQuoteLib::RequestStockList(short MarketNo)
{
    return m_pSKQuoteLib->SKQuoteLib_RequestStockList(MarketNo);
}

long CSKQuoteLib::RequestKLine(string strStockNo)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");
    BSTR BstrStockNo = _bstr_t(strStockNo.c_str());

    long res = 0;

    res = m_pSKQuoteLib->SKQuoteLib_RequestKLineAM(BstrStockNo, 0, 1, 0);
    DEBUG(DEBUG_LEVEL_DEBUG, "m_pSKQuoteLib->SKQuoteLib_RequestKLineAM = %d", res);

    return res;
}

long CSKQuoteLib::RequestServerTime()
{
    long res = 0;

    res = m_pSKQuoteLib->SKQuoteLib_RequestServerTime();
    DEBUG(DEBUG_LEVEL_DEBUG, "m_pSKQuoteLib->SKQuoteLib_RequestServerTime = %d", res);

    return res;
}

long CSKQuoteLib::RequestStockIndexMap(IN string strStockNo, OUT SKCOMLib::SKSTOCKLONG *pSKStock)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    BSTR bstrStockNo = _bstr_t(strStockNo.c_str());

    long res = m_pSKQuoteLib->SKQuoteLib_GetStockByNoLONG(bstrStockNo, pSKStock);
    DEBUG(DEBUG_LEVEL_DEBUG, "m_pSKQuoteLib->SKQuoteLib_GetStockByNoLONG = %d", res);

    if (res == 0)
    {
        char *szStockNo = _com_util::ConvertBSTRToString(pSKStock->bstrStockNo);
        char *szStockName = _com_util::ConvertBSTRToString(pSKStock->bstrStockName);

        DEBUG(DEBUG_LEVEL_DEBUG, "szStockNo: %s, szStockName : %s, nStockidx : %ld, nHigh: %d, nLow: %d",
              szStockNo,
              szStockName,
              pSKStock->nStockIdx,
              pSKStock->nHigh,
              pSKStock->nLow);

        delete[] szStockName;
        delete[] szStockNo;
    }

    return res;
}

long CSKQuoteLib::GetMarketBuySellUpDown(VOID)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    long res = m_pSKQuoteLib->SKQuoteLib_GetMarketBuySellUpDown();
    DEBUG(DEBUG_LEVEL_DEBUG, "m_pSKQuoteLib->SKQuoteLib_GetMarketBuySellUpDown = %d", res);

    return res;
}

void CSKQuoteLib::ProcessDaysOrNightCommHighLowPoint()
{
    if (gCurServerTime[0] < 0)
    {
        return;
    }

    bool isDaySession = gCurServerTime[0] >= 8 && gCurServerTime[0] <= 14;

    bool isNightSession = gCurServerTime[0] < 8 || gCurServerTime[0] > 14;

    if (isDaySession)
    {
        DEBUG(DEBUG_LEVEL_INFO, "isDaySession");

        for (const auto &entry : gDaysCommHighLowPoint) // need ordered by date  from the past to the present
        {
            long diff = static_cast<long>(entry.second.first - entry.second.second);

            DEBUG(DEBUG_LEVEL_DEBUG, "Date: %s, High: %f, Low: %f", entry.first, entry.second.first, entry.second.second);

            gDaysKlineDiff.push_back(diff);

            if (gDaysKlineDiff.size() > DayMA)
            {
                gDaysKlineDiff.pop_front();
            }
        }
    }
    else if (isNightSession)
    {
        DEBUG(DEBUG_LEVEL_INFO, "isNightSession");

        for (const auto &entry : gDaysNightAllCommHighLowPoint) // need ordered by date  from the past to the present
        {
            auto cur = entry.second;

            long diff = static_cast<long>(cur.first - cur.second);

            DEBUG(DEBUG_LEVEL_DEBUG, "Date: %s, High: %f, Low: %f", entry.first, cur.first, cur.second);

            gDaysKlineDiff.push_back(diff);

            if (gDaysKlineDiff.size() > DayMA)
            {
                gDaysKlineDiff.pop_front();
            }
        }
    }

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
}

VOID CSKQuoteLib::GetCommodityIdx(VOID)
{
    SKCOMLib::SKSTOCKLONG skStock;

    long MTXIdxNo = 0, MTXIdxNoAM = 0, TSMCIdxNo = 0, HHIdxNo = 0, TSEAIdxNo = 0;

    std::string CommList;

    std::ostringstream oss;
    oss << COMMODITY_MAIN << "AM";
    CommList = oss.str();

    long res = RequestStockIndexMap(CommList, &skStock);

    MTXIdxNoAM = skStock.nStockIdx;

    DEBUG(DEBUG_LEVEL_INFO, "RequestStockIndexMap()=%d, MTXIdxNoAM=%d", res, MTXIdxNoAM);

    res = RequestStockIndexMap(COMMODITY_MAIN, &skStock);

    MTXIdxNo = skStock.nStockIdx;

    DEBUG(DEBUG_LEVEL_INFO, "RequestStockIndexMap()=%d, MTXIdxNo=%d", res, MTXIdxNo);

    res = RequestStockIndexMap("2330", &skStock);

    TSMCIdxNo = skStock.nStockIdx;
    DEBUG(DEBUG_LEVEL_INFO, "RequestStockIndexMap()=%d, TSMCIdxNo=%d", res, TSMCIdxNo);

    res = RequestStockIndexMap("2317", &skStock);

    HHIdxNo = skStock.nStockIdx;

    DEBUG(DEBUG_LEVEL_INFO, "RequestStockIndexMap()=%d, HHIdxNo=%d", res, HHIdxNo);

    res = RequestStockIndexMap("TSEA", &skStock);

    TSEAIdxNo = skStock.nStockIdx;

    DEBUG(DEBUG_LEVEL_INFO, "RequestStockIndexMap()=%d, TSEAIdxNo=%d", res, TSEAIdxNo);

    gCommodtyInfo.HHIdxNo = HHIdxNo;
    gCommodtyInfo.MTXIdxNo = MTXIdxNo;
    gCommodtyInfo.MTXIdxNoAM = MTXIdxNoAM;
    gCommodtyInfo.TSEAIdxNo = TSEAIdxNo;
    gCommodtyInfo.TSMCIdxNo = TSMCIdxNo;
}

// Events
void CSKQuoteLib::OnConnection(long nKind, long nCode)
{
    switch (nKind)
    {
    case SK_SUBJECT_CONNECTION_CONNECTED:
    {
        cout << endl
             << "OnConnection" << endl;
        break;
    }
    case SK_SUBJECT_CONNECTION_DISCONNECT:
    {
        cout << endl
             << "OnConnection Disconnected" << endl;
        break;
    }
    case SK_SUBJECT_CONNECTION_STOCKS_READY: //
    {
        cout << endl
             << "OnConnection STOCKS READY" << endl;
        break;
    }
    }
}

// struct SKSTOCKLONG
// {
//     LONG nStockidx; // Custom stock index code
//     SHORT sDecimal; // Decimal places
//     SHORT sTypeNo;  // EX: (Securities) Category type, 1 Cement, 2 Food, etc.
//     // …etc.
//     BSTR bstrMarketNo;  // Market code
//     BSTR bstrStockNo;   // Stock code, e.g., 1101 Taiwan Cement, TX12 Taiwan Index Futures December, etc.
//     BSTR bstrStockName; // Stock name
//     LONG nHigh;         // Highest price
//     LONG nOpen;         // Opening price
//     LONG nLow;          // Lowest price
//     LONG nClose;        // Closing price
//     LONG nTickQty;      // Tick volume
//     LONG nRef;          // Previous close or reference price
//     LONG nBid;          // Bid price
//     LONG nBc;           // Bid volume
//     LONG nAsk;          // Ask price
//     LONG nAc;           // Ask volume
//     LONG nTBc;          // Bid volume (external volume)
//     LONG nTAc;          // Ask volume (internal volume)
//     LONG nFutureOI;     // Futures open interest (OI)
//     LONG nTQty;         // Total volume
//     LONG nYQty;         // Previous day's volume
//     LONG nUp;           // Upper limit price
//     LONG nDown;         // Lower limit price
//     LONG nSimulate;     // Indicator: 0: Normal, 1: Simulated
//     // * [Securities tick-by-tick] When '1: Simulated' appears during trading, it indicates a price stability measure is in effect.
//     LONG nDayTrade;   // [Limited to securities with whole lot trading] Whether day trading is allowed: 0: Normal, 1: Allowed for buying first and selling later (day trading), 2: Allowed for both buying first and selling later and selling first and buying later (day trading)
//     LONG nTradingDay; // Trading day (YYYYMMDD)
//     // Note: If it's a non-trading day, the data is from the previous trading day.
//     LONG nDealTime; // Transaction time (hhmmss)
// };

void CSKQuoteLib::OnNotifyQuoteLONG(short sMarketNo, long nStockIndex)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    DEBUG(DEBUG_LEVEL_DEBUG, "sMarketNo= %d, nStockIndex=%d", sMarketNo, nStockIndex);

    SKCOMLib::SKSTOCKLONG skStock;

    long nResult = GetStockByIndexLONG(sMarketNo, nStockIndex, &skStock);

    DEBUG(DEBUG_LEVEL_DEBUG, "GetStockByIndexLONG res = ", nResult);

    if (nResult != 0)
    {
        return;
    }

    if (skStock.nSimulate == 1)
    {
        return;
    }

    char *szStockNo = _com_util::ConvertBSTRToString(skStock.bstrStockNo);
    char *szStockName = _com_util::ConvertBSTRToString(skStock.bstrStockName);

    DEBUG(DEBUG_LEVEL_DEBUG, "nStockIndex= %ld, szStockNo: %s, szStockName : %s, nTradingDay: %ld, nDealTime: %ld, Open: %d, High: %d, Low: %d, Close: %d, Simulate: %d",
          nStockIndex,
          szStockNo,
          szStockName,
          skStock.nTradingDay,
          skStock.nDealTime,
          skStock.nOpen,
          skStock.nHigh,
          skStock.nLow,
          skStock.nClose,
          skStock.nSimulate);

    gCurCommHighLowPoint[nStockIndex][0] = skStock.nHigh;
    gCurCommHighLowPoint[nStockIndex][1] = skStock.nLow;
    gCurCommHighLowPoint[nStockIndex][2] = skStock.nOpen;

    GetCurPrice(nStockIndex, skStock.nClose, skStock.nSimulate);

    delete[] szStockName;
    delete[] szStockNo;
}

void CSKQuoteLib::OnNotifyTicksLONG(long nStockIndex, long nPtr, long nDate, long lTimehms, long nBid, long nAsk, long nClose, long nQty, long nSimulate)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    if (nSimulate == 1)
    {
        return;
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "nStockIndex: %ld, nPtr: %ld,nDate: %ld,lTimehms: %ld,nBid: %ld,nAsk: %ld,nClose: %ld,nQty: %ld\n",
          nStockIndex, nPtr, nDate, lTimehms, nBid, nAsk, nClose, nQty);

    if (gBest5BidOffer[nStockIndex].size() < 10)
    {
        return;
    }

    if (gBest5BidOffer[nStockIndex].size() == 10)
    {
        gBest5BidOffer[nStockIndex].push_back({nClose, nQty});
    }
    else if (gBest5BidOffer[nStockIndex].size() >= 11)
    {
        gBest5BidOffer[nStockIndex][10] = {nClose, nQty};
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

void CSKQuoteLib::OnNotifyHistoryTicksLONG(long nStockIndex, long nPtr, long nDate, long lTimehms, long nBid, long nAsk, long nClose, long nQty, long nSimulate)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    if (nSimulate == 1)
    {
        return;
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "nStockIndex: %ld, nPtr: %ld,nDate: %ld,lTimehms: %ld,nBid: %ld,nAsk: %ld,nClose: %ld,nQty: %ld\n",
          nStockIndex, nPtr, nDate, lTimehms, nBid, nAsk, nClose, nQty);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

void CSKQuoteLib::OnNotifyBest5LONG(
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
    LONG nSimulate)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    DEBUG(DEBUG_LEVEL_DEBUG, "Ofr5: [%ld], q5G: [%ld]\n\n", nBestAsk5, nBestAskQty5);
    DEBUG(DEBUG_LEVEL_DEBUG, "Ofr4: [%ld], q4G: [%ld]\n", nBestAsk4, nBestAskQty4);
    DEBUG(DEBUG_LEVEL_DEBUG, "Ofr3: [%ld], q3G: [%ld]\n", nBestAsk3, nBestAskQty3);
    DEBUG(DEBUG_LEVEL_DEBUG, "Ofr2: [%ld], q2G: [%ld]\n", nBestAsk2, nBestAskQty2);
    DEBUG(DEBUG_LEVEL_DEBUG, "Ofr1: [%ld], q1G: [%ld]\n", nBestAsk1, nBestAskQty1);

    DEBUG(DEBUG_LEVEL_DEBUG, "Bid1: [%ld], q1G: [%ld]\n", nBestBid1, nBestBidQty1);
    DEBUG(DEBUG_LEVEL_DEBUG, "Bid2: [%ld], q2G: [%ld]\n", nBestBid2, nBestBidQty2);
    DEBUG(DEBUG_LEVEL_DEBUG, "Bid3: [%ld], q3G: [%ld]\n", nBestBid3, nBestBidQty3);
    DEBUG(DEBUG_LEVEL_DEBUG, "Bid4: [%ld], q4G: [%ld]\n", nBestBid4, nBestBidQty4);
    DEBUG(DEBUG_LEVEL_DEBUG, "Bid5: [%ld], q5G: [%ld]\n\n", nBestBid5, nBestBidQty5);

    if (nSimulate == 0)
    {

        if (gBest5BidOffer.count(nStockidx) <= 0)
        {
            gBest5BidOffer[nStockidx] = {
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0}

            };
        }

        gBest5BidOffer[nStockidx][0] = {nBestBid1, nBestBidQty1};
        gBest5BidOffer[nStockidx][1] = {nBestBid2, nBestBidQty2};
        gBest5BidOffer[nStockidx][2] = {nBestBid3, nBestBidQty3};
        gBest5BidOffer[nStockidx][3] = {nBestBid4, nBestBidQty4};
        gBest5BidOffer[nStockidx][4] = {nBestBid5, nBestBidQty5};

        gBest5BidOffer[nStockidx][5] = {nBestAsk1, nBestAskQty1};
        gBest5BidOffer[nStockidx][6] = {nBestAsk2, nBestAskQty2};
        gBest5BidOffer[nStockidx][7] = {nBestAsk3, nBestAskQty3};
        gBest5BidOffer[nStockidx][8] = {nBestAsk4, nBestAskQty4};
        gBest5BidOffer[nStockidx][9] = {nBestAsk5, nBestAskQty5};
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

void CSKQuoteLib::OnNotifyStockList(long sMarketNo, string strStockData)
{
    string tempstr = "";
    for (int i = 0; i < strStockData.length(); i++)
    {
        if (strStockData[i] == ';')
        {
            cout << tempstr << endl;
            if (tempstr == "##,,")
            {
                break;
            }
            tempstr = "";
            continue;
        }
        tempstr += strStockData[i];
    }

    cout << endl;
}

void CSKQuoteLib::OnNotifyKLineData(BSTR bstrStockNo, BSTR bstrData)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    string strStockNo = string(_bstr_t(bstrStockNo));

    DEBUG(DEBUG_LEVEL_DEBUG, "strStockNo= %s", strStockNo);

    string strData = string(_bstr_t(bstrData));

    // (Date, Open Price, High Price, Low Price, Close Price, Volume)
    // For example, product code (1108)
    // 2021/8/2, 12.95, 13.00, 12.80, 12.95, 291
    // 2024/07/30 16:16, 22256.00, 22259.00, 22250.00, 22255.00, 389
    // The prices retrieved in the new output function have been processed to include decimal points and are provided by the Solace K-Line server.

    DEBUG(DEBUG_LEVEL_DEBUG, "strData= %s", strData);

    parseAndProcessData(strData);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

void CSKQuoteLib::OnNotifyServerTime(SHORT sHour, SHORT sMinute, SHORT sSecond, LONG nTotal)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Hour: %d Minute: %d Second: %d Total[%ld]", sHour, sMinute, sSecond, nTotal);

    gCurServerTime[0] = sHour;
    gCurServerTime[1] = sMinute;
    gCurServerTime[2] = sSecond;
}

void CSKQuoteLib::OnNotifyMarketTot(SHORT sMarketNo, SHORT sPtr, LONG nTime, LONG nTotv, LONG nTots, LONG nTotc)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    DEBUG(DEBUG_LEVEL_DEBUG, "sMarketNo: %d nTime: %d nTotv: %ld", sMarketNo, nTime, nTotv);

    gCurTaiexInfo[sMarketNo][0] = nTime;
    gCurTaiexInfo[sMarketNo][1] = nTotv;

    // GetCurTaiexInfo(sMarketNo, nTime, nTotv, 0, 0);
}

void CSKQuoteLib::OnNotifyMarketBuySell(SHORT sMarketNo, SHORT sPtr, LONG nTime, LONG nBc, LONG nSc, LONG nBs, LONG nSs)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    DEBUG(DEBUG_LEVEL_DEBUG, "sMarketNo: %d nTime: %d Buy: %ld Sell: %ld", sMarketNo, nTime, nBs, nSs);

    gCurTaiexInfo[sMarketNo][2] = nBs;
    gCurTaiexInfo[sMarketNo][3] = nSs;
}

long CalculateDiff(const std::string &data)
{
    std::istringstream stream(data);
    std::string token;
    std::string highStr, lowStr;

    // Skip to the third and fourth values (22156.00 and 21918.00)
    for (int i = 0; i < 4; ++i)
    {
        std::getline(stream, token, ',');
        if (i == 2)
        {
            highStr = token;
        }
        else if (i == 3)
        {
            lowStr = token;
        }
    }

    // Convert the strings to doubles
    double high = std::stod(highStr);
    double low = std::stod(lowStr);

    // Calculate and return the absolute difference
    return std::lround(std::abs(high - low));
}

void CaluCurCommHighLowPoint(IN long nStockIndex, IN long nClose, IN long nSimulate, IN long lTimehms)
{
    if (nClose <= 0 || nSimulate == 1)
    {
        return;
    }

    bool isDaySession = gCurServerTime[0] >= 8 && gCurServerTime[0] <= 14;

    bool isNightSession = gCurServerTime[0] < 8 || gCurServerTime[0] > 14;

    if ((isDaySession && lTimehms > 50000 && lTimehms <= 134500) ||
        (isNightSession && (lTimehms <= 50000 || lTimehms > 134500)))
    {
        if (gCurCommHighLowPoint.count(nStockIndex) <= 0)
        {
            gCurCommHighLowPoint[nStockIndex] = {LONG_MIN, LONG_MAX};
        }

        DEBUG(DEBUG_LEVEL_DEBUG, "nStockIndex = %ld, lTimehms=%ld, nClose=%ld", nStockIndex, lTimehms, nClose);

        gCurCommHighLowPoint[nStockIndex][0] = max(gCurCommHighLowPoint[nStockIndex][0], nClose);
        gCurCommHighLowPoint[nStockIndex][1] = min(gCurCommHighLowPoint[nStockIndex][1], nClose);
    }
}

void GetCurPrice(IN long nStockIndex, IN long nClose, IN long nSimulate)
{
    if (nClose <= 0 || nSimulate == 1)
    {
        return;
    }

    gCurCommPrice[nStockIndex] = nClose;
}

/**
 * @brief Splits the given trading data into day and night sessions and records the highest and lowest prices.
 *
 * @param datetime The datetime string in "YYYY/MM/DD HH:MM" format.
 * @param openPrice The opening price.
 * @param highPrice The highest price.
 * @param lowPrice The lowest price.
 * @param closePrice The closing price.
 * @param volume The trading volume.
 */
void processTradingData(const string &datetime, double openPrice, double highPrice, double lowPrice, double closePrice, int volume)
{

    DEBUG(DEBUG_LEVEL_DEBUG, "datetime: %s, highPrice: %f, lowPrice: %f", datetime, highPrice, lowPrice);

    // Extract the date and time from the datetime string
    string date = datetime.substr(0, 10);
    string time = datetime.substr(11, 5);

    // Convert time to hour and minute
    int hour = stoi(time.substr(0, 2));
    int minute = stoi(time.substr(3, 2));

    if ((hour == 8 && minute >= 45) || (hour >= 9 && hour < 13) || (hour == 13 && minute <= 45))
    {
        // Day session
        if (gDaysCommHighLowPoint.count(date) == 0)
        {
            gDaysCommHighLowPoint[date] = {highPrice, lowPrice};
        }

        auto &entry = gDaysCommHighLowPoint[date];
        entry.first = max(entry.first, highPrice);
        entry.second = min(entry.second, lowPrice);

        DEBUG(DEBUG_LEVEL_DEBUG, "datetime: %s, highPrice: %f, lowPrice: %f", datetime, highPrice, lowPrice);

        DEBUG(DEBUG_LEVEL_DEBUG, "Date08_45: %s, High: %f, Low: %f",
              date, entry.first, entry.second);
    }

    {
        // Night session

        if (gDaysNightAllCommHighLowPoint.count(date) == 0)
        {
            gDaysNightAllCommHighLowPoint[date] = {highPrice, lowPrice};
        }

        auto &entry = gDaysNightAllCommHighLowPoint[date];
        entry.first = max(entry.first, highPrice);
        entry.second = min(entry.second, lowPrice);

        DEBUG(DEBUG_LEVEL_DEBUG, "datetime: %s, highPrice: %f, lowPrice: %f", datetime, highPrice, lowPrice);

        DEBUG(DEBUG_LEVEL_DEBUG, "Date15_00: %s, High: %f, Low: %f",
              date, entry.first, entry.second);
    }
}

/**
 * @brief Parses a trading data string and processes it.
 *
 * @param data The trading data string in the format "YYYY/MM/DD HH:MM, openPrice, highPrice, lowPrice, closePrice, volume".
 */
void parseAndProcessData(const string &data)
{

    stringstream ss(data);
    string datetime;
    double openPrice, highPrice, lowPrice, closePrice;
    int volume;

    getline(ss, datetime, ',');
    ss >> openPrice;
    ss.ignore(1); // Ignore the comma
    ss >> highPrice;
    ss.ignore(1); // Ignore the comma
    ss >> lowPrice;
    ss.ignore(1); // Ignore the comma
    ss >> closePrice;
    ss.ignore(1); // Ignore the comma
    ss >> volume;

    processTradingData(datetime, openPrice, highPrice, lowPrice, closePrice, volume);
}
