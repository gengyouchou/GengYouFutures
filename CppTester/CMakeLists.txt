cmake_minimum_required(VERSION 3.15)
project(CppTester)

# 設定 C++ 標準為 C++17 並必須
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加源文件
set(SOURCES
    CppTester/main.cpp
    CppTester/SKCenterLib.cpp
    CppTester/SKOrderLib.cpp
    CppTester/SKQuoteLib.cpp
    CppTester/SKOSQuoteLib.cpp
    CppTester/SKReplyLib.cpp
    CppTester/Strategy.cpp
    CppTester/StrategyCfd.cpp
    CppTester/StrategyOF.cpp          # <<< 新增：海外期貨停損停利邏輯
    ../Logger/Logger.cpp
    # Sqlite
    ../Sqlite/shell.c
    ../Sqlite/sqlite3.c
)

# 添加頭文件
set(HEADERS
    CppTester/SKCenterLib.h
    CppTester/SKCOM_reference.h
    CppTester/SKOrderLib.h
    CppTester/SKQuoteLib.h
    CppTester/SKOSQuoteLib.h
    CppTester/SKReplyLib.h
    CppTester/TEventHandler.h
    CppTester/Strategy.h
    CppTester/StrategyCfd.h
    CppTester/StrategyOF.h           # <<< 新增：海外期貨停損停利介面
    ../Logger/Logger.h
    ../GengYouFuturesUI/GengYouFuturesUI.h
    # Sqlite
    ../Sqlite/sqlite3.h
    ../Sqlite/sqlite3ext.h
)

# 添加頭文件目錄
include_directories(${CMAKE_SOURCE_DIR}/CppTester)
include_directories("../Logger")
include_directories("../../")
include_directories("../GengYouFuturesUI/")
include_directories("../Sqlite")
# 【新增】加入 httplib 的包含目錄
include_directories("${CMAKE_SOURCE_DIR}/x64/_deps/httplib-src")

# 設定配置類型
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# 引入 yaml-cpp
include(FetchContent)
FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG yaml-cpp-0.7.0
)
FetchContent_MakeAvailable(yaml-cpp)

# 添加 GengYouFuturesUI 子目錄
add_subdirectory("${CMAKE_SOURCE_DIR}/../GengYouFuturesUI" "${CMAKE_BINARY_DIR}/GengYouFuturesUI")

# 鏈接 yaml-cpp 到 GengYouFuturesUI
target_link_libraries(GengYouFuturesUI PUBLIC yaml-cpp)

# 定義生成的目標
add_executable(CppTester ${SOURCES} ${HEADERS})

# 鏈接 yaml-cpp、GengYouFuturesUI 與 nlohmann_json 到 CppTester
target_link_libraries(CppTester 
    PUBLIC yaml-cpp 
           GengYouFuturesUI 
           nlohmann_json::nlohmann_json
)

# 設置 SQLite3 DLL 的路徑
set(SQLITE3_DLL_PATH "${CMAKE_SOURCE_DIR}/../DLL/x64/Sqlite/sqlite3.dll")

# 將 SQLite3 DLL 複製到可執行檔目錄
add_custom_command(TARGET CppTester POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${SQLITE3_DLL_PATH}"
    $<TARGET_FILE_DIR:CppTester>
)

# 將 SKCOM.dll 複製到可執行檔目錄
add_custom_command(TARGET CppTester POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/../DLL/x64/SKCOM.dll"
    $<TARGET_FILE_DIR:CppTester>
)

# 將 database.yaml 複製到可執行檔目錄
add_custom_command(TARGET CppTester POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/CppTester/database.yaml"
    $<TARGET_FILE_DIR:CppTester>
)

# 获取当前 CMakeLists.txt 文件所在的目录
set(CPP_TESTER_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# 从当前路径提取 Windows 根目录（如 "F:/"）
get_filename_component(ROOT_DIR "${CPP_TESTER_DIR}" DIRECTORY) # 取得上层路径
get_filename_component(WINDOWS_ROOT "${ROOT_DIR}" DIRECTORY)

# 定义 database.yaml 的完整路径
set(DATABASE_PATH "${CPP_TESTER_DIR}/CppTester/database.yaml")
set(CACHE_DATABASE_PATH "${CMAKE_CURRENT_BINARY_DIR}/cache.yaml")

# 打印调试信息
message(STATUS "CppTester directory: ${CPP_TESTER_DIR}")
message(STATUS "Windows root: ${WINDOWS_ROOT}")
message(STATUS "Database path: ${DATABASE_PATH}")

# 生成 config.h 文件
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# 輸出一些調試信息
message(STATUS "CppTester directory: ${CMAKE_SOURCE_DIR}/CppTester")
