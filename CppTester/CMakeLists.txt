# File: CMakeLists.txt

# 指定所需的最低 CMake 版本
cmake_minimum_required(VERSION 3.10)

# 项目信息
project(CppTester)

# 设置 C++ 标准为 17 并指定为必需
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# if(NOT CMAKE_BUILD_TYPE)
#     set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
# endif()

# 添加源文件
set(SOURCES
    CppTester/main.cpp
    CppTester/SKCenterLib.cpp
    CppTester/SKOrderLib.cpp
    CppTester/SKQuoteLib.cpp
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
    CppTester/SKReplyLib.h
    CppTester/TEventHandler.h
    CppTester/Strategy.h

    ../Logger/Logger.h
)

# 添加头文件目录
include_directories(${CMAKE_SOURCE_DIR}/CppTester)
include_directories("../Logger")
include_directories("../../")

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

# # 添加库路径
# link_directories("${CMAKE_SOURCE_DIR}/../DLL/x64")

# # 链接库
# target_link_libraries(CppTester SKCOM)

# # 设置运行时 DLL 路径
# set_target_properties(CppTester PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../"
# )

# 设置 DLL 的路径，确保 DLL 可以被找到
add_custom_command(TARGET CppTester POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/../DLL/x64/SKCOM.dll"
    $<TARGET_FILE_DIR:CppTester>
)

