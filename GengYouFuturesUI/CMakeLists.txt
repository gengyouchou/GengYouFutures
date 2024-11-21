cmake_minimum_required(VERSION 3.14)

# 设置项目名称和版本
project(GengYouFuturesUI VERSION 1.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# 引入 FetchContent 模块
include(FetchContent)

# 配置 Drogon 框架
FetchContent_Declare(
  drogon
  GIT_REPOSITORY https://github.com/drogonframework/drogon.git
  GIT_TAG v1.9.1  # 固定版本号
)
FetchContent_MakeAvailable(drogon)

# 添加可执行文件
add_executable(GengYouFuturesUI GengYouFuturesUI.cpp)

# 链接 Drogon
target_link_libraries(GengYouFuturesUI PRIVATE Drogon::Drogon)
