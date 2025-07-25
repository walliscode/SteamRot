# generate the steamrot_directory_paths.cpp file based on whether
# the executable has specified TEST_DIRECTORIES

if(USE_TEST_DIRECTORIES)
  cmake_path(SET data_path ${CMAKE_SOURCE_DIR}/tests/test_data)

else()
  cmake_path(SET data_path ${CMAKE_SOURCE_DIR}/data)
endif()

configure_file(
  steamrot_directory_paths.cpp.in
  steamrot_directory_paths.cpp
  @ONLY)

add_library(steamrot_directory_paths STATIC
  ${CMAKE_CURRENT_BINARY_DIR}/steamrot_directory_paths.cpp)

target_include_directories(steamrot_directory_paths PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)
