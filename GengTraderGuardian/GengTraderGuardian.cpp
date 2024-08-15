#include <iostream>
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <algorithm>
#include <ctime>
#include <filesystem>

void PrintAllProcesses()
{
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hProcessSnap, &pe32))
        {
            do
            {
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
                if (hProcess)
                {
                    wchar_t processPath[MAX_PATH];
                    if (GetModuleFileNameExW(hProcess, NULL, processPath, MAX_PATH)) // 使用 Unicode 版本
                    {
                        std::wcout << L"Process: " << pe32.szExeFile << L" Path: " << processPath << std::endl;
                    }
                    CloseHandle(hProcess);
                }
            } while (Process32Next(hProcessSnap, &pe32));
        }
        CloseHandle(hProcessSnap);
    }
}

void CloseProcessByPath(const std::wstring &processPath)
{
    std::wstring modifiedPath = processPath;
    std::replace(modifiedPath.begin(), modifiedPath.end(), L'\\', L'/');

    std::wstring command = L"wmic process where ExecutablePath='" + modifiedPath + L"' delete";
    _wsystem(command.c_str());
    std::wcout << L"Attempted to close process at: " << processPath << std::endl;
}

bool IsTimeToClose()
{
    time_t now = time(0);
    tm localTime;
    localtime_s(&localTime, &now);

    // if ((localTime.tm_hour == 8 && localTime.tm_min == 30) ||
    //     (localTime.tm_hour == 15 && localTime.tm_min == 0) ||
    //     (localTime.tm_hour == 20 && localTime.tm_min == 1))
    // {
    //     return true;
    // }
    // return false;

    return true;
}

void PrintCurrentTime()
{
    time_t now = time(0);
    tm localTime;
    localtime_s(&localTime, &now);
    std::wcout << L"Current time: "
               << localTime.tm_hour << L":"
               << localTime.tm_min << L":"
               << localTime.tm_sec << std::endl;
}

int main()
{
    std::filesystem::path currentDrive = std::filesystem::current_path().root_name();
    std::wstring processPath = currentDrive.wstring() + L"\\GengYouFutures\\CppTester\\x64\\Debug\\CppTester.exe";
    int printCounter = 0;

    while (true)
    {
        if (IsTimeToClose())
        {
            std::wcout << L"Time's up. Attempting to close process at: " << processPath << std::endl;
            PrintAllProcesses();
            CloseProcessByPath(processPath);
            Sleep(60000);
        }

        if (++printCounter >= 2)
        {
            PrintCurrentTime();
            printCounter = 0;
        }

        Sleep(5000);
    }
    return 0;
}
