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

void TerminateProcessByPath(const std::wstring &processPath)
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
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
                if (hProcess)
                {
                    wchar_t currentProcessPath[MAX_PATH];
                    if (GetModuleFileNameExW(hProcess, NULL, currentProcessPath, MAX_PATH))
                    {
                        if (processPath == currentProcessPath)
                        {
                            if (TerminateProcess(hProcess, 0))
                            {
                                std::wcout << L"Successfully terminated process: " << pe32.szExeFile << L" Path: " << processPath << std::endl;
                            }
                            else
                            {
                                std::wcerr << L"Failed to terminate process: " << pe32.szExeFile << L" Path: " << processPath << std::endl;
                            }
                        }
                    }
                    CloseHandle(hProcess);
                }
            } while (Process32Next(hProcessSnap, &pe32));
        }
        CloseHandle(hProcessSnap);
    }
}

bool IsTimeToClose()
{
    time_t now = time(0);
    tm localTime;
    localtime_s(&localTime, &now);

    if ((localTime.tm_hour == 8 && localTime.tm_min == 00) ||
        (localTime.tm_hour == 15 && localTime.tm_min == 0))
    {
        return true;
    }

    return false;
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
    std::wstring processPath = currentDrive.wstring() + L"\\GengYouFutures\\CppTester\\x64\\Release\\CppTester.exe";
    int printCounter = 0;

    while (true)
    {
        if (IsTimeToClose())
        {
            std::wcout << L"Time's up. Attempting to terminate process at: " << processPath << std::endl;
            // PrintAllProcesses();
            TerminateProcessByPath(processPath);
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
