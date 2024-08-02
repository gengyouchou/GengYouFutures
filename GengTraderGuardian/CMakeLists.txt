cmake_minimum_required(VERSION 3.10)

# 项目名称
project(GengTraderGuardian C)

# 设置源文件
set(SOURCE_FILES GengTraderGuardian.c)

# 可执行文件输出路径
add_executable(GengTraderGuardian ${SOURCE_FILES})

# 链接所需的库（如果有的话）
# target_link_libraries(GengTraderGuardian libname)
