# File: CMakeLists.txt

# 指定所需的最低 CMake 版本
cmake_minimum_required(VERSION 3.10...3.24)

# 项目信息
project(CppTester)

# 设置 C++ 标准为 17 并指定为必需
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加源文件
set(SOURCES
    CppTester/main.cpp
    CppTester/SKCenterLib.cpp
    CppTester/SKOrderLib.cpp
    CppTester/SKQuoteLib.cpp
    CppTester/SKOSQuoteLib.cpp
    CppTester/SKReplyLib.cpp
    CppTester/Strategy.cpp
    ../Logger/Logger.cpp
)

# 添加头文件
set(HEADERS
    CppTester/SKCenterLib.h
    CppTester/SKCOM_reference.h
    CppTester/SKOrderLib.h
    CppTester/SKQuoteLib.h
    CppTester/SKOSQuoteLib.h
    CppTester/SKReplyLib.h
    CppTester/TEventHandler.h
    CppTester/Strategy.h
    ../Logger/Logger.h
    ../GengYouFuturesUI/GengYouFuturesUI.h
)

# 添加头文件目录
include_directories(${CMAKE_SOURCE_DIR}/CppTester)
include_directories("../Logger")
include_directories("../../")
include_directories("../GengYouFuturesUI/")

# 设置不同配置的编译选项
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# 引入 yaml-cpp
include(FetchContent)

FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
  GIT_TAG yaml-cpp-0.7.0  # 可以指定具体的 tag、分支名或 commit hash
)
FetchContent_MakeAvailable(yaml-cpp)

# 添加子目录
add_subdirectory("${CMAKE_SOURCE_DIR}/../GengYouFuturesUI" "${CMAKE_BINARY_DIR}/GengYouFuturesUI")

# 定义生成的目标
add_executable(CppTester ${SOURCES} ${HEADERS})

# 链接 yaml-cpp 库和靜態庫 GengYouFuturesUI
target_link_libraries(CppTester PUBLIC yaml-cpp GengYouFuturesUI)

# 设置编译器标志
target_compile_options(CppTester PRIVATE
    $<$<CONFIG:Debug>:-D_DEBUG -D_CONSOLE>
    $<$<CONFIG:Release>:-DNDEBUG -D_CONSOLE>
)

# 设置链接器标志
target_link_options(CppTester PRIVATE
    $<$<CONFIG:Debug>:-debug>
    $<$<CONFIG:Release>:-optimize>
)

# 设置 DLL 的路径，确保 DLL 可以被找到
add_custom_command(TARGET CppTester POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/../DLL/x64/SKCOM.dll"
    $<TARGET_FILE_DIR:CppTester>
)

# 在 add_custom_command 中修改路徑
add_custom_command(TARGET CppTester POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/CppTester/database.yaml"
    $<TARGET_FILE_DIR:CppTester>
)
# 獲取當前 CMakeLists.txt 文件所在的目錄
set(CPP_TESTER_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# 從當前路徑提取 Windows 根目錄（如 "F:/"）
get_filename_component(ROOT_DIR "${CPP_TESTER_DIR}" DIRECTORY) # 取得上層路徑
get_filename_component(WINDOWS_ROOT "${ROOT_DIR}" DIRECTORY)

# 定義 database.yaml 的完整路徑
set(DATABASE_PATH "${CPP_TESTER_DIR}/CppTester/database.yaml")

# 打印調試信息
message(STATUS "CppTester directory: ${CPP_TESTER_DIR}")
message(STATUS "Windows root: ${WINDOWS_ROOT}")
message(STATUS "Database path: ${DATABASE_PATH}")


# Generate the config.h file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)


# 将生成的头文件目录添加到包含路径中
include_directories(${CMAKE_CURRENT_BINARY_DIR})



