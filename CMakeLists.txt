
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

# Disable C++20 module scanning since this project uses traditional headers
# CMake 3.31+ with Ninja + Clang enables module scanning by default
# This project does not use C++20 modules, only traditional #pragma once headers
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# set the cmake module path (for import libraries and other cmake files)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/FetchContentModules")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/FlatbuffersCompilation")

# set project wide variables
cmake_path(SET data_dir ${CMAKE_SOURCE_DIR}/data)
cmake_path(SET test_data_dir ${CMAKE_SOURCE_DIR}/tests/data)


include(flatbuffers_compilation)

# fetch_content_modules.cmake contains a menu list of all fetch content
include(fetch_content_modules)

# Set compile and link options for our project targets only (not dependencies)
# Use libc++ with clang for C++23 features like std::expected
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(PROJECT_COMPILE_OPTIONS -stdlib=libc++)
  set(PROJECT_LINK_OPTIONS -stdlib=libc++)
endif()

add_subdirectory(steamrot)
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(docs)

# Include test dashboard generator target (Python tool for visualizing test data)
include(${CMAKE_SOURCE_DIR}/cmake/GenerateTestDashboard.cmake)

