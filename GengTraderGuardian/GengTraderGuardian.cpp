#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>

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
                std::wstring exeFile(pe32.szExeFile);
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

void RestartProcess(const std::wstring &processName, const std::wstring &processPath)
{
    DWORD processID = GetProcessIDByName(processName);
    if (processID != 0)
    {
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processID);
        if (hProcess != NULL)
        {
            TerminateProcess(hProcess, 0);
            CloseHandle(hProcess);
        }
    }
    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi;
    if (CreateProcessW(processPath.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

int main()
{
    std::wstring processName = L"XXX.exe";               // 要检测的进程名
    std::wstring processPath = L"C:\\Path\\To\\XXX.exe"; // 进程的完整路径

    while (true)
    {
        DWORD processID = GetProcessIDByName(processName);
        if (processID == 0)
        {
            // 进程不存在，重新启动
            RestartProcess(processName, processPath);
            std::wcout << L"Restarted " << processName << std::endl;
        }
        Sleep(5000); // 每5秒检查一次
    }

    return 0;
}
