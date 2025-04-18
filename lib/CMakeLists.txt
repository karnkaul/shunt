project(shunt-lib)

message(STATUS "[${PROJECT_NAME}]")

add_library(${PROJECT_NAME})
add_library(shunt::lib ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC
  klib::klib
)

if(shunt_is_top_level)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    klib::klib-warnings
    klib::klib-ndwerror
  )
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
  include
)

target_include_directories(${PROJECT_NAME} PRIVATE
  src
)

file(GLOB_RECURSE sources LIST_DIRECTORIES false "include/shunt/*.hpp" "src/*.[hc]pp")
target_sources(${PROJECT_NAME} PRIVATE
  ${sources}
)
