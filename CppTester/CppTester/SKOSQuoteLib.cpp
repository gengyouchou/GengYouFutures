#include "SKOSQuoteLib.h"
#include "Strategy.h"
#include <array>
#include <deque>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <yaml-cpp/yaml.h>

#define SK_SUBJECT_CONNECTION_CONNECTED 3001
#define SK_SUBJECT_CONNECTION_DISCONNECT 3002
#define SK_SUBJECT_CONNECTION_STOCKS_READY 3003

std::unordered_map<long, std::array<long, 6>> gOsTransactionList;
std::unordered_map<long, long> gCurOsCommPrice;
COMMODITY_OS_INFO gCommodtyOsInfo = {-1};

CSKOSQuoteLib::CSKOSQuoteLib()
{
    m_pSKOSQuoteLib.CreateInstance(__uuidof(SKCOMLib::SKOSQuoteLib));
    m_pSKOSQuoteLibEventHandler = new ISKOSQuoteLibEventHandler(*this, m_pSKOSQuoteLib, &CSKOSQuoteLib::OnEventFiringObjectInvoke);
}

CSKOSQuoteLib::~CSKOSQuoteLib()
{
    if (m_pSKOSQuoteLibEventHandler)
    {
        m_pSKOSQuoteLibEventHandler->ShutdownConnectionPoint();
        m_pSKOSQuoteLibEventHandler->Release();
        m_pSKOSQuoteLibEventHandler = NULL;
    }

    if (m_pSKOSQuoteLib)
    {
        m_pSKOSQuoteLib->Release();
    }
}

HRESULT CSKOSQuoteLib::OnEventFiringObjectInvoke(
    ISKOSQuoteLibEventHandler *pEventHandler,
    DISPID dispidMember,
    REFIID riid,
    LCID lcid,
    WORD wFlags,
    DISPPARAMS *pdispparams,
    VARIANT *pvarResult,
    EXCEPINFO *pexcepinfo,
    UINT *puArgErr)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "CSKOSQuoteLib DispidMember == %d", dispidMember);

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

    case 16: // OnNotifyTicksNineDigitLONG
    {
        long nStockIndex = V_I4(&(pdispparams->rgvarg)[5]);
        long nPtr = V_I4(&(pdispparams->rgvarg)[4]);
        long nDate = V_I4(&(pdispparams->rgvarg)[3]);
        long lTimehms = V_I4(&(pdispparams->rgvarg)[2]);
        LONG nClose = V_I4(&(pdispparams->rgvarg)[1]);
        long nQty = V_I4(&(pdispparams->rgvarg)[0]);
        OnNotifyTicksNineDigitLONG(nStockIndex, nPtr, nDate, lTimehms, nClose, nQty);

        break;
    }

    default:
        // Code for other cases
        break;
    }

    return S_OK;
}

// Methods
long CSKOSQuoteLib::EnterMonitorLONG()
{
    return m_pSKOSQuoteLib->SKOSQuoteLib_EnterMonitorLONG();
}

long CSKOSQuoteLib::IsConnected()
{
    return m_pSKOSQuoteLib->SKOSQuoteLib_IsConnected();
}

long CSKOSQuoteLib::LeaveMonitor()
{
    return m_pSKOSQuoteLib->SKOSQuoteLib_LeaveMonitor();
}

long CSKOSQuoteLib::RequestStocks(short *psPageNo, string strStockNos)
{
    return m_pSKOSQuoteLib->SKOSQuoteLib_RequestStocks(psPageNo, _bstr_t(strStockNos.c_str()));
}

long CSKOSQuoteLib::GetStockByIndexLONG(long nStockIndex, SKCOMLib::SKFOREIGNLONG *pSKStock)
{
    return m_pSKOSQuoteLib->SKOSQuoteLib_GetStockByIndexLONG(nStockIndex, pSKStock);
}

long CSKOSQuoteLib::RequestTicks(short *psPageNo, string strStockNos)
{
    // SKOSQuoteLib_RequestLiveTick
    return m_pSKOSQuoteLib->SKOSQuoteLib_RequestLiveTick(psPageNo, _bstr_t(strStockNos.c_str()));
}

long CSKOSQuoteLib::RequestStockIndexMap(IN string strStockNo, OUT SKCOMLib::SKFOREIGNLONG *pSKStock)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    BSTR bstrStockNo = _bstr_t(strStockNo.c_str());

    long res = m_pSKOSQuoteLib->SKOSQuoteLib_GetStockByNoLONG(bstrStockNo, pSKStock);
    DEBUG(DEBUG_LEVEL_DEBUG, "m_pSKOSQuoteLib->SKOSQuoteLib_GetStockByNoLONG = %d", res);

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

// Events
void CSKOSQuoteLib::OnConnection(long nKind, long nCode)
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

void CSKOSQuoteLib::OnNotifyTicksNineDigitLONG(LONG nStockIndex, LONG nPtr, LONG nDate, LONG lTimehms, LONG nClose, LONG nQty)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    if (nClose <= 0)
    {
        return;
    }

    DEBUG(DEBUG_LEVEL_DEBUG, "nStockIndex: %ld, nPtr: %ld,nDate: %ld, lTimehms: %ld, nClose: %ld, nQty: %ld\n",
          nStockIndex, nPtr, nDate, lTimehms, nClose, nQty);

    gOsTransactionList[nStockIndex][0] = nPtr;
    gOsTransactionList[nStockIndex][1] = nClose;
    gOsTransactionList[nStockIndex][2] = nQty;
    gOsTransactionList[nStockIndex][3] = lTimehms;

    gCurOsCommPrice[nStockIndex] = nClose;

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

LONG CSKOSQuoteLib::GetCommodityIdx(VOID)
{
    SKCOMLib::SKFOREIGNLONG skStock;

    long res = RequestStockIndexMap(COMMODITY_OS_MAIN, &skStock);

    gCommodtyOsInfo.NQIdxNo = skStock.nStockIdx;

    DEBUG(DEBUG_LEVEL_INFO, "RequestStockIndexMap()=%ld, COMMODITY_OS_MAIN=%ld", res, skStock.nStockIdx);

    return res;
}
