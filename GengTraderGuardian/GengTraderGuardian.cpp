#include <filesystem> // C++17 filesystem library for relative paths
#include <iostream>
#include <string>

#include <windows.h>

#include <tlhelp32.h>

// Convert CHAR array to std::wstring
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
                std::wstring exeFile = CharToWstring(pe32.szExeFile); // Convert CHAR array to std::wstring
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

bool RestartProcess(const std::wstring &processPath)
{
    std::wcerr << L"Attempting to restart at path: " << processPath << std::endl;

    // Ensure the process is started
    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi;
    if (CreateProcessW(processPath.c_str(), NULL, NULL, NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &si, &pi))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true; // Process started successfully
    }
    return false; // Failed to start the process
}

int main()
{
    std::wstring processName = L"CppTester.exe";

    // get current root
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::wstring driveLetter = currentPath.root_name().wstring();

    std::filesystem::path processPath = driveLetter + L"\\GengYouFutures\\CppTester\\x64\\Debug\\" + processName;

    while (true)
    {
        DWORD processID = GetProcessIDByName(processName);
        if (processID == 0)
        {
            // Process is not running, restart it
            if (RestartProcess(processPath.wstring()))
            {
                std::wcout << L"Restarted " << processName << std::endl;
            }
            else
            {
                std::wcerr << L"Failed to restart " << processName << std::endl;
            }
        }
        Sleep(50000); // Check every 5 seconds
    }

    return 0;
}
