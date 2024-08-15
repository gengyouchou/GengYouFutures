#include <iostream>
#include <string>
#include <windows.h>
#include <ctime>
#include <filesystem>

void CloseProcessByPath(const std::wstring &processPath)
{
    // 构建 WMIC 命令来终止指定路径的进程
    std::wstring command = L"wmic process where ExecutablePath='" + processPath + L"' delete";
    _wsystem(command.c_str());
    std::wcout << L"Attempted to close process at: " << processPath << std::endl;
}

bool IsTimeToClose()
{
    time_t now = time(0);
    tm localTime;
    localtime_s(&localTime, &now);

    // Check if current time is 8:30 AM or 3:00 PM
    if ((localTime.tm_hour == 8 && localTime.tm_min == 30) ||
        (localTime.tm_hour == 15 && localTime.tm_min == 0) ||
        (localTime.tm_hour == 19 && localTime.tm_min == 48))
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
    std::wstring processPath = currentDrive.wstring() + L"\\GengYouFutures\\CppTester\\x64\\Debug\\CppTester.exe";
    int printCounter = 0;

    while (true)
    {
        if (IsTimeToClose())
        {
            std::wcout << L"Time's up. Attempting to close process at: " << processPath << std::endl;
            CloseProcessByPath(processPath);
            Sleep(60000); // Wait 1 minute to avoid multiple closures within the same minute
        }

        if (++printCounter >= 2) // Print time every 10 seconds
        {
            PrintCurrentTime();
            printCounter = 0;
        }

        Sleep(5000); // Check every 5 seconds
    }
    return 0;
}
