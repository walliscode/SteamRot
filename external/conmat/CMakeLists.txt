
# CMake 3.31 is the latest stable version at the time of this update
# This uses modern CMake practices and is forward-compatible with CMake 4.x
cmake_minimum_required(VERSION 3.31 FATAL_ERROR)

project(conmat
  VERSION 0.1.0
  LANGUAGES CXX)

# Only enable testing and add test subdirectory if this is the top-level project
# PROJECT_IS_TOP_LEVEL is a built-in CMake variable available since CMake 3.21
if(PROJECT_IS_TOP_LEVEL)
  enable_testing()
endif()

# This next bit of code comes from Professional CMake: A Practical Guide,
# pg 205 (in documentation/textbooks) as a safe way to add CXX standards
# Require C++23, but let a parent project ask for something higher
if(DEFINED CMAKE_CXX_STANDARD)
  if(CMAKE_CXX_STANDARD MATCHES "^(90|99)$" OR CMAKE_CXX_STANDARD LESS 23)
    message(FATAL_ERROR "This project requires at least C++23")
  endif()
else()
  set(CMAKE_CXX_STANDARD 23)
endif()

# Always enforce the language constraint
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Only add fetch content modules if this is the top-level project
if(PROJECT_IS_TOP_LEVEL)
  # set the cmake module path (for import libraries and other cmake files)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/fetch-content-modules")

  # fetch_content_modules.cmake contains a menu list of all fetch content
  # scripts to include. to keep top level cmake tidier
  include(fetch-content-modules)

  add_subdirectory(config)
endif()

# Always add the main library
add_subdirectory(src)

# Only add tests and demo when this is the top-level project
if(PROJECT_IS_TOP_LEVEL)
  add_subdirectory(tests)
endif()



