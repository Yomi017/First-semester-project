cmake_minimum_required(VERSION 3.0)
project(CALC)

# 设定项目包含目录（假设 project.h 在当前目录）
include_directories(${CMAKE_SOURCE_DIR})  # 将当前项目目录加入包含路径

# 编译生成可执行文件
add_executable(project lex.cpp project.cpp)