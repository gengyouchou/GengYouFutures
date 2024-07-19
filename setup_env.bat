@echo off
REM 设置 Visual Studio Build Tools 环境变量
REM 根据你的 Visual Studio 版本和安装路径修改路径

REM 设置编译器路径
set "VS_VERSION=2019"  REM 设置 Visual Studio 版本
set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\%VS_VERSION%\Community\VC\Auxiliary\Build"

REM 设置 Windows Kits 路径
set "WINDOWS_KITS_PATH=C:\Program Files (x86)\Windows Kits\10"

REM 设置环境变量
echo Setting up Visual Studio environment...
call "%VS_PATH%\vcvarsall.bat" x64

REM 添加 Windows Kits 的 include 和 lib 路径
set "INCLUDE=%WINDOWS_KITS_PATH%\Include\10.0.19041.0\ucrt;%WINDOWS_KITS_PATH%\Include\10.0.19041.0\shared;%INCLUDE%"
set "LIB=%WINDOWS_KITS_PATH%\Lib\10.0.19041.0\ucrt\x64;%WINDOWS_KITS_PATH%\Lib\10.0.19041.0\um\x64;%LIB%"

REM 确保 CMakeLists.txt 文件存在于指定目录
set "SOURCE_DIR=C:\GengYouFutures\CppTester"
set "BUILD_DIR=C:\GengYouFutures\CppTester\build"

if exist "%SOURCE_DIR%\CMakeLists.txt" (
    echo CMakeLists.txt found in %SOURCE_DIR%. Running CMake...
    cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%"
) else (
    echo Error: CMakeLists.txt not found in %SOURCE_DIR%.
)

REM 提示用户按任意键继续
echo.
echo Press any key to continue...
pause >nul
