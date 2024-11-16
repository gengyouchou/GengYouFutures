cmake_minimum_required(VERSION 3.14)

# 设置项目名称和版本
project(GengYouFuturesUI VERSION 1.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# 引入 FetchContent 模块
include(FetchContent)

# 配置 Boost（Crow 依赖 Boost.Asio）
FetchContent_Declare(
  boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.81.0
)
# 启用 Boost 的 CMake 支持
set(BOOST_BUILD_CMAKE ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(boost)

# 配置 Crow 框架
FetchContent_Declare(
  crow
  GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
  GIT_TAG master
)
# 禁用 Crow 的测试，避免对 Asio 的额外查找需求
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(crow)

# 配置 Crow 的 SSL 支持（可选）
option(CROW_ENABLE_SSL "Enable SSL support in Crow" ON)

# 如果启用 SSL，查找 OpenSSL
if(CROW_ENABLE_SSL)
    find_package(OpenSSL REQUIRED)
endif()

# 添加可执行文件
add_executable(GengYouFuturesUI GengYouFuturesUI.cpp)

# 链接 Crow 和 Boost
target_link_libraries(GengYouFuturesUI PRIVATE
    crow
    Boost::headers  # 只需要头文件，Asio 是 header-only
)

# 如果启用 SSL，链接 OpenSSL
if(CROW_ENABLE_SSL)
    target_link_libraries(GengYouFuturesUI PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()

# 添加 include 路径
target_include_directories(GengYouFuturesUI PRIVATE
    ${boost_SOURCE_DIR}  # Boost 源码目录作为 include 路径
)
