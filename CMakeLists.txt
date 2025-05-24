
cmake_minimum_required (VERSION 3.31)

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

# make sure the data output directory exists
if (NOT EXISTS ${DATA_OUT_DIR})
  file(MAKE_DIRECTORY ${DATA_OUT_DIR})
endif()


# set the cmake module path (for import libraries and other cmake files)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/FetchContentModules")

# fetch_content_modules.cmake contains a menu list of all fetch content
# scripts to include. to keep top level cmake tidier
include(fetch_content_modules)


add_subdirectory(steamrot)
add_subdirectory(src)
add_subdirectory(tests)


