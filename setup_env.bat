@echo off
REM 设置 Visual Studio Build Tools 环境变量
REM 根据你的 Visual Studio 版本和安装路径修改路径

REM 设置编译器路径
set "VS_VERSION=2019"  REM 设置 Visual Studio 版本
set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\%VS_VERSION%\Community"

REM 找到 vcvarsall.bat 文件
set "VC_VARS_PATH="
for /d %%D in ("%VS_PATH%\*") do (
    if exist "%%D\VC\Auxiliary\Build\vcvarsall.bat" (
        set "VC_VARS_PATH=%%D\VC\Auxiliary\Build\vcvarsall.bat"
        goto :found
    )
)

echo Error: vcvarsall.bat was not found.
exit /b 1

:found
REM 调用 vcvarsall.bat 设置编译器环境
call "%VC_VARS_PATH%" x64

REM 设置 CMake 生成器
set "CMAKE_GENERATOR=NMake Makefiles"
set "CMAKE_BUILD_TYPE=Debug"

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
    cmake -G "%CMAKE_GENERATOR%" -S "%SOURCE_DIR%" -B "%BUILD_DIR%"
) else (
    echo Error: CMakeLists.txt not found in %SOURCE_DIR%.
)

REM 提示用户按任意键继续
echo.
echo Press any key to continue...
pause >nul
