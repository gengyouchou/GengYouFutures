#include <windows.h>
#include <winhttp.h>
#include <iostream>
#include <string>

#pragma comment(lib, "winhttp.lib")

int main()
{
    // 伺服器資訊
    LPCWSTR serverName = L"localhost";
    INTERNET_PORT serverPort = 8080;
    LPCWSTR endpoint = L"/futures";

    // 初始化 WinHTTP Session
    HINTERNET hSession = WinHttpOpen(L"Futures Client/1.0",
                                     WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                     WINHTTP_NO_PROXY_NAME,
                                     WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession)
    {
        std::cerr << "WinHttpOpen failed, error: " << GetLastError() << std::endl;
        return 1;
    }

    // 連接伺服器
    HINTERNET hConnect = WinHttpConnect(hSession, serverName, serverPort, 0);
    if (!hConnect)
    {
        std::cerr << "WinHttpConnect failed, error: " << GetLastError() << std::endl;
        WinHttpCloseHandle(hSession);
        return 1;
    }

    // 建立請求
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", endpoint,
                                            NULL, WINHTTP_NO_REFERER,
                                            WINHTTP_DEFAULT_ACCEPT_TYPES,
                                            WINHTTP_FLAG_REFRESH);
    if (!hRequest)
    {
        std::cerr << "WinHttpOpenRequest failed, error: " << GetLastError() << std::endl;
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return 1;
    }

    // 模擬發送的資料
    std::string jsonData = R"({"time": "2024-11-17 10:05", "price": 1234.56, "volume": 100, "symbol": "FUTURE1"})";

    // 設置標頭
    BOOL result = WinHttpAddRequestHeaders(hRequest, L"Content-Type: application/json", -1L, WINHTTP_ADDREQ_FLAG_ADD);
    if (!result)
    {
        std::cerr << "Failed to set request headers, error: " << GetLastError() << std::endl;
    }

    // 發送請求
    result = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                (LPVOID)jsonData.c_str(), jsonData.size(),
                                jsonData.size(), 0);
    if (!result)
    {
        std::cerr << "WinHttpSendRequest failed, error: " << GetLastError() << std::endl;
    }
    else
    {
        // 等待伺服器響應
        result = WinHttpReceiveResponse(hRequest, NULL);
        if (result)
        {
            std::cout << "Data sent successfully: " << jsonData << std::endl;
        }
        else
        {
            std::cerr << "WinHttpReceiveResponse failed, error: " << GetLastError() << std::endl;
        }
    }

    // 清理資源
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return 0;
}
