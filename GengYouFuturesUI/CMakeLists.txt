cmake_minimum_required(VERSION 3.15)
project(GengYouFuturesUI)

# 設定 C++ 標準
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自動下載並導入 cpp-httplib
include(FetchContent)
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG master  # 使用最新的 master 分支
)
FetchContent_MakeAvailable(httplib)

# 自動下載並導入 nlohmann/json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.10.0  # 指定使用 v3.10.0 版本
)
FetchContent_MakeAvailable(nlohmann_json)

# 定義項目的可執行文件
add_executable(GengYouFuturesUI GengYouFuturesUI.cpp)

# 添加头文件
set(HEADERS
    GengYouFuturesUI.h
    ../CppTester/CppTester/SKCenterLib.h
    ../CppTester/CppTester/SKCOM_reference.h
    ../CppTester/CppTester/SKOrderLib.h
    ../CppTester/CppTester/SKQuoteLib.h
    ../CppTester/CppTester/SKOSQuoteLib.h
    ../CppTester/CppTester/SKReplyLib.h
    ../CppTester/CppTester/TEventHandler.h
    ../CppTester/CppTester/Strategy.h
    ../Logger/Logger.h
)

include_directories("../Logger")
include_directories("../../")
include_directories("../CppTester/CppTester")

# 添加 httplib 和 json 作為外部庫依賴
target_link_libraries(GengYouFuturesUI PRIVATE httplib::httplib nlohmann_json::nlohmann_json)

# 移除 Windows 特定的命名管道支持
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_CRT_SECURE_NO_WARNINGS")
endif()
