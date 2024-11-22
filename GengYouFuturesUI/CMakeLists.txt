cmake_minimum_required(VERSION 3.15)
project(GengYouFuturesUI)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用 FetchContent 模块
include(FetchContent)

# 自动下载并配置 Crow
FetchContent_Declare(
  Crow
  GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(Crow)

# 自动下载并配置 Boost（Crow 的依赖库）
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG        boost-1.82.0  # 指定 Boost 版本
)
set(BOOST_SUBMODULES_INIT ON)
FetchContent_MakeAvailable(Boost)

# 自动下载并配置 OpenSSL（HTTPS 支持）
FetchContent_Declare(
  OpenSSL
  GIT_REPOSITORY https://github.com/openssl/openssl.git
  GIT_TAG        OpenSSL_1_1_1-stable
)
FetchContent_MakeAvailable(OpenSSL)

# 自动下载并配置 Asio（Crow 的另一个依赖）
FetchContent_Declare(
  Asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(Asio)

# 定义项目的可执行文件
add_executable(GengYouFuturesUI GengYouFuturesUI.cpp)

# 链接依赖库
target_include_directories(GengYouFuturesUI PRIVATE
  ${Boost_INCLUDE_DIRS}
  ${CROW_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${ASIO_INCLUDE_DIR}
)

target_link_libraries(GengYouFuturesUI PRIVATE
  Boost::headers
  Boost::boost
  OpenSSL::SSL
  Crow::Crow
)
