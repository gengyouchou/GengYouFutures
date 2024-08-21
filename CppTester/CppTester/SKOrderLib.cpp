#include "SKOrderLib.h"
#include "SKCenterLib.h"

#include <Logger.h>
#include <string>
#include <vector>

#include <iostream>
#include <sstream>

using namespace std;

// Format 1:

// 1. Market Type
// 2. Account Number
// 3. Product
// 4. Buy/Sell Indicator
// 5. Open Position
// 6. Day Trading Open Position
// 7. Average Cost (Decimal Part Processed)
// 8. Commission per Contract
// 9. Transaction Tax (Ten-thousandths of X)
// 10.LOGIN_ID

OpenInterestInfo gOpenInterestInfo = {
    "",  // product
    "",  // Buy/Sell Indicator
    0,   // openPosition 0
    0,   // dayTradePosition 0
    0.0, // avgCost 0.0
    0.0  // profitAndLoss
};

string g_strUserId = "";
string gPwd = "";

void ParseOpenInterestMessage(const std::string &strMessage);

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
    case 4:
    {
        if (pdispparams->cArgs == 1)
        {
            BSTR bstrData = pdispparams->rgvarg[0].bstrVal;
            OnOpenInterest(bstrData);
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
//     BSTR bstrFullAccount; // 7
//     BSTR bstrStockNo;     //
//     SHORT sTradeType;     // 0:ROD 3:IOC 4:FOK ,sTradeTypeRODnOrderPriceType
//     SHORT sBuySell;  // 0: 1:
//     SHORT sDayTrade; // 0: 1:
//     SHORT sNewClose; // 0: 1: 2:
//     BSTR bstrPrice;  // ()[OCO]
//                      // nOrderPriceTypeP ,{}
//     BSTR bstrPrice2; //[OCO]
//     LONG nQty;           //
//     BSTR bstrTrigger;    // [OCO]{MITOCO0P}
//     BSTR bstrTrigger2; //[OCO]

//     BSTR bstrMovingPoint; //{}
//     SHORT sReserved;      // 0:(TT+1)1:T{MIT }
//     BSTR bstrDealPrice;   //  {MIT0, }

//     BSTR bstrSettlementMonth; // YYYYMM6(EX: 202206)
//     LONG nOrderPriceType;     //   2: ; 3:
//                               // sTradeTypeRODnOrderPriceType
//     LONG nTriggerDirection;   //{MIT} 1:GTE, 2:LTE
// };
//  : TX00 MTX00bstrSettlementMonth

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
    // pFutures.bstrPrice = _bstr_t(strPrice.c_str()).Detach();
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

long CSKOrderLib::GetOpenInterest(
    string strLogInID,
    long nFormat)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "Start");

    string strFullAccount_TF = "";

    if (vec_strFullAccount_TF.size() > 0)
    {
        strFullAccount_TF = vec_strFullAccount_TF[0];
    }
    else
    {
        cout << "GetOpenInterest Error : No Future Account.";
        return -1;
    }

    BSTR bstrLogInID = _bstr_t(strLogInID.c_str());
    BSTR bstrAccount = _bstr_t(strFullAccount_TF.c_str());

    long m_nCode = m_pSKOrderLib->GetOpenInterestGW(bstrLogInID, bstrAccount, nFormat);

    DEBUG(DEBUG_LEVEL_DEBUG, "GetOpenInterestGW=%ld", m_nCode);

    DEBUG(DEBUG_LEVEL_DEBUG, "End");

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

    DEBUG(DEBUG_LEVEL_INFO, "%s", strMessage);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

void CSKOrderLib::OnAsyncOrder(long nThreadID, long nCode, string strMessage)
{
    cout << "On AsyncOrder ThreadID : " << nThreadID << ", nCode : " << nCode << ", Message : " << strMessage;
    cout << endl;
}

// ,
// 1:
// 1
//
// 2
//
// 3
//
// 4
//
// 5
//
// 6
//
// 7
//  ()
// 8
//
// 9
//  (X)
// 10
//  LOGIN_ID

void CSKOrderLib::OnOpenInterest(IN BSTR bstrData)
{
    DEBUG(DEBUG_LEVEL_DEBUG, "start");

    string strMessage = string(_bstr_t(bstrData));

    DEBUG(DEBUG_LEVEL_DEBUG, "strMessage=%s", strMessage);

    ParseOpenInterestMessage(strMessage);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

void ParseOpenInterestMessage(const std::string &strMessage)
{
    std::string message = strMessage.substr(strMessage.find('=') + 1); //  [OnOpenInterest] strMessage=
    std::vector<std::string> items;
    std::stringstream ss(message);
    std::string item;

    // [OnEventFiringObjectInvoke] dispidMember == 4
    // [OnOpenInterest] strMessage=TF,F0200006358844,TM08,S,1,0,21236.00,,,F129305651
    // [OnEventFiringObjectInvoke] dispidMember == 4
    // [OnOpenInterest] strMessage=##,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,

    while (std::getline(ss, item, ','))
    {
        items.push_back(item);
    }

    std::string UnKnowMarket = "##";

    if (!items.empty() && items[0] == UnKnowMarket)
    {
        return;
    }

    if (items.size() >= 7)
    {
        gOpenInterestInfo.product = items[2];                     // 3
        gOpenInterestInfo.buySell = items[3];                     // 4
        gOpenInterestInfo.openPosition = std::stol(items[4]);     // 5
        gOpenInterestInfo.dayTradePosition = std::stol(items[5]); // 6
        gOpenInterestInfo.avgCost = std::stod(items[6]);          // 7

        LOG(DEBUG_LEVEL_DEBUG, "product: %s", gOpenInterestInfo.product);
        LOG(DEBUG_LEVEL_DEBUG, "buySell: %s", gOpenInterestInfo.buySell);
        LOG(DEBUG_LEVEL_DEBUG, "openPosition: %ld", gOpenInterestInfo.openPosition);
        LOG(DEBUG_LEVEL_DEBUG, "dayTradePosition: %ld", gOpenInterestInfo.dayTradePosition);
        LOG(DEBUG_LEVEL_DEBUG, "avgCost: %f", gOpenInterestInfo.avgCost);
    }
    else
    {
        gOpenInterestInfo = {
            "",  // product
            "",  // Buy/Sell Indicator
            0,   // openPosition 0
            0,   // dayTradePosition 0
            0.0, // avgCost 0.0
            0.0  // profitAndLoss
        };

        DEBUG(DEBUG_LEVEL_DEBUG, "NO Open Position: %s", strMessage);
    }
}