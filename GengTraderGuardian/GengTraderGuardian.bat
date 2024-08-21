@echo off
setlocal enabledelayedexpansion

:: 获取当前盘符
set "currentDrive=%~d0"

:: 设置程序路径
set "cppTesterDir=%currentDrive%GengYouFutures\CppTester\x64\Release"
set "cppTesterPath=%cppTesterDir%\CppTester.exe"
set "gengTraderGuardianDir=%currentDrive%GengYouFutures\GengTraderGuardian\x64\Release"
set "gengTraderGuardianPath=%gengTraderGuardianDir%\GengTraderGuardian.exe"

:: 输出路径信息，调试用
echo Current Drive: %currentDrive%
echo CppTester Directory: %cppTesterDir%
echo CppTester Path: %cppTesterPath%
echo GengTraderGuardian Directory: %gengTraderGuardianDir%
echo GengTraderGuardian Path: %gengTraderGuardianPath%

:checkProcesses
:: 检查CppTester.exe是否在运行
tasklist /FI "IMAGENAME eq CppTester.exe" | find /I "CppTester.exe" >nul
if errorlevel 1 (
    echo CppTester.exe is not running. Restarting...
    :: 直接启动CppTester.exe
    start "" "%cppTesterPath%"
    :: 输出启动信息
    echo Started %cppTesterPath%
)

:: 检查GengTraderGuardian.exe是否在运行
tasklist /FI "IMAGENAME eq GengTraderGuardian.exe" | find /I "GengTraderGuardian.exe" >nul
if errorlevel 1 (
    echo GengTraderGuardian.exe is not running. Restarting...
    :: 直接启动GengTraderGuardian.exe
    start "" "%gengTraderGuardianPath%"
    :: 输出启动信息
    echo Started %gengTraderGuardianPath%
)

:: 每10秒检查一次
timeout /t 10 /nobreak >nul
goto checkProcesses
