@echo off
setlocal enabledelayedexpansion

REM 设置源代码和构建目录
set "SOURCE_DIR=%cd%"
set "BUILD_DIR=%cd%\x64"

REM 检查构建目录是否存在，不存在则创建
if not exist "%BUILD_DIR%" (
    echo Creating build directory at %BUILD_DIR%...
    mkdir "%BUILD_DIR%"
)

REM 确保在源目录中存在 CMakeLists.txt 文件
if exist "%SOURCE_DIR%\CMakeLists.txt" (
    echo CMakeLists.txt found in %SOURCE_DIR%. Running CMake configuration...

    REM 如果构建目录已存在，清理旧的构建文件
    if exist "%BUILD_DIR%" (
        echo Cleaning old build files...
        rd /s /q "%BUILD_DIR%"
    )
    
    REM 运行 CMake 配置
    cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%" -G "Visual Studio 16 2019" -A x64
    if %errorlevel% neq 0 (
        echo Error: CMake configuration failed with error code %errorlevel%.
        exit /b %errorlevel%
    )
) else (
    echo Error: CMakeLists.txt not found in %SOURCE_DIR%.
    exit /b 1
)

REM 构建项目
echo Starting build process...
cmake --build "%BUILD_DIR%" --config Release
if %errorlevel% neq 0 (
    echo Error: Build process failed with error code %errorlevel%.
    exit /b %errorlevel%
) else (
    echo Build succeeded. Executable should be in:
    echo %BUILD_DIR%\Release
)

REM 可选：返回源代码目录
cd "%SOURCE_DIR%"
endlocal
