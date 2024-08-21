# File: CMakeLists.txt

# 指定所需的最低 CMake 版本
cmake_minimum_required(VERSION 3.10)

# 项目信息
project(GengTraderGuardian)

# 设置 C++ 标准为 17 并指定为必需
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# if(NOT CMAKE_BUILD_TYPE)
#     set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
# endif()

# 设置源文件
set(SOURCE_FILES GengTraderGuardian.cpp)

# 可执行文件输出路径
add_executable(GengTraderGuardian ${SOURCE_FILES})

# 链接所需的库（如果有的话）
# target_link_libraries(GengTraderGuardian libname)




# 添加多个源文件：
# 如果你的项目包含多个源文件，你可以将它们添加到 set(SOURCE_FILES ...) 中，例如：

# cmake
# 複製程式碼
# set(SOURCE_FILES GengTraderGuardian.cpp another_file.cpp yet_another_file.cpp)
# 链接库：
# 如果你的项目依赖于外部库，比如你之前提到的 CSKCenterLib、CSKQuoteLib 等，可以使用 target_link_libraries 来链接它们：

# cmake
# 複製程式碼
# target_link_libraries(GengTraderGuardian CSKCenterLib CSKQuoteLib)
# 添加包含路径：
# 如果你有额外的头文件路径，可以使用 include_directories 来添加：

# cmake
# 複製程式碼
# include_directories(/path/to/include)
# 设置编译器选项：
# 如果你需要指定一些编译器选项，可以使用 target_compile_options：

# cmake
# 複製程式碼
# target_compile_options(GengTraderGuardian PRIVATE -Wall -Wextra)
# 以上这些都是常见的 CMake 用法，可以根据你的实际需求进行调整。你目前的配置适用于一个简单的项目，但如果需要更复杂的设置，可以随时扩展这个文件。