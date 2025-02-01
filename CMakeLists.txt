
cmake_minimum_required (VERSION 3.31)

project(SteamRot
     VERSION 0.1.0.0
 LANGUAGES CXX)

enable_testing()



# This next bit of code comes from Professional CMake: A Practical Guide, pg 205 (in documentation/textbooks) as a safe way to add CXX standards
# Require C++20, but let a parent project ask for something higher
if(DEFINED CMAKE_CXX_STANDARD)
  if(CMAKE_CXX_STANDARD MATCHES "^(90|99)$" OR CMAKE_CXX_STANDARD LESS 20)
    message(FATAL_ERROR "This project requires at least C++20")
  endif()
else()
  set(CMAKE_CXX_STANDARD 20)
endif()

# Always enforce the language constraint
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(RESOURCES_PATH "${CMAKE_SOURCE_DIR}/resources")
set(DATA_OUT_DIR "${CMAKE_SOURCE_DIR}/data_out") # for outputting data files from places such as somultations

# make sure the data output directory exists
if (NOT EXISTS ${DATA_OUT_DIR})
  file(MAKE_DIRECTORY ${DATA_OUT_DIR})
endif()

# set the cmake module path (for import libraries and other cmake files)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/FetchContentModules")
include(AddSFML)
include(AddCatch2)
include(AddJson)


add_subdirectory(steamrot)
add_subdirectory(src)


# add external libraries
include_directories(${CMAKE_SOURCE_DIR}/external)


