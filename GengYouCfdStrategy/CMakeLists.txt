cmake_minimum_required(VERSION 3.15)
project(GengYouCfdStrategy)

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

# 定義 DLL 庫，改用 SHARED 生成 DLL
add_library(GengYouCfdStrategy SHARED GengYouCfdStrategy.cpp)

# 添加頭文件（請根據實際需要提供對應的 .h 檔案）
set(HEADERS
    GengYouCfdStrategy.h
)

# 設定包含目錄
target_include_directories(GengYouCfdStrategy PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}  # 添加庫的源文件目錄
)

# 添加 httplib 和 nlohmann_json 作為外部庫依賴
target_link_libraries(GengYouCfdStrategy PRIVATE httplib::httplib nlohmann_json::nlohmann_json)

# 為 Windows 平台自動匯出所有符號（可選）
if(WIN32)
    set_target_properties(GengYouCfdStrategy PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# 安裝 DLL 與頭文件
install(TARGETS GengYouCfdStrategy DESTINATION lib)
install(FILES ${HEADERS} DESTINATION include)
