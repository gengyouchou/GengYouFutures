@echo off
REM Build_target.bat

REM 设置 SOURCE_DIR 和 BUILD_DIR
set "SOURCE_DIR=%CD%"
set "BUILD_DIR=%CD%\..\build"

REM 运行 CMake 生成构建文件
cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%"

REM 使用 CMake 和 Visual Studio 进行构建
echo Building project...
cmake --build "%BUILD_DIR%" --config Debug
