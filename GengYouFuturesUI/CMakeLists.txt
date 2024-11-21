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

# 使用相对路径指定 JsonCpp 和 Drogon 的路径
set(JSONCPP_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/x64/_deps/jsoncpp-build/include")
set(JSONCPP_LIBRARIES "${CMAKE_SOURCE_DIR}/x64/_deps/jsoncpp-build/lib/libjsoncpp.a")

set(DROGON_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/x64/_deps/drogon-src/lib/inc")
set(DROGON_LIBRARIES "${CMAKE_SOURCE_DIR}/x64/_deps/drogon-build/lib/libdrogon.a")

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
