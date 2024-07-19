set "SOURCE_DIR=C:\GengYouFutures\CppTester"
set "BUILD_DIR=C:\GengYouFutures\CppTester\build"

cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%"

REM 使用 CMake 和 Visual Studio 進行構建
echo Building project...
cmake --build . --config Debug

