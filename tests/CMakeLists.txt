project(shunt-tests)

file(GLOB_RECURSE sources LIST_DIRECTORIES false "test_*.cpp")

if(NOT "${sources}" STREQUAL "")
  add_executable(${PROJECT_NAME})
  target_link_libraries(${PROJECT_NAME} PRIVATE
    shunt::lib
    klib::klib-warnings
    klib::klib-ndwerror
    klib::klib-test-main
  )
  target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../lib/src"
    "${CMAKE_CURRENT_SOURCE_DIR}"
  )
  target_sources(${PROJECT_NAME} PRIVATE ${sources})
  add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
endif()
