@echo off

REM Set source and build directories
set "SOURCE_DIR=%cd%\GengTraderGuardian"
set "BUILD_DIR=%cd%\GengTraderGuardian\x64"

REM Create build directory if it doesn't exist
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

REM Navigate to build directory
cd "%BUILD_DIR%"

REM 确保 CMakeLists.txt 文件存在于指定目录
if exist "%SOURCE_DIR%\CMakeLists.txt" (
    echo CMakeLists.txt found in %SOURCE_DIR%. Running CMake...
    cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%"
) else (
    echo Error: CMakeLists.txt not found in %SOURCE_DIR%.
)

REM 使用 CMake 和 Visual Studio 进行构建
echo Building project...
cmake --build "%BUILD_DIR%" --config Debug
@REM cmake --build "%BUILD_DIR%" --config Release

REM 打印构建结果
if errorlevel 1 (
    echo Build failed.
) else (
    echo Build succeeded.
)

cd ..
cd ..

