@echo off
REM 设置 Visual Studio 环境变量
REM 根据你的 Visual Studio 版本和安装路径修改路径

REM 设置编译器路径
set "VS_VERSION=2019"
set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\%VS_VERSION%\VC\Auxiliary\Build"

REM 确定 MSVC 版本号（需要根据实际安装的版本号进行调整）
set "MSVC_VERSION=14.29.30133"  REM 设置 MSVC 版本号

REM 修复路径问题，去掉多余的反斜杠
set "CL_PATH=%VS_PATH%\Tools\MSVC\%MSVC_VERSION%\bin\Hostx64\x64"
set "INCLUDE_PATH=%VS_PATH%\Tools\MSVC\%MSVC_VERSION%\include"
set "LIB_PATH=%VS_PATH%\Tools\MSVC\%MSVC_VERSION%\lib\x64"

REM 设置编译器环境变量
call "%VS_PATH%\vcvarsall.bat" x64

REM 打印当前环境变量设置
echo Visual Studio Build Tools environment variables set:
echo PATH: %PATH%
echo INCLUDE: %INCLUDE%
echo LIB: %LIB%

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
