#pragma once

// EXPORT 宏，如果你在 Windows 下用 DLL，可視情況調整
#ifdef _WIN32
#ifdef STRATEGYOF_EXPORTS
#define SOF_API __declspec(dllexport)
#else
#define SOF_API __declspec(dllimport)
#endif
#else
#define SOF_API
#endif

#include <string>

extern "C"
{
    // 1. 初始化：登入 + 啟用報價
    SOF_API void SOF_Initialize(
        const wchar_t *userId,
        const wchar_t *password);

    // 2. 註冊 COM 事件回報（只需呼叫一次）
    SOF_API void SOF_RegisterEvents();

    // 3. 執行一次停損停利檢查並下 OCO
    //    marketNo: 交易所代號 (e.g. L"CME")
    //    prodCode: 商品代號   (e.g. L"ES0000")
    SOF_API void SOF_ManageSLTP(
        const wchar_t *marketNo,
        const wchar_t *prodCode);

    // 4. 清理 COM 資源
    SOF_API void SOF_Shutdown();
}
