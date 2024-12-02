#include <windows.h>
#include <winhttp.h>
#include <iostream>
#include <string>
#include <thread>
#include <cstdlib>
#include <ctime>

#pragma comment(lib, "winhttp.lib")

double generateRandomPrice(double currentPrice)
{
    double fluctuation = (rand() % 21 - 10) / 10.0;
    return currentPrice + fluctuation;
}

bool sendData(HINTERNET hConnect, LPCWSTR endpoint, const std::string &jsonData)
{
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", endpoint,
                                            NULL, WINHTTP_NO_REFERER,
                                            WINHTTP_DEFAULT_ACCEPT_TYPES,
                                            WINHTTP_FLAG_REFRESH);
    if (!hRequest)
    {
        std::cerr << "WinHttpOpenRequest failed, error: " << GetLastError() << std::endl;
        return false;
    }

    BOOL result = WinHttpAddRequestHeaders(hRequest, L"Content-Type: application/json", -1L, WINHTTP_ADDREQ_FLAG_ADD);
    if (!result)
    {
        std::cerr << "Failed to set request headers, error: " << GetLastError() << std::endl;
    }

    result = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                (LPVOID)jsonData.c_str(), (DWORD)jsonData.size(),
                                (DWORD)jsonData.size(), 0);
    if (!result)
    {
        std::cerr << "WinHttpSendRequest failed, error: " << GetLastError() << std::endl;
        WinHttpCloseHandle(hRequest);
        return false;
    }

    result = WinHttpReceiveResponse(hRequest, NULL);
    if (result)
    {
        std::cout << "Data sent successfully: " << jsonData << std::endl;
    }
    else
    {
        std::cerr << "WinHttpReceiveResponse failed, error: " << GetLastError() << std::endl;
    }

    WinHttpCloseHandle(hRequest);
    return result;
}

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    LPCWSTR serverName = L"localhost";
    INTERNET_PORT serverPort = 8080;
    LPCWSTR endpoint = L"/futures";

    HINTERNET hSession = WinHttpOpen(L"Futures Client/1.0",
                                     WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                     WINHTTP_NO_PROXY_NAME,
                                     WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession)
    {
        std::cerr << "WinHttpOpen failed, error: " << GetLastError() << std::endl;
        return 1;
    }

    HINTERNET hConnect = WinHttpConnect(hSession, serverName, serverPort, 0);
    if (!hConnect)
    {
        std::cerr << "WinHttpConnect failed, error: " << GetLastError() << std::endl;
        WinHttpCloseHandle(hSession);
        return 1;
    }

    double currentPrice = 1234.56;
    while (true)
    {
        currentPrice = generateRandomPrice(1);

        std::string jsonData = R"({"time": ")" + std::to_string(time(0)) + R"(", "price": )" +
                               std::to_string(currentPrice) + R"(, "volume": 100, "symbol": "FUTURE1"})";

        if (!sendData(hConnect, endpoint, jsonData))
        {
            break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return 0;
}
