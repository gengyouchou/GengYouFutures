#include <filesystem>
#include <iostream>
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include <ctime>

std::wstring CharToWstring(const char *charArray)
{
    int size = MultiByteToWideChar(CP_ACP, 0, charArray, -1, NULL, 0);
    std::wstring wideString(size, 0);
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, &wideString[0], size);
    return wideString;
}

DWORD GetProcessIDByName(const std::wstring &processName)
{
    DWORD processID = 0;
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hProcessSnap, &pe32))
        {
            do
            {
                std::wstring exeFile = CharToWstring(pe32.szExeFile);
                if (processName == exeFile)
                {
                    processID = pe32.th32ProcessID;
                    break;
                }
            } while (Process32Next(hProcessSnap, &pe32));
        }
        CloseHandle(hProcessSnap);
    }
    return processID;
}

void CloseProcess(const DWORD processID)
{
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processID);
    if (hProcess)
    {
        TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
    }
}

bool IsTimeToClose()
{
    time_t now = time(0);
    tm *localTime = localtime(&now);

    // Check if current time is 8:45 AM or 3:00 PM
    if ((localTime->tm_hour == 8 && localTime->tm_min == 30) ||
        (localTime->tm_hour == 14 && localTime->tm_min == 45))
    {
        return true;
    }
    return false;
}

int main()
{
    std::wstring processName = L"CppTester.exe";

    while (true)
    {
        DWORD processID = GetProcessIDByName(processName);
        if (IsTimeToClose())
        {
            if (processID != 0)
            {
                CloseProcess(processID);
                std::wcout << L"Closed " << processName << L" at the designated time" << std::endl;
            }
            Sleep(60000); // Wait 1 minute to avoid multiple closures within the same minute
        }
        Sleep(5000); // Check every 5 seconds
    }
    return 0;
}
