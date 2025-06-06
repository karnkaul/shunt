cmake_minimum_required(VERSION 3.23)

project(shunting-yard)

option(SHUNT_BUILD_CLI "Build shunt-cli" ${PROJECT_IS_TOP_LEVEL})
option(SHUNT_BUILD_TESTS "Build shunt-tests" ${PROJECT_IS_TOP_LEVEL})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "-d")

set(shunt_is_top_level ${PROJECT_IS_TOP_LEVEL})

add_subdirectory(ext)
add_subdirectory(lib)

if(SHUNT_BUILD_CLI)
  add_subdirectory(cli)
endif()

if(SHUNT_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
