@echo off
setlocal

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

:: 确认 Python 版本
python --version

:: 检查虚拟环境是否已存在
if not exist "venv" (
    echo Virtual environment not found, creating a new one...
    python -m venv venv
) else (
    echo Virtual environment already exists.
)

:: 激活虚拟环境
echo Activating virtual environment...
call venv\Scripts\activate

:: 确保 pip 是最新的
echo Updating pip to the latest version...
pip install --upgrade pip

:: 安装 websockets 库
echo Installing websockets...
pip install websockets

:: 启动 Python WebSocket 服务器
echo Starting Python WebSocket server...
python server.py

:: 显示消息并暂停脚本
echo WebSocket server is running. Press any key to exit...
pause

endlocal
