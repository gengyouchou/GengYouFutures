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
    0.0, // profitAndLoss
    TRUE // NeedToUpdate
};

string g_strUserId = "";
string gPwd = "";

double gFloatingProfitLoss = 0.0;

void ParseOpenInterestMessage(const std::string &strMessage);
void ParseOnFutureRightsMessage(const std::string &strMessage);

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

    ParseOnFutureRightsMessage(strMessage);

    DEBUG(DEBUG_LEVEL_DEBUG, "end");
}

void CSKOrderLib::OnAsyncOrder(long nThreadID, long nCode, string strMessage)
{
    cout << "On AsyncOrder ThreadID : " << nThreadID << ", nCode : " << nCode << ", Message : " << strMessage;
    cout << endl;
}

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
    // or
    // M003 NO DATA#

    while (std::getline(ss, item, ','))
    {
        items.push_back(item);
    }

    std::string UnKnowMarket = "##";

    if (!items.empty() && items[0] == UnKnowMarket)
    {
        LOG(DEBUG_LEVEL_DEBUG, "Message: %s", strMessage);
        gOpenInterestInfo.NeedToUpdate = FALSE;
        return;
    }

    if (items.size() >= 7)
    {
        gOpenInterestInfo.product = items[2];                     // 3
        gOpenInterestInfo.buySell = items[3];                     // 4
        gOpenInterestInfo.openPosition = std::stol(items[4]);     // 5
        gOpenInterestInfo.dayTradePosition = std::stol(items[5]); // 6
        gOpenInterestInfo.avgCost = std::stod(items[6]);          // 7
        gOpenInterestInfo.NeedToUpdate = FALSE;

        if (gOpenInterestInfo.buySell == "S")
        {
            gOpenInterestInfo.openPosition = -gOpenInterestInfo.openPosition;
        }

        LOG(DEBUG_LEVEL_DEBUG, "product: %s", gOpenInterestInfo.product);
        LOG(DEBUG_LEVEL_DEBUG, "buySell: %s", gOpenInterestInfo.buySell);
        LOG(DEBUG_LEVEL_DEBUG, "openPosition: %ld", gOpenInterestInfo.openPosition);
        LOG(DEBUG_LEVEL_DEBUG, "dayTradePosition: %ld", gOpenInterestInfo.dayTradePosition);
        LOG(DEBUG_LEVEL_DEBUG, "avgCost: %f", gOpenInterestInfo.avgCost);
        LOG(DEBUG_LEVEL_DEBUG, "NeedToUpdate = FALSE");
    }
    else
    {
        gOpenInterestInfo = {
            "",   // product
            "",   // Buy/Sell Indicator
            0,    // openPosition 0
            0,    // dayTradePosition 0
            0.0,  // avgCost 0.0
            0.0,  // profitAndLoss
            FALSE // NeedToUpdate
        };

        DEBUG(DEBUG_LEVEL_DEBUG, "NO Open Position: %s", strMessage);
    }
}

/**
 * @brief Parses a comma-separated string of account data.
 *
 * The input string contains multiple fields related to account and trading data,
 * separated by commas. The function will parse these fields and process them.
 *
 * The fields in the string are as follows:
 *
 * 0  - Account Balance (double): Total balance in the account.
 * 1  - Floating Profit/Loss (double): Current floating profit or loss.
 * 2  - Realized Expenses (double): Expenses that have been realized.
 * 3  - Transaction Tax (double): Tax applied to transactions.
 * 4  - Withheld Premium (double): Premium amount withheld.
 * 5  - Premium Payments (double): Payments made for premiums.
 * 6  - Equity (double): Total equity in the account.
 * 7  - Excess Margin (double): Margin amount exceeding the required level.
 * 8  - Deposits/Withdrawals (double): Total amount of deposits and withdrawals.
 * 9  - Buyer Market Value (double): Market value of positions bought.
 * 10 - Seller Market Value (double): Market value of positions sold.
 * 11 - Futures Closing Profit/Loss (double): Profit or loss from futures positions closed.
 * 12 - Unrealized Intraday (double): Intraday unrealized profit or loss.
 * 13 - Initial Margin (double): Initial margin required.
 * 14 - Maintenance Margin (double): Margin required to maintain positions.
 * 15 - Position Initial Margin (double): Initial margin for specific positions.
 * 16 - Position Maintenance Margin (double): Maintenance margin for specific positions.
 * 17 - Order Margin (double): Margin tied up in orders.
 * 18 - Excess Optimal Margin (double): Optimal excess margin.
 * 19 - Total Option Value (double): Total value of options held.
 * 20 - Withheld Expenses (double): Expenses that have been withheld.
 * 21 - Initial Margin (double): Initial margin again for calculations.
 * 22 - Previous Day Balance (double): Account balance from the previous day.
 * 23 - Option Combination Margin Adjustment (double): Margin adjustment for option combinations.
 * 24 - Maintenance Ratio (double): Ratio of maintenance margin to required margin.
 * 25 - Currency (string): Currency type of the account.
 * 26 - Full Initial Margin (double): Full amount required for initial margin.
 * 27 - Full Maintenance Margin (double): Full amount required for maintenance margin.
 * 28 - Full Available (double): Fully available amount in the account.
 * 29 - Offset Amount (double): Amount that can offset positions.
 * 30 - Valuable Available (double): Valuable amount available.
 * 31 - Available Balance (double): Total available balance in the account.
 * 32 - Full Cash Available (double): Fully available cash in the account.
 * 33 - Valuable Value (double): Value of valuable positions.
 * 34 - Risk Indicator (double): Indicator of account risk.
 * 35 - Option Expiry Difference (double): Difference at option expiry.
 * 36 - Option Expiry Profit/Loss (double): Profit or loss from expired options.
 * 37 - Futures Expiry Profit/Loss (double): Profit or loss from futures expiry.
 * 38 - Additional Margin (double): Additional margin required.
 * 39 - LOGIN_ID (string): Login ID for the account.
 * 40 - ACCOUNT_NO (string): Account number.
 *
 * @param strMessage A comma-separated string containing the account data fields.
 */
void ParseOnFutureRightsMessage(const std::string &strMessage)
{
    // Split the string by commas
    std::stringstream ss(strMessage);
    std::string token;
    std::vector<std::string> fields;

    while (std::getline(ss, token, ','))
    {
        fields.push_back(token);
    }

    // if (fields.size() != 41)
    // {
    //     std::cerr << "Error: Expected 41 fields, but got " << fields.size() << std::endl;
    //     return;
    // }

    double accountBalance = std::stod(fields[0]);
    double floatingPL = std::stod(fields[1]);
    // double realizedExpenses = std::stod(fields[2]);
    // double transactionTax = std::stod(fields[3]);
    // double withheldPremium = std::stod(fields[4]);
    // double premiumPayments = std::stod(fields[5]);
    // double equity = std::stod(fields[6]);
    // double excessMargin = std::stod(fields[7]);
    // double depositsWithdrawals = std::stod(fields[8]);
    // double buyerMarketValue = std::stod(fields[9]);
    // double sellerMarketValue = std::stod(fields[10]);
    double futuresClosingPL = std::stod(fields[11]);
    // double unrealizedIntraday = std::stod(fields[12]);
    // double initialMargin = std::stod(fields[13]);
    // double maintenanceMargin = std::stod(fields[14]);
    // double positionInitialMargin = std::stod(fields[15]);
    // double positionMaintenanceMargin = std::stod(fields[16]);
    // double orderMargin = std::stod(fields[17]);
    // double excessOptimalMargin = std::stod(fields[18]);
    // double totalOptionValue = std::stod(fields[19]);
    // double withheldExpenses = std::stod(fields[20]);
    // double secondInitialMargin = std::stod(fields[21]); // Second occurrence of Initial Margin
    // double previousDayBalance = std::stod(fields[22]);
    // double optionCombinationMarginAdjustment = std::stod(fields[23]);
    // double maintenanceRatio = std::stod(fields[24]);
    // std::string currency = fields[25];
    // double fullInitialMargin = std::stod(fields[26]);
    // double fullMaintenanceMargin = std::stod(fields[27]);
    // double fullAvailable = std::stod(fields[28]);
    // double offsetAmount = std::stod(fields[29]);
    // double valuableAvailable = std::stod(fields[30]);
    // double availableBalance = std::stod(fields[31]);
    // double fullCashAvailable = std::stod(fields[32]);
    // double valuableValue = std::stod(fields[33]);
    // double riskIndicator = std::stod(fields[34]);
    // double optionExpiryDifference = std::stod(fields[35]);
    // double optionExpiryPL = std::stod(fields[36]);
    // double futuresExpiryPL = std::stod(fields[37]);
    // double additionalMargin = std::stod(fields[38]);
    // std::string loginID = fields[39];
    // std::string accountNo = fields[40];

    DEBUG(DEBUG_LEVEL_DEBUG, "Account Balance:%f", accountBalance);
    DEBUG(DEBUG_LEVEL_DEBUG, "Floating P/L:%f", floatingPL);
    DEBUG(DEBUG_LEVEL_DEBUG, "futuresClosingPL:%f", futuresClosingPL);

    gFloatingProfitLoss = min(floatingPL, futuresClosingPL);
}
