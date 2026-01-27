# reporters library
# providing custom output formats for test results


add_library(reporters_objects OBJECT
  HarnessReporter.cpp
)

target_link_libraries(reporters_objects
  PUBLIC
    Catch2::Catch2
    conmat::conmat
)

target_include_directories(reporters_objects
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
