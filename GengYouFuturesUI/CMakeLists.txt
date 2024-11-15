cmake_minimum_required(VERSION 3.14)

# 设置项目名称和版本
project(GengYouFuturesUI VERSION 1.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# 引入 FetchContent 模块
include(FetchContent)

# FetchContent 用于引入 libwebsockets
FetchContent_Declare(
  libwebsockets
  GIT_REPOSITORY https://github.com/warmcat/libwebsockets.git
  GIT_TAG v4.3.1  # 可以根据需要指定版本
)

# 下载并构建 libwebsockets
FetchContent_MakeAvailable(libwebsockets)

# 设置 pthread（如果使用多线程）
find_package(Threads REQUIRED)

# 添加可执行文件（入口文件是 GengYouFuturesUI.cpp）
add_executable(GengYouFuturesUI GengYouFuturesUI.cpp)

# 链接 libwebsockets 和 pthread 库
target_link_libraries(GengYouFuturesUI PRIVATE libwebsockets Threads::Threads)

# 如果需要其他链接设置，可以在这里添加
# target_include_directories(GengYouFuturesUI PRIVATE ${libwebsockets_SOURCE_DIR}/include)
