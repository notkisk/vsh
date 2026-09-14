cmake_minimum_required(VERSION 3.20)

project(shell)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#don't fall back to older versions of c++ standard if c++23 is not available
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# this exports a compile_commands.json file containing compilation commands for every
#source file, this is so clangd stop complaining

add_executable(shell
  src/main.cpp
  src/builtins.cpp
  src/path.cpp
  src/process.cpp
  src/tokenizer.cpp
)

target_include_directories(shell PRIVATE include)

target_compile_options(shell PRIVATE
  -Wall
  -Wextra
  # -pedantic-errors
)
