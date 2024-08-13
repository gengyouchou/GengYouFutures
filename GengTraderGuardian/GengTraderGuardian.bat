@echo off
setlocal

:: 设置变量
set "processName=CppTester.exe"
set "currentDir=%~dp0"
set "processPath=%currentDir%\%processName%"

:: 获取当前驱动器
for %%d in ("%currentDir%") do set "driveLetter=%%d"

:: 检测驱动器是否存在
if exist "%driveLetter%\" (
    :: 设置进程路径
    set "processPath=%driveLetter%\GengYouFutures\CppTester\x64\Debug\%processName%"

    :: 检查进程是否正在运行
    tasklist /FI "IMAGENAME eq %processName%" 2>NUL | find /I /N "%processName%">NUL
    if "%ERRORLEVEL%"=="0" (
        :: 进程正在运行
        echo %processName% is already running.
    ) else (
        :: 进程没有运行，尝试启动
        echo %processName% is not running. Restarting...
        if exist "%processPath%" (
            start "" "%processPath%"
            if "%ERRORLEVEL%"=="0" (
                echo %processName% restarted successfully.
            ) else (
                echo Failed to restart %processName%.
            )
        ) else (
            echo The path %processPath% does not exist.
        )
    )
) else (
    echo Drive %driveLetter% does not exist.
)

endlocal
