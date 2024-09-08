REM 自动检测当前盘符
set "currentDrive=%~d0"

set "SOURCE_DIR=%currentDrive%\GengYouFutures"

git reset --hard head
git pull origin
git checkout origin/GengYou/main
git pull origin
git checkout GengYou/main
git status
git fetch origin
git checkout GengYou/main
git branch --show-current
git status

"%SOURCE_DIR%"\Build_target.bat