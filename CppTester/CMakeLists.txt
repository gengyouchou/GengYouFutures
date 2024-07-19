# 指定所需的最低 CMake 版本
cmake_minimum_required(VERSION 3.10)

# 项目信息
project(CppTester)

# 设置 C++ 标准为 17 并指定为必需
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加源文件
set(SOURCES
    main.cpp
    SKCenterLib.cpp
    SKOrderLib.cpp
    SKQuoteLib.cpp
    SKReplyLib.cpp
)

# 添加头文件
set(HEADERS
    SKCenterLib.h
    SKCOM_reference.h
    SKOrderLib.h
    SKQuoteLib.h
    SKReplyLib.h
    TEventHandler.h
)

# 添加头文件目录
include_directories(${CMAKE_SOURCE_DIR})

# 设置不同配置的编译选项
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# 定义生成的目标
add_executable(CppTester ${SOURCES} ${HEADERS})

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
