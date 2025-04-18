#include "StrategyOF.h"
#include <windows.h>
#include <comdef.h>
#include <iostream>

// 引入策略王 COM TypeLib（路徑請依專案實際放置調整）
#import "CppTester/SKCenterLib.tlb" no_namespace named_guids
#import "CppTester/SKOrderLib.tlb" no_namespace named_guids
#import "CppTester/SKOSQuoteLib.tlb" no_namespace named_guids

// 停損／停利參數
static constexpr long STOP_LOSS_TWD = 8000;    // 每口停損台幣
static constexpr long TAKE_PROFIT_TWD = 16000; // 每口停利台幣

// 合約乘數：請依實際商品調整
inline long ContractSize() { return 50; }

// 全域 COM 物件指標
static ISKCenterLibPtr g_pCenter;
static ISKOrderLibPtr g_pOrder;
static ISKOSQuoteLibPtr g_pQuote;

// 狀態機
struct State
{
    bool waitingOpenInterest = false;
    bool waitingSmartReport = false;
    long lastQty = 0;
    double lastAvgCost = 0.0;
    bool hasExistingOCO = false;
    _bstr_t account;
    _bstr_t marketNo;
    _bstr_t prodCode;
} g_state;

// 回報：未平倉 (GW)
void __stdcall OnOFOpenInterestGWReport(const OFOPENINTERESTGWREPORT *rpt)
{
    if (!g_state.waitingOpenInterest)
        return;
    g_state.lastQty = rpt->nTotalQty;
    g_state.lastAvgCost = rpt->dAvgCostPrice;
    g_state.waitingOpenInterest = false;

    // 接著去查智慧單
    g_state.waitingSmartReport = true;
    g_pOrder->GetOFSmartStrategyReport(
        g_state.account,
        g_state.marketNo,
        g_state.prodCode,
        0, L"", L"");
}

// 回報：智慧單查詢
void __stdcall OnOFSmartStrategyReport(const OFSMARTSTRATEGYREPORT *rpt)
{
    if (!g_state.waitingSmartReport)
        return;
    if (rpt->nStrategyType == OF_STRATEGY_OCO &&
        _bstr_t(rpt->bstrProdCode) == g_state.prodCode)
    {
        g_state.hasExistingOCO = true;
    }
    if (rpt->nEndFlag == 1)
    {
        g_state.waitingSmartReport = false;
        // 有持倉且無既有 OCO，才下新單
        if (g_state.lastQty != 0 && !g_state.hasExistingOCO)
        {
            double dSL = STOP_LOSS_TWD / double(ContractSize());
            double dTP = TAKE_PROFIT_TWD / double(ContractSize());
            double stopP = g_state.lastAvgCost - dSL;
            double takeP = g_state.lastAvgCost + dTP;

            OVERSEAFUTUREORDER oco = {};
            oco.bstrAccount = g_state.account;
            oco.bstrMarketNo = g_state.marketNo;
            oco.bstrProdCode = g_state.prodCode;
            oco.nQty = g_state.lastQty;
            oco.sBuySell = (g_state.lastQty > 0
                                ? TRANSEORDER_SELL
                                : TRANSEORDER_BUY);
            oco.sPriceType = PRICE_LIMIT;
            oco.dPrice = takeP; // 停利限價
            oco.sOCOType = OF_STRATEGY_OCO;
            oco.sStopLossType = OF_STRATEGY_MARKET; // 市價停損
            oco.bAsyncOrder = VARIANT_TRUE;

            _bstr_t msg;
            long ret = g_pOrder->SendOverSeaFutureOCOOrder(&oco, &msg);
            std::wcout << L"[StrategyOF] SendOCO ret=" << ret
                       << L" msg=" << (wchar_t *)msg << std::endl;
        }
        // 重置狀態
        g_state.hasExistingOCO = false;
    }
}

extern "C" SOF_API void SOF_Initialize(
    const wchar_t *userId,
    const wchar_t *password)
{
    CoInitialize(NULL);
    g_pCenter = CLSID_SKCenterLib;
    g_pOrder = CLSID_SKOrderLib;
    g_pQuote = CLSID_SKOSQuoteLib;

    // 登入並啟用報價
    g_pCenter->LoginSetQuote(_bstr_t(userId), _bstr_t(password), L"Y");

    // 初始化下單元件並讀憑證
    g_pOrder->Initialize();
    g_pOrder->ReadCertByID(_bstr_t(userId));
}

extern "C" SOF_API void SOF_RegisterEvents()
{
    // 綁定回報
    g_pOrder->OnOFOpenInterestGWReport = OnOFOpenInterestGWReport;
    g_pOrder->OnOFSmartStrategyReport = OnOFSmartStrategyReport;
}

extern "C" SOF_API void SOF_ManageSLTP(
    const wchar_t *marketNo,
    const wchar_t *prodCode)
{
    // 設定狀態
    g_state.account = g_pCenter->GetLoginID();
    g_state.marketNo = marketNo;
    g_state.prodCode = prodCode;
    g_state.hasExistingOCO = false;
    g_state.waitingOpenInterest = true;

    // 查 GW 未平倉
    g_pOrder->GetOverseaFutureOpenInterestGW(
        g_state.account,
        g_state.account);
}

extern "C" SOF_API void SOF_Shutdown()
{
    CoUninitialize();
}
