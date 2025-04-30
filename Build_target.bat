@echo off
REM Build_target.bat

REM 自动检测当前盘符

set "SOURCE_DIR=%cd%\CppTester"
set "BUILD_DIR=%cd%\CppTester\x64"

REM 确保 CMakeLists.txt 文件存在于指定目录
if exist "%SOURCE_DIR%\CMakeLists.txt" (
    echo CMakeLists.txt found in %SOURCE_DIR%. Running CMake configuration for x64...
    cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%" -A x64 ^
          -DCMAKE_POLICY_VERSION_MINIMUM=3.5
) else (
    echo Error: CMakeLists.txt not found in %SOURCE_DIR%.
    exit /b 1
)


REM 使用 CMake 和 Visual Studio 进行构建
echo Building project...
cmake --build "%BUILD_DIR%" --config Release

REM 打印构建结果
if errorlevel 1 (
    echo Build failed.
    exit /b 1
) else (
    echo Build succeeded.
)