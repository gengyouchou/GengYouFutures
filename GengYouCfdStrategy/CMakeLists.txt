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

# 定義庫
add_library(GengYouFuturesUI STATIC GengYouFuturesUI.cpp)

# 添加頭文件
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

# 設定包含目錄
target_include_directories(GengYouFuturesUI PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}  # 添加庫的源文件目錄
    ../Logger
    ../../
    ../CppTester/CppTester
)

# 添加 httplib 和 json 作為外部庫依賴
target_link_libraries(GengYouFuturesUI PRIVATE httplib::httplib nlohmann_json::nlohmann_json)

# 安裝靜態庫和頭文件
install(TARGETS GengYouFuturesUI DESTINATION lib)
install(FILES ${HEADERS} DESTINATION include)
