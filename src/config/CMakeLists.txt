cmake_path(SET data_path ${CMAKE_SOURCE_DIR}/data)
cmake_path(SET test_data_path ${CMAKE_SOURCE_DIR}/tests/data)

configure_file(
  directory_paths.cpp.in
  directory_paths.cpp
  @ONLY)


add_library(config STATIC
  ${CMAKE_CURRENT_BINARY_DIR}/directory_paths.cpp)

target_include_directories(config PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)
