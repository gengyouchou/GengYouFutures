cmake_minimum_required(VERSION 3.14)

# 项目信息
project(GengYouFuturesUI VERSION 1.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# 引入 FetchContent 模块
include(FetchContent)

# 配置 JsonCpp
FetchContent_Declare(
  jsoncpp
  GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
  GIT_TAG 1.9.5
)
FetchContent_MakeAvailable(jsoncpp)

# 配置 Drogon
FetchContent_Declare(
  drogon
  GIT_REPOSITORY https://github.com/drogonframework/drogon.git
  GIT_TAG v1.9.1
)
FetchContent_MakeAvailable(drogon)

# 设置包含目录和库文件路径
set(JSONCPP_INCLUDE_DIRS "${jsoncpp_SOURCE_DIR}/include")
set(JSONCPP_LIBRARIES "${jsoncpp_BINARY_DIR}/lib/jsoncpp.lib")

set(DROGON_INCLUDE_DIRS "${drogon_SOURCE_DIR}/lib/inc")
set(DROGON_LIBRARIES "${drogon_BINARY_DIR}/lib/drogon.lib")

# 检查是否生成了 jsoncpp.lib 和 drogon.lib
if(NOT EXISTS "${JSONCPP_LIBRARIES}")
    message(FATAL_ERROR "JsonCpp library not found: ${JSONCPP_LIBRARIES}")
endif()

if(NOT EXISTS "${DROGON_LIBRARIES}")
    message(FATAL_ERROR "Drogon library not found: ${DROGON_LIBRARIES}")
endif()

# 显示调试信息
message(STATUS "JsonCpp Include Dir: ${JSONCPP_INCLUDE_DIRS}")
message(STATUS "JsonCpp Library: ${JSONCPP_LIBRARIES}")
message(STATUS "Drogon Include Dir: ${DROGON_INCLUDE_DIRS}")
message(STATUS "Drogon Library: ${DROGON_LIBRARIES}")

# 添加可执行文件
add_executable(GengYouFuturesUI main.cpp)

# 设置包含目录
target_include_directories(GengYouFuturesUI PRIVATE
    ${JSONCPP_INCLUDE_DIRS}
    ${DROGON_INCLUDE_DIRS}
)

# 链接库
target_link_libraries(GengYouFuturesUI PRIVATE
    ${JSONCPP_LIBRARIES}
    ${DROGON_LIBRARIES}
)

