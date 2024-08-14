@echo off
setlocal enabledelayedexpansion

:: 获取当前盘符
set "currentDrive=%~d0"

:: 设置程序路径
set "cppTesterDir=%currentDrive%GengYouFutures\CppTester\x64\Debug"
set "cppTesterPath=%cppTesterDir%\CppTester.exe"
set "gengTraderGuardianPath=%currentDrive%GengYouFutures\GengTraderGuardian\GengTraderGuardian.exe"

:: 输出路径信息，调试用
echo Current Drive: %currentDrive%
echo CppTester Directory: %cppTesterDir%
echo CppTester Path: %cppTesterPath%

:restartCppTester
:: 检查CppTester.exe是否在运行
tasklist /FI "IMAGENAME eq CppTester.exe" | find /I "CppTester.exe" >nul
if errorlevel 1 (
    echo CppTester.exe is not running. Restarting...
    :: 更改工作目录为CppTester.exe所在目录
    cd /d "%cppTesterDir%"
    echo Changing directory to %cppTesterDir%
    :: 启动CppTester.exe
    start "" "%cppTesterPath%"
    :: 输出启动信息
    echo Started %cppTesterPath%
)

:: 每10秒检查一次
timeout /t 10 /nobreak >nul
goto restartCppTester

:: 每天早上8:30和下午2:30重启CppTester.exe
:restartDaily
call :restartAtTime "08:30:00" "Morning restart"
call :restartAtTime "14:30:00" "Afternoon restart"
goto restartCppTester

:restartAtTime
:: 参数1: 目标时间 (格式: HH:MM:SS)
:: 参数2: 重启原因
set "now=%time%"
set "target=%~1"

:: 比较时间
for /F "tokens=1-4 delims=:." %%a in ("%now%") do set /A nowSeconds=(((%%a*60)+%%b)*60)+%%c
for /F "tokens=1-4 delims=:." %%a in ("%target%") do set /A targetSeconds=(((%%a*60)+%%b)*60)+%%c

set /A waitSeconds=targetSeconds-nowSeconds

if %waitSeconds% LEQ 0 (
    :: 计算负数等待时间，表示已经过了目标时间，需要等到第二天
    set /A waitSeconds=86400-nowSeconds+targetSeconds
)

echo Waiting %waitSeconds% seconds until %target% (%~2)
timeout /t %waitSeconds% /nobreak >nul

:: 重启CppTester.exe
echo Restarting CppTester.exe at %target% due to %~2...
cd /d "%cppTesterDir%"
echo Changing directory to %cppTesterDir%
start "" "%cppTesterPath%"

exit /b
