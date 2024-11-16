cmake_minimum_required(VERSION 3.14)

# 设置项目名称和版本
project(GengYouFuturesUI VERSION 1.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# 引入 FetchContent 模块
include(FetchContent)

# FetchContent 用于引入 Crow 框架
FetchContent_Declare(
  crow
  GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
  GIT_TAG master  # 选择最新版本
)

# 如果需要 SSL 支持（例如使用 Boost 和 OpenSSL）
option(CROW_ENABLE_SSL "Enable SSL support in Crow" ON)

# FetchContent 用于引入 Boost 库（Crow 依赖）
FetchContent_Declare(
  boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.81.0  # 可根据需要选择版本
)
set(BOOST_BUILD_CMAKE ON CACHE BOOL "" FORCE)

# 下载并构建 Crow 和 Boost
FetchContent_MakeAvailable(crow)
FetchContent_MakeAvailable(boost)

# 查找线程库
find_package(Threads REQUIRED)

# 添加可执行文件（入口文件是 GengYouFuturesUI.cpp）
add_executable(GengYouFuturesUI GengYouFuturesUI.cpp)

# 链接 Crow 和 Boost 库
target_link_libraries(GengYouFuturesUI PRIVATE crow Threads::Threads)

# 如果启用了 SSL 支持，链接 OpenSSL
if(CROW_ENABLE_SSL)
    find_package(OpenSSL REQUIRED)
    target_link_libraries(GengYouFuturesUI PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()
