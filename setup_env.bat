@echo off
REM setup_env.bat
REM 设置 Visual Studio 环境变量
REM 根据你的 Visual Studio 版本和安装路径修改路径

REM 设置字符集
chcp 65001

REM 设置编译器路径
set "VS_VERSION=2019"
set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\%VS_VERSION%\VC\Auxiliary\Build"
set "COMMUNITY_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build"

REM 确定 MSVC 版本号（需要根据实际安装的版本号进行调整）
set "MSVC_VERSION=14.29.30133" 

REM 修复路径问题，去掉多余的反斜杠
set "CL_PATH=%VS_PATH%\Tools\MSVC\%MSVC_VERSION%\bin\Hostx64\x64"
set "INCLUDE_PATH=%VS_PATH%\Tools\MSVC\%MSVC_VERSION%\include"
set "LIB_PATH=%VS_PATH%\Tools\MSVC\%MSVC_VERSION%\lib\x64"

REM 尝试设置编译器环境变量
echo Trying to set environment variables using %VS_PATH%...
call "%VS_PATH%\vcvarsall.bat" x64

if errorlevel 1 (
    echo Failed to set environment variables using %VS_PATH%. Trying %COMMUNITY_PATH%...
    call "%COMMUNITY_PATH%\vcvarsall.bat" x64

    if errorlevel 1 (
        echo Error: Failed to set environment variables using both paths.
        pause
        exit /b 1
    ) else (
        echo Environment variables set using %COMMUNITY_PATH%.
    )
) else (
    echo Environment variables set using %VS_PATH%.
)

REM 打印当前环境变量设置
echo Visual Studio Build Tools environment variables set:
echo PATH: %PATH%
echo INCLUDE: %INCLUDE%
echo LIB: %LIB%


