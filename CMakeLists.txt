
cmake_minimum_required (VERSION 3.28)

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


# We don't need compiler extensions, but let a parent ask for them
if(NOT DEFINED CMAKE_CXX_EXTENSIONS)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # necessary for puting the executable and dlls in the same folder
# Include external dependencies.
# set the cmake module path (for import libraries and other cmake files)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/FetchContentModules")
include(AddSFML)
include(AddCatch2)

add_subdirectory(src)
add_subdirectory(tests)


install(TARGETS SteamRot) # creates an install folder under installDir (in CMakePresets.json)
