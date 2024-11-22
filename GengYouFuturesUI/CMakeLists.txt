cmake_minimum_required(VERSION 3.15)
project(GengYouFuturesUI)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 定义项目的可执行文件
add_executable(GengYouFuturesUI GengYouFuturesUI.cpp)

# Windows 特定设置（用于命名管道支持）
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_CRT_SECURE_NO_WARNINGS")  # 禁用一些不必要的安全警告
endif()

# 在 Windows 上编译时自动链接 Windows 库（如 user32 和 kernel32）
target_link_libraries(GengYouFuturesUI PRIVATE
    kernel32
    user32
)
