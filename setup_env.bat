@echo off
REM setup_env.bat

REM 设置字符集
chcp 65001

REM 获取当前脚本的目录
set "SCRIPT_DIR=%cd%"

REM 设置 Visual Studio 环境变量
REM 根据你的 Visual Studio 版本和安装路径修改路径

REM 设置编译器路径
set "VS_VERSION=2019"
set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\%VS_VERSION%\VC\Auxiliary\Build"
set "COMMUNITY_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build"

REM 确定 MSVC 版本号（需要根据实际安装的版本号进行调整）
set "MSVC_VERSION=14.29.30133"

REM 尝试设置编译器环境变量
echo Trying to set environment variables using "%VS_PATH%"...
call "%VS_PATH%\vcvarsall.bat" x64

if errorlevel 1 (
    echo Failed to set environment variables using "%VS_PATH%". Trying "%COMMUNITY_PATH%"...
    call "%COMMUNITY_PATH%\vcvarsall.bat" x64

    if errorlevel 1 (
        echo Error: Failed to set environment variables using both paths.
        pause
        exit /b 1
    ) else (
        echo Environment variables set using "%COMMUNITY_PATH%".
    )
) else (
    echo Environment variables set using "%VS_PATH%".
)

REM 设置临时变量
set "CMAKE_SOURCE_DIR=%SCRIPT_DIR%\CppTester"
set "BUILD_DIR=%SCRIPT_DIR%\build"
set "DLL_PATH=%SCRIPT_DIR%\DLL\x64"
set "PATH=%DLL_PATH%;%PATH%"

REM 打印所有临时变量
echo.
echo Temporary Variables:
echo.
echo CMAKE_SOURCE_DIR=%CMAKE_SOURCE_DIR%
echo.
echo BUILD_DIR=%BUILD_DIR%
echo.
echo DLL_PATH=%DLL_PATH%
echo.
echo INCLUDE=%INCLUDE%
echo.
echo LIB=%LIB%
echo.
echo PATH=%PATH%
echo.

REM 检查 DLL 文件是否存在
if exist "%DLL_PATH%\SKCOM.dll" (
    echo SKCOM.dll found in %DLL_PATH%.
) else (
    echo Error: SKCOM.dll not found in %DLL_PATH%.
)

REM 确保 CMakeLists.txt 文件存在于指定目录
if exist "%CMAKE_SOURCE_DIR%\CMakeLists.txt" (
    echo CMakeLists.txt found in %CMAKE_SOURCE_DIR%. Running CMake...
    cmake -S "%CMAKE_SOURCE_DIR%" -B "%BUILD_DIR%"
) else (
    echo Error: CMakeLists.txt not found in %CMAKE_SOURCE_DIR%.
    exit /b 1
)
