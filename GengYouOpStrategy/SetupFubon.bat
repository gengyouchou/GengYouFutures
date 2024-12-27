@echo off
REM Set up virtual environment and install packages
set VENV_DIR=venv
set WHL_FILE=fubon_neo-2.1.0-cp37-abi3-win_amd64.whl
set DOC_DIR=docs

REM Step 1: Create virtual environment if it does not exist
if not exist "%VENV_DIR%" (
    echo Creating virtual environment...
    python -m venv "%VENV_DIR%"
) else (
    echo Virtual environment already exists.
)

REM Step 2: Activate virtual environment
call "%VENV_DIR%\Scripts\activate"

REM Step 3: Update pip to the latest version
echo Updating pip...
python -m pip install --upgrade pip

REM Step 4: Install the .whl file
echo Installing %WHL_FILE%...
pip install %WHL_FILE%

REM Step 5: Install missing requests package
echo Installing requests package...
pip install requests

REM Step 6: Create documentation directory if it does not exist
if not exist "%DOC_DIR%" (
    echo Creating documentation directory...
    mkdir "%DOC_DIR%"
)

REM Step 7: Generate documentation
echo Generating documentation...
python -m pydoc -w fubon_neo

REM Move generated .html file to the documentation directory
if exist "fubon_neo.html" (
    move "fubon_neo.html" "%DOC_DIR%\fubon_neo.html"
    echo Documentation generated at %DOC_DIR%\fubon_neo.html
) else (
    echo Documentation generation failed!
)

call python GengYouOpStrategy.py

@REM REM Deactivate virtual environment
@REM deactivate

echo All tasks completed successfully.
pause
