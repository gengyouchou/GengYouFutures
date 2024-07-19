@echo off
REM 设置 Visual Studio Build Tools 环境变量
REM 根据你的 Visual Studio 版本和安装路径修改路径

REM 设置编译器路径
set "VS_VERSION=2019"  REM 设置 Visual Studio 版本
set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\%VS_VERSION%\BuildTools\VC\Tools\MSVC"

REM 确定 MSVC 版本号（需要根据实际安装的版本号进行调整）
set "MSVC_VERSION=14.29.30133"  REM 设置 MSVC 版本号

set "CL_PATH=%VS_PATH%\%MSVC_VERSION%\bin\Hostx64\x64"
set "INCLUDE_PATH=%VS_PATH%\%MSVC_VERSION%\include"
set "LIB_PATH=%VS_PATH%\%MSVC_VERSION%\lib\x64"

REM 设置编译器环境变量
set "PATH=%CL_PATH%;%PATH%"
set "INCLUDE=%INCLUDE_PATH%"
set "LIB=%LIB_PATH%"

REM 打印当前环境变量设置
echo Visual Studio Build Tools environment variables set:
echo PATH: %PATH%
echo INCLUDE: %INCLUDE%
echo LIB: %LIB%

REM 运行 CMake 命令
cmake -S %CD% -B %CD%\build

REM 提示用户按任意键继续
echo.
echo Press any key to continue...
pause >nul
