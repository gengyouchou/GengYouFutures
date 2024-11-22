@echo off
REM 定义项目根目录
set ROOT_DIR=%~dp0

REM 定义构建目录
set BUILD_DIR=%ROOT_DIR%build

REM 切换到项目根目录
cd /d %ROOT_DIR%

REM 检查是否已存在构建目录，如果存在则清理
if exist %BUILD_DIR% (
    echo Cleaning existing build directory...
    rmdir /s /q %BUILD_DIR%
)

REM 创建新的构建目录
mkdir %BUILD_DIR%
cd %BUILD_DIR%

REM 配置项目
echo Running CMake configuration...
cmake .. -G "Visual Studio 16 2019" -A x64
if %errorlevel% neq 0 (
    echo CMake configuration failed!
    exit /b %errorlevel%
)

REM 编译项目
echo Building project...
cmake --build . --config Debug
if %errorlevel% neq 0 (
    echo Build failed!
    exit /b %errorlevel%
)

REM 提示完成并显示生成文件路径
echo Build completed successfully!
echo Executable is located at:
echo %BUILD_DIR%\Debug\GengYouFuturesUI.exe
