@echo off

set "file=C:\GengYouFutures\GengTraderGuardian\config.yaml"
set "tempfile=temp.txt"


:menu
echo Slect(1,2,3,4,5):
echo 1 - Kill CppTester.exe AND GengTraderGuardian.exe
echo 2 - Run cmdCaputre.exe
echo 3 - Modify Config.yaml AND Restart Order Machine
echo 4 - pullOrigin AND Build
echo 5 - Exit

set /p option="Slect (1, 2, 3, 4, 5): "

if "%option%"=="1" (
    echo Kill CppTester.exe & GengTraderGuardian.exe
    taskkill /IM CppTester.exe /F
	taskkill /IM GengTraderGuardian.exe /F
) else if "%option%"=="2" (
    echo Lookup CppTester.exe Output
    call  "C:\Users\adslp\cmdCaptureClinet.exe.lnk"
) else if "%option%"=="3" (
	cls
    echo ======== Change yaml Before ========
	for /f "usebackq delims=" %%a in ("%file%") do (echo %%a)
	echo ======== Change yaml Before ========
	echo.
setlocal enabledelayedexpansion

	set /a lineCount=0
	set "patten3=BID_OFFER_LONG_SHORT_THRESHOLD: "
	set "patten4=ACTIVITY_POINT: "
	set "patten5=CLOSING_KEY_PRICE_LEVEL: "
	set "patten6=MAXIMUM_LOSS: "
	set "patten7=STRATEGY_MODE: "

	(for /f "usebackq delims=" %%a in ("%file%") do (
		set /a lineCount = lineCount + 1
		if !lineCount! == 3 (
			echo %%a
			set /p "newContent=NewValue: "
			echo !patten3!!newContent! >> "%tempfile%"			
		) else  if !lineCount! == 4 (
			echo %%a
			set /p "newContent=NewValue: "
			echo !patten4!!newContent! >> "%tempfile%"	
		)else  if !lineCount! == 5 (
			echo %%a
			set /p "newContent=NewValue: "
			echo !patten5!!newContent! >> "%tempfile%"	
		)else  if !lineCount! == 6 (
			echo %%a
			set /p "newContent=NewValue: "
			echo !patten6!!newContent! >> "%tempfile%"	
		)else  if !lineCount! == 7 (
			echo %%a
			set /p "newContent=NewValue: "
			echo !patten7!!newContent! >> "%tempfile%"	
		)else  if !lineCount! == 1 (
			echo %%a >> "%tempfile%"	
		)else  if !lineCount! == 2 (			
            echo password: "youlose1A^!" >> "%tempfile%"
		)
	)) 
endlocal
	REM 用临时文件覆盖原始文件
	move /y "%tempfile%" "%file%"
	echo ======== Change yaml After ========
	for /f "usebackq delims=" %%a in ("%file%") do (echo %%a)
	echo ======== Change yaml After ========
	taskkill /IM CppTester.exe /F
	taskkill /IM GengTraderGuardian.exe /F
	echo Exit...
    exit /b
) else if "%option%"=="4" (
    echo pullOriginBuild.bat
	call "C:\GengYouFutures\pullOriginBuild.bat"
    
) else if "%option%"=="5" (
    echo Exit...
    exit /b
) else (
    echo Fk Choice。
)
echo.
goto menu