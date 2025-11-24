
cmake_minimum_required (VERSION 3.31 FATAL_ERROR)

project(SteamRot
  VERSION 0.1.0.0
  LANGUAGES CXX)

enable_testing()

# This next bit of code comes from Professional CMake: A Practical Guide, pg 205 (in documentation/textbooks) as a safe way to add CXX standards
# Require C++20, but let a parent project ask for something higher
if(DEFINED CMAKE_CXX_STANDARD)
  if(CMAKE_CXX_STANDARD MATCHES "^(90|99)$" OR CMAKE_CXX_STANDARD LESS 23)
    message(FATAL_ERROR "This project requires at least C++23")
  endif()
else()
  set(CMAKE_CXX_STANDARD 23)
endif()

# Always enforce the language constraint
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# If using Clang, use libc++ for full C++23 support (including std::expected)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_options(-stdlib=libc++)
  add_link_options(-stdlib=libc++ -lc++abi)
endif()

# make sure the data output directory exists
if (NOT EXISTS ${DATA_OUT_DIR})
  file(MAKE_DIRECTORY ${DATA_OUT_DIR})
endif()


# set the cmake module path (for import libraries and other cmake files)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/FetchContentModules")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/FlatbuffersCompilation")

# set project wide variables
cmake_path(SET data_dir ${CMAKE_SOURCE_DIR}/data)
cmake_path(SET test_data_dir ${CMAKE_SOURCE_DIR}/tests/data)

# Include flatbuffers compilation scripts
include(flatbuffers_compilation)

# fetch_content_modules.cmake contains a menu list of all fetch content
include(fetch_content_modules)

add_subdirectory(steamrot)
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(docs)


