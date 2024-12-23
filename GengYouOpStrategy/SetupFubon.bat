@echo off

REM 設置變數
set VENV_DIR=venv
set WHL_FILE=fubon_neo-2.1.0-cp37-abi3-win_amd64.whl
set MODULE_NAME=fubon_neo

REM 檢查 Python 是否安裝
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [錯誤] 未找到 Python，請先安裝 Python。
    exit /b 1
)

REM 創建虛擬環境
if not exist %VENV_DIR% (
    echo [信息] 正在創建虛擬環境...
    python -m venv %VENV_DIR%
)

REM 激活虛擬環境
call %VENV_DIR%\Scripts\activate.bat
if %errorlevel% neq 0 (
    echo [錯誤] 無法激活虛擬環境。
    exit /b 1
)

REM 確保 pip 已更新
echo [信息] 正在更新 pip...
pip install --upgrade pip

REM 安裝 WHL 文件
echo [信息] 正在安裝 %WHL_FILE%...
pip install %WHL_FILE%
if %errorlevel% neq 0 (
    echo [錯誤] 無法安裝 WHL 文件。
    exit /b 1
)

REM 生成文檔
echo [信息] 正在生成 %MODULE_NAME% 的文檔...
python -m pydoc -w %MODULE_NAME%
if %errorlevel% neq 0 (
    echo [錯誤] 無法生成文檔。
    exit /b 1
)

REM 檢查文檔生成
if exist %MODULE_NAME%.html (
    echo [完成] 文檔已生成為 %MODULE_NAME%.html。
) else (
    echo [錯誤] 未找到生成的文檔。
    exit /b 1
)

REM 結束
echo [完成] 所有操作成功完成！
exit /b 0
