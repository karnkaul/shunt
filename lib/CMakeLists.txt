project(shunt-lib)

add_library(${PROJECT_NAME})
add_library(shunt::lib ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
  include
)

target_include_directories(${PROJECT_NAME} PRIVATE
  src
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  shunt::warnings
)

file(GLOB_RECURSE sources LIST_DIRECTORIES false "include/shunt/*.hpp" "src/*.[hc]pp")
target_sources(${PROJECT_NAME} PRIVATE
  ${sources}
)
