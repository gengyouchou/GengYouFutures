cmake_minimum_required(VERSION 3.14)

# 设置项目名称和版本
project(GengYouFuturesUI VERSION 1.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# 引入 FetchContent 模块
include(FetchContent)

# 配置 Jsoncpp
FetchContent_Declare(
  jsoncpp
  GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
  GIT_TAG 1.9.5  # 固定版本号
)
FetchContent_MakeAvailable(jsoncpp)

# 配置 Drogon 框架
FetchContent_Declare(
  drogon
  GIT_REPOSITORY https://github.com/drogonframework/drogon.git
  GIT_TAG v1.9.1  # 固定版本号
)
FetchContent_MakeAvailable(drogon)

# 手动指定 Jsoncpp 的路径
set(JSONCPP_INCLUDE_DIRS ${jsoncpp_SOURCE_DIR}/include)
set(JSONCPP_LIBRARIES jsoncpp_lib)

# 添加可执行文件
add_executable(GengYouFuturesUI GengYouFuturesUI.cpp)

# 链接 Jsoncpp 和 Drogon
target_include_directories(GengYouFuturesUI PRIVATE ${JSONCPP_INCLUDE_DIRS})
target_link_libraries(GengYouFuturesUI PRIVATE Drogon::Drogon ${JSONCPP_LIBRARIES})
