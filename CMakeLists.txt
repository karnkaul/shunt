cmake_minimum_required(VERSION 3.23)

project(shunting-yard)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "-d")

add_library(shunt-warnings INTERFACE)
add_library(shunt::warnings ALIAS shunt-warnings)

if(CMAKE_CXX_COMPILER_ID STREQUAL Clang OR CMAKE_CXX_COMPILER_ID STREQUAL GNU)
  target_compile_options(shunt-warnings INTERFACE
    -Wall -Wextra -Wpedantic -Wconversion -Werror=return-type
  )
endif()

add_subdirectory(lib)
add_subdirectory(lab)
