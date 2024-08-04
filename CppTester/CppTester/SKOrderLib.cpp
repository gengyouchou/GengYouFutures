#include "SKOrderLib.h"
#include "SKCenterLib.h"

#include <Logger.h>
#include <string>
#include <vector>

using namespace std;

CSKOrderLib::CSKOrderLib()
{
    m_pSKOrderLib.CreateInstance(__uuidof(SKCOMLib::SKOrderLib));
    m_pSKOrderLibEventHandler = new ISKOrderLibEventHandler(*this, m_pSKOrderLib, &CSKOrderLib::OnEventFiringObjectInvoke);
}

CSKOrderLib::~CSKOrderLib()
{
    if (m_pSKOrderLibEventHandler)
    {
        m_pSKOrderLibEventHandler->ShutdownConnectionPoint();
        m_pSKOrderLibEventHandler->Release();
        m_pSKOrderLibEventHandler = NULL;
    }

    if (m_pSKOrderLib)
    {
        m_pSKOrderLib->Release();
    }
}

HRESULT CSKOrderLib::OnEventFiringObjectInvoke(
    ISKOrderLibEventHandler *pEventHandler,
    DISPID dispidMember,
    REFIID riid,
    LCID lcid,
    WORD wFlags,
    DISPPARAMS *pdispparams,
    VARIANT *pvarResult,
    EXCEPINFO *pexcepinfo,
    UINT *puArgErr)
{
    VARIANT varlValue;
    VariantInit(&varlValue);
    VariantClear(&varlValue);

    DEBUG(DEBUG_LEVEL_DEBUG, "dispidMember == %d", dispidMember);

    switch (dispidMember)
    {
    case 1:
    {
        varlValue = (pdispparams->rgvarg)[1];
        _bstr_t bstrLoginID = V_BSTR(&varlValue);
        varlValue = (pdispparams->rgvarg)[0];
        _bstr_t bstrData = V_BSTR(&varlValue);
        OnAccount(string(bstrLoginID), string(bstrData));

        break;
    }
    case 2:
    {
        varlValue = (pdispparams->rgvarg)[2];
        LONG nThreadID = V_I4(&varlValue);
        varlValue = (pdispparams->rgvarg)[1];
        LONG nCode = V_I4(&varlValue);
        varlValue = (pdispparams->rgvarg)[0];
        _bstr_t bstrMessage = V_BSTR(&varlValue);
        OnAsyncOrder(nThreadID, nCode, string(bstrMessage));

        break;
    }
    case 9: // DISPID == 9
    {
        if (pdispparams->cArgs == 1)
        {
            BSTR bstrData = pdispparams->rgvarg[0].bstrVal;
            OnFutureRights(bstrData);
        }
        break;
    }
    }

    return S_OK;
}

// Methods
long CSKOrderLib::Initialize()
{
    return m_pSKOrderLib->SKOrderLib_Initialize();
}

long CSKOrderLib::GetUserAccount()
{
    return m_pSKOrderLib->GetUserAccount();
}

long CSKOrderLib::GetFutureRights(string strLogInID)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    string strFullAccount_TF = "";

    if (vec_strFullAccount_TF.size() > 0)
        strFullAccount_TF = vec_strFullAccount_TF[0];
    else
    {
        cout << "GetFutureRights Error : No Future Account.";
        return -1;
    }

    BSTR BstrUserId = _bstr_t(strLogInID.c_str());
    BSTR BstrFullAccount = _bstr_t(strFullAccount_TF.c_str()).Detach();

    long res = m_pSKOrderLib->GetFutureRights(BstrUserId, BstrFullAccount, 0);

    DEBUG(DEBUG_LEVEL_DEBUG, "m_pSKOrderLib->GetFutureRights result = %d", res);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");

    return res;
}

long CSKOrderLib::ReadCertByID(string strLogInID)
{
    return m_pSKOrderLib->ReadCertByID(_bstr_t(strLogInID.c_str()));
}

long CSKOrderLib::SendStockOrder(string strLogInID, bool bAsyncOrder, string strStockNo, short sPrime, short sPeriod, short sFlag, short sBuySell, string strPrice, long nQty, long nTradeType, long nSpecialTradeType)
{
    string strFullAccount_TS = "";

    if (vec_strFullAccount_TS.size() > 0)
        strFullAccount_TS = vec_strFullAccount_TS[0];
    else
    {
        cout << "SendStockOrder Error : No Stock Account.";
        return -1;
    }

    SKCOMLib::STOCKORDER pOrder;
    pOrder.bstrFullAccount = _bstr_t(strFullAccount_TS.c_str()).Detach();
    pOrder.bstrStockNo = _bstr_t(strStockNo.c_str()).Detach();
    pOrder.sPrime = sPrime;
    pOrder.sPeriod = sPeriod;
    pOrder.sFlag = sFlag;
    pOrder.sBuySell = sBuySell;
    pOrder.bstrPrice = _bstr_t(strPrice.c_str()).Detach();
    pOrder.nQty = nQty;
    pOrder.nTradeType = nTradeType;
    pOrder.nSpecialTradeType = nSpecialTradeType;

    BSTR bstrMessage;
    long m_nCode = m_pSKOrderLib->SendStockOrder(_bstr_t(strLogInID.c_str()), VARIANT_BOOL(bAsyncOrder), &pOrder, &bstrMessage);

    cout << "SendStockOrder : " << string(_bstr_t(bstrMessage)) << endl;

    ::SysFreeString(bstrMessage);

    return m_nCode;
}

long CSKOrderLib::SendFutureOrder(string strLogInID, bool bAsyncOrder, string strStockNo, short sTradeType, short sBuySell, short sDayTrade, short sNewClose, string strPrice, long nQty, short sReserved)
{
    string strFullAccount_TF = "";

    if (vec_strFullAccount_TF.size() > 0)
        strFullAccount_TF = vec_strFullAccount_TF[0];
    else
    {
        cout << "SendFutureOrder Error : No Future Account.";
        return -1;
    }

    SKCOMLib::FUTUREORDER pFutures;
    pFutures.bstrFullAccount = _bstr_t(strFullAccount_TF.c_str()).Detach();
    pFutures.bstrStockNo = _bstr_t(strStockNo.c_str()).Detach();
    pFutures.sTradeType = sTradeType;
    pFutures.sBuySell = sBuySell;
    pFutures.sDayTrade = sDayTrade;
    pFutures.sNewClose = sNewClose;
    pFutures.bstrPrice = _bstr_t(strPrice.c_str()).Detach();
    pFutures.nQty = nQty;
    pFutures.sReserved = sReserved;
    pFutures.nOrderPriceType = 3;

    BSTR bstrMessage;
    long m_nCode = m_pSKOrderLib->SendFutureOrderCLR(_bstr_t(strLogInID.c_str()), VARIANT_BOOL(bAsyncOrder), &pFutures, &bstrMessage);

    string StrMessage = string(_bstr_t(bstrMessage));
    cout << "SendFutureOrder : " << StrMessage << endl;

    DEBUG(DEBUG_LEVEL_INFO, "SendFutureOrder : %s", StrMessage);

    ::SysFreeString(bstrMessage);

    return m_nCode;
}

// struct FUTUREORDER
// {
//     BSTR bstrFullAccount; // 期貨帳號，分公司代碼＋帳號7碼
//     BSTR bstrStockNo;     // 委託期權商品代號
//     SHORT sTradeType;     // 0:ROD 3:IOC 4:FOK ,sTradeType為ROD時，nOrderPriceType僅可指定限價
//     SHORT sBuySell;  // 0:買進 1:賣出
//     SHORT sDayTrade; // 當沖0:否 1:是，可當沖商品請參考交易所規定。
//     SHORT sNewClose; // 新平倉，0:新倉 1:平倉 2:自動
//     BSTR bstrPrice;  // 委託價格，(指定限價時，需填此欄)、[OCO]停利委託價。
//                      // 請設nOrderPriceType代表範圍市價，不可用特殊價代碼「P」代表範圍市價 ,{移動停損不須填委託價格}
//     BSTR bstrPrice2; //[OCO]停損委託價。
//     LONG nQty;           // 交易口數
//     BSTR bstrTrigger;    // 觸發價，觸發基準價、[OCO]停利觸發價。{期貨停損、移動停損、選擇權停損、MIT、OCO下單使用：不可0、不可給特殊價代碼P}
//     BSTR bstrTrigger2; //[OCO]停損觸發價。

//     BSTR bstrMovingPoint; //{僅移動停損下單使用}移動點數。
//     SHORT sReserved;      // 盤別，0:盤中(T盤及T+1盤)；1:T盤預約{MIT 單不須填盤別}
//     BSTR bstrDealPrice;   // 成交價 {限MIT下單使用：不可0、不可給特殊價代碼, 需自行取得委託當下成交價}

//     BSTR bstrSettlementMonth; // 委託商品年月，YYYYMM共6碼(EX: 202206)
//     LONG nOrderPriceType;     // 委託價類別  2: 限價; 3:範圍市價 （不支援市價）
//                               // sTradeType為ROD時，nOrderPriceType僅可指定限價
//     LONG nTriggerDirection;   //{限MIT下單使用} 觸發方向1:GTE, 2:LTE
// };
// 註 : 若委託期貨商品代號為TX00、 MTX00等近月商品，則可忽略bstrSettlementMonth商品年月

long CSKOrderLib::SendFutureStop(string strLogInID,
                                 bool bAsyncOrder,
                                 string strStockNo,
                                 short sTradeType,
                                 short sBuySell,
                                 short sDayTrade,
                                 short sNewClose,
                                 string strPrice,
                                 string strTrigger,
                                 long nQty,
                                 short sReserved)
{
    DEBUG(DEBUG_LEVEL_INFO, "Start");

    string strFullAccount_TF = "";

    if (vec_strFullAccount_TF.size() > 0)
        strFullAccount_TF = vec_strFullAccount_TF[0];
    else
    {
        cout << "SendFutureStop Error : No Future Account.";
        return -1;
    }

    DEBUG(DEBUG_LEVEL_INFO, "Consturt FUTUREORDER");

    SKCOMLib::FUTUREORDER pFutures;
    pFutures.bstrFullAccount = _bstr_t(strFullAccount_TF.c_str()).Detach();
    pFutures.bstrStockNo = _bstr_t(strStockNo.c_str()).Detach();
    pFutures.sTradeType = sTradeType;
    pFutures.sBuySell = sBuySell;
    pFutures.sDayTrade = sDayTrade;
    pFutures.sNewClose = sNewClose;
    //pFutures.bstrPrice = _bstr_t(strPrice.c_str()).Detach();
    pFutures.bstrTrigger = _bstr_t(strTrigger.c_str()).Detach(); // For stop
    pFutures.nQty = nQty;
    pFutures.sReserved = sReserved;
    pFutures.nOrderPriceType = 3;

    DEBUG(DEBUG_LEVEL_INFO, "SendFutureStopLossOrder at %s", strTrigger);

    BSTR bstrMessage;
    long m_nCode = m_pSKOrderLib->SendFutureStopLossOrder(_bstr_t(strLogInID.c_str()), VARIANT_BOOL(bAsyncOrder), &pFutures, &bstrMessage);

    DEBUG(DEBUG_LEVEL_INFO, "m_nCode=%ld", m_nCode);

    string StrMessage = string(_bstr_t(bstrMessage));
    cout << "SendFutureStop : " << StrMessage << endl;

    DEBUG(DEBUG_LEVEL_INFO, "SendFutureStop : %s", StrMessage);

    ::SysFreeString(bstrMessage);

    DEBUG(DEBUG_LEVEL_INFO, "End");

    return m_nCode;
}

long CSKOrderLib::SendOptionOrder(string strLogInID, bool bAsyncOrder, string strStockNo, short sTradeType, short sBuySell, short sDayTrade, short sNewClose, string strPrice, long nQty, short sReserved)
{
    string strFullAccount_TF = "";

    if (vec_strFullAccount_TF.size() > 0)
        strFullAccount_TF = vec_strFullAccount_TF[0];
    else
    {
        cout << "SendOptionOrder Error : No Future Account.";
        return -1;
    }

    SKCOMLib::FUTUREORDER pFutures;
    pFutures.bstrFullAccount = _bstr_t(strFullAccount_TF.c_str()).Detach();
    pFutures.bstrStockNo = _bstr_t(strStockNo.c_str()).Detach();
    pFutures.sTradeType = sTradeType;
    pFutures.sBuySell = sBuySell;
    pFutures.sDayTrade = sDayTrade;
    pFutures.sNewClose = sNewClose;
    pFutures.bstrPrice = _bstr_t(strPrice.c_str()).Detach();
    pFutures.nQty = nQty;
    pFutures.sReserved = sReserved;

    BSTR bstrMessage;
    long m_nCode = m_pSKOrderLib->SendOptionOrder(_bstr_t(strLogInID.c_str()), VARIANT_BOOL(bAsyncOrder), &pFutures, &bstrMessage);
    cout << "SendOptionOrder : " << string(_bstr_t(bstrMessage)) << endl;

    ::SysFreeString(bstrMessage);

    return m_nCode;
}

long CSKOrderLib::CancelOrder(string strLogInID, bool bAsyncOrder, int nMarket, int nType, string strNo)
{
    string strFullAccount = "";
    long m_nCode = 0;

    if (nMarket == 0)
    {
        if (vec_strFullAccount_TS.size() > 0)
            strFullAccount = vec_strFullAccount_TS[0];
        else
        {
            cout << "CancelOrder Error : No Stock Account.";
            return -1;
        }
    }
    else if (nMarket == 1 || nMarket == 2)
    {
        if (vec_strFullAccount_TF.size() > 0)
            strFullAccount = vec_strFullAccount_TF[0];
        else
        {
            cout << "CancelOrder Error : No Future Account.";
            return -1;
        }
    }

    BSTR bstrMessage = ::SysAllocString(L"");

    if (nType == 0)
        m_nCode = m_pSKOrderLib->CancelOrderBySeqNo(_bstr_t(strLogInID.c_str()), VARIANT_BOOL(bAsyncOrder),
                                                    _bstr_t(strFullAccount.c_str()), _bstr_t(strNo.c_str()), &bstrMessage);
    else if (nType == 1)
        m_nCode = m_pSKOrderLib->CancelOrderByBookNo(_bstr_t(strLogInID.c_str()), VARIANT_BOOL(bAsyncOrder),
                                                     _bstr_t(strFullAccount.c_str()), _bstr_t(strNo.c_str()), &bstrMessage);
    else if (nType == 2)
        m_nCode = m_pSKOrderLib->CancelOrderByStockNo(_bstr_t(strLogInID.c_str()), VARIANT_BOOL(bAsyncOrder),
                                                      _bstr_t(strFullAccount.c_str()), _bstr_t(strNo.c_str()), &bstrMessage);

    cout << "CancelOrder : " << string(_bstr_t(bstrMessage)) << endl;

    ::SysFreeString(bstrMessage);

    return m_nCode;
}

long CSKOrderLib::CorrectPrice(string strLogInID, bool bAsyncOrder, int nMarket, int nType, string strNo, string strPrice, long nTradeType)
{
    string strFullAccount = "";
    string strMarketSymbol = "";
    long m_nCode = 0;

    if (nMarket == 0)
    {
        if (vec_strFullAccount_TS.size() > 0)
            strFullAccount = vec_strFullAccount_TS[0];
        else
        {
            cout << "CorrectPrice Error : No Stock Account.";
            return -1;
        }

        strMarketSymbol = "TS";
    }
    else if (nMarket == 1 || nMarket == 2)
    {
        if (vec_strFullAccount_TF.size() > 0)
            strFullAccount = vec_strFullAccount_TF[0];
        else
        {
            cout << "CorrectPrice Error : No Future Account.";
            return -1;
        }

        if (nMarket == 1)
            strMarketSymbol = "TF";
        else
            strMarketSymbol = "TO";
    }

    BSTR bstrMessage = ::SysAllocString(L"");

    if (nType == 0)
        m_nCode = m_pSKOrderLib->CorrectPriceBySeqNo(_bstr_t(strLogInID.c_str()), VARIANT_BOOL(bAsyncOrder),
                                                     _bstr_t(strFullAccount.c_str()), _bstr_t(strNo.c_str()), _bstr_t(strPrice.c_str()), nTradeType, &bstrMessage);
    else if (nType == 1)
        m_nCode = m_pSKOrderLib->CorrectPriceByBookNo(_bstr_t(strLogInID.c_str()), VARIANT_BOOL(bAsyncOrder),
                                                      _bstr_t(strFullAccount.c_str()), _bstr_t(strMarketSymbol.c_str()), _bstr_t(strNo.c_str()), _bstr_t(strPrice.c_str()), nTradeType, &bstrMessage);

    cout << "CorrectPrice : " << string(_bstr_t(bstrMessage)) << endl;

    ::SysFreeString(bstrMessage);

    return m_nCode;
}

long CSKOrderLib::DecreaseOrder(string strLogInID, bool bAsyncOrder, int nMarket, string strNo, long nDecreaseQty)
{
    string strFullAccount = "";
    long m_nCode = 0;

    if (nMarket == 0)
    {
        if (vec_strFullAccount_TS.size() > 0)
            strFullAccount = vec_strFullAccount_TS[0];
        else
        {
            cout << "DecreaseOrder Error : No Stock Account.";
            return -1;
        }
    }
    else if (nMarket == 1 || nMarket == 2)
    {
        if (vec_strFullAccount_TF.size() > 0)
            strFullAccount = vec_strFullAccount_TF[0];
        else
        {
            cout << "DecreaseOrder Error : No Future Account.";
            return -1;
        }
    }

    BSTR bstrMessage;
    m_nCode = m_pSKOrderLib->DecreaseOrderBySeqNo(_bstr_t(strLogInID.c_str()), VARIANT_BOOL(bAsyncOrder),
                                                  _bstr_t(strFullAccount.c_str()), _bstr_t(strNo.c_str()), nDecreaseQty, &bstrMessage);
    cout << "DecreaseOrder : " << string(_bstr_t(bstrMessage)) << endl;

    ::SysFreeString(bstrMessage);

    return m_nCode;
}

// Event
void CSKOrderLib::OnAccount(string strLoginID, string strAccountData)
{
    cout << "OnAccount ID = " + strLoginID + "  Account=" + strAccountData << endl;
    vector<string> vec_strValues;
    string strTemp;

    while (true)
    {
        strTemp = strAccountData.find(",") == -1 ? strAccountData : strAccountData.substr(0, strAccountData.find(","));

        vec_strValues.push_back(strTemp);

        if (strAccountData.find(",") == -1)
            break;

        strAccountData = strAccountData.substr(strAccountData.find(",") + 1, strAccountData.length() - 1);
    }

    if (vec_strValues.size() >= 7 && vec_strValues[0] == "TS")
    {
        string strFullAccount_TS = vec_strValues[1] + vec_strValues[3];
        vec_strFullAccount_TS.push_back(strFullAccount_TS);
        printf(" iOnAccount jTSFullAccount=%s\n", strFullAccount_TS.c_str());
    }
    else if (vec_strValues.size() >= 7 && vec_strValues[0] == "TF")
    {
        string strFullAccount_TF = vec_strValues[1] + vec_strValues[3];
        vec_strFullAccount_TF.push_back(strFullAccount_TF);
        printf(" iOnAccount jTFFullAccount=%s\n", strFullAccount_TF.c_str());
    }
    else if (vec_strValues.size() >= 7 && vec_strValues[0] == "OF")
    {
        string strFullAccount_OF = vec_strValues[1] + vec_strValues[3];
        vec_strFullAccount_OF.push_back(strFullAccount_OF);
        printf(" iOnAccount jOFFullAccount=%s\n", strFullAccount_OF.c_str());
    }
    else
    {
    }
}

void CSKOrderLib::OnFutureRights(BSTR bstrData)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    string strMessage = string(_bstr_t(bstrData));

    cout << "OnFutureRights : " << endl;
    cout << "Message : " << strMessage;

    cout << endl;

    // CalculateLoss();

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

void CSKOrderLib::OnAsyncOrder(long nThreadID, long nCode, string strMessage)
{
    cout << "On AsyncOrder ThreadID : " << nThreadID << ", nCode : " << nCode << ", Message : " << strMessage;
    cout << endl;
}
