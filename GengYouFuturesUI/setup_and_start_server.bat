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
echo Activating virtual environment...
call venv\Scripts\activate.bat
if %errorlevel% neq 0 (
    echo Failed to activate virtual environment. Please check your environment setup.
    pause
    exit /b 1
)

:: 确保 pip 是最新的
echo Updating pip to the latest version...
pip install --upgrade pip
if %errorlevel% neq 0 (
    echo Failed to update pip. Please check your internet connection or pip installation.
    pause
    exit /b 1
)

:: 安装依赖包
echo Installing required packages...
pip install websockets pywin32
if %errorlevel% neq 0 (
    echo Failed to install required packages. Please check your internet connection or pip configuration.
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

:: 显示消息并暂停脚本
echo WebSocket server is running. Press any key to exit...
pause

endlocal
