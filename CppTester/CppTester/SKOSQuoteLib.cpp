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
// long nPtr, long nBid, long nAsk, long nClose, long nQty,

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
    DEBUG(DEBUG_LEVEL_INFO, "CSKOSQuoteLib DispidMember == %d", dispidMember);

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

    case 21: // OnNotifyTicksNineDigitLONG
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
        OnNotifyTicksNineDigitLONG(nStockIndex, nPtr, nDate, lTimehms, nBid, nAsk, nClose, nQty, nSimulate);
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
    return m_pSKOSQuoteLib->SKOSQuoteLib_RequestTicks(psPageNo, _bstr_t(strStockNos.c_str()));
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

void CSKOSQuoteLib::OnNotifyTicksNineDigitLONG(long nStockIndex, long nPtr, long nDate, long lTimehms, long nBid, long nAsk, long nClose, long nQty, long nSimulate)
{
    DEBUG(DEBUG_LEVEL_INFO, "start");

    if (nSimulate == 1 || nBid == 0 || nAsk == 0)
    {
        return;
    }

    DEBUG(DEBUG_LEVEL_INFO, "nStockIndex: %ld, nPtr: %ld,nDate: %ld,lTimehms: %ld,nBid: %ld,nAsk: %ld,nClose: %ld,nQty: %ld\n",
          nStockIndex, nPtr, nDate, lTimehms, nBid, nAsk, nClose, nQty);

    gOsTransactionList[nStockIndex][0] = nPtr;
    gOsTransactionList[nStockIndex][1] = nBid;
    gOsTransactionList[nStockIndex][2] = nAsk;
    gOsTransactionList[nStockIndex][3] = nClose;
    gOsTransactionList[nStockIndex][4] = nQty;
    gOsTransactionList[nStockIndex][5] = lTimehms;

    DEBUG(DEBUG_LEVEL_INFO, "end");
}
