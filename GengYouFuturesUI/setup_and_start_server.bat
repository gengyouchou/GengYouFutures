@echo off
setlocal enabledelayedexpansion

:: 设置工作目录为当前文件夹
cd /d %~dp0

:: 检查 Python 是否已安装
echo Checking for Python installation...
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Python is not installed or not in the PATH. Please install Python and make sure it's added to PATH.
    pause
    exit /b 1
)

:: 显示 Python 版本
for /f "tokens=2 delims= " %%a in ('python --version') do set PYTHON_VERSION=%%a
echo Found Python version: !PYTHON_VERSION!

:: 检查虚拟环境是否已存在
if not exist "venv" (
    echo Virtual environment not found, creating a new one...
    python -m venv venv
    if %errorlevel% neq 0 (
        echo Failed to create a virtual environment. Please check your Python installation.
        pause
        exit /b 1
    )
) else (
    echo Virtual environment already exists.
)

:: 激活虚拟环境
if exist "venv\Scripts\activate.bat" (
    call venv\Scripts\activate.bat
) else (
    echo Virtual environment activation script not found. Please check your virtual environment setup.
    pause
    exit /b 1
)

:: 确保 pip 是最新的
echo Updating pip to the latest version...
venv\Scripts\python.exe -m pip install --upgrade pip
if %errorlevel% neq 0 (
    echo Failed to update pip. Please check your internet connection or pip installation.
    pause
    exit /b 1
)

:: 安装依赖包（包括 aiohttp）
echo Installing required packages...
pip install websockets pywin32 aiohttp
if %errorlevel% neq 0 (
    echo Failed to install required packages. Please check your internet connection or pip configuration.
    pause
    exit /b 1
)

:: 检查 server.py 是否存在
if not exist "server.py" (
    echo server.py not found in the current directory. Please check your setup.
    pause
    exit /b 1
)

:: 启动 HTTP 服务器（服务静态文件，如 index.html）
if not exist "index.html" (
    echo index.html not found in the current directory. Please check your setup.
    pause
    exit /b 1
)

start cmd /k "echo Starting HTTP server on port 8000... && python -m http.server 8000"
if %errorlevel% neq 0 (
    echo HTTP server failed to start. Please check for errors.
    pause
    exit /b 1
)

:: 启动 Python WebSocket 服务器
echo Starting Python WebSocket server...
python server.py
if %errorlevel% neq 0 (
    echo WebSocket server failed to start. Please check your server.py script for errors.
    pause
    exit /b 1
)

:: 脚本结束
echo Both servers are running. Press any key to exit...
pause
endlocal
