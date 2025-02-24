cmake_minimum_required(VERSION 3.15)
project(GengYouCfdStrategy)

# 設定 C++ 標準
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 選項：是否以 32 位元編譯
option(BUILD_32BIT "Build for 32-bit target" OFF)
if(BUILD_32BIT)
    message(STATUS "Building for 32-bit target")
    if(NOT MSVC)
        # 非 MSVC 下自動添加 32 位編譯標誌
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    else()
        message(WARNING "For MSVC, please configure using -A Win32 to build a 32-bit version.")
    endif()
endif()

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

# 定義共享庫（DLL）
add_library(GengYouCfdStrategy SHARED GengYouCfdStrategy.cpp)

# 添加頭文件（根據實際需求）
set(HEADERS
    GengYouCfdStrategy.h
)

# 設定包含目錄
target_include_directories(GengYouCfdStrategy PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 連結外部庫：httplib 與 nlohmann/json
target_link_libraries(GengYouCfdStrategy PRIVATE httplib::httplib nlohmann_json::nlohmann_json)

# 若在 Windows 平台，設定自動匯出所有符號
if(WIN32)
    set_target_properties(GengYouCfdStrategy PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# 安裝 DLL 與頭文件
install(TARGETS GengYouCfdStrategy DESTINATION lib)
install(FILES ${HEADERS} DESTINATION include)
