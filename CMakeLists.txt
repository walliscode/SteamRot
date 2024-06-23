
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



# ensures that the default libraries are statically linked
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
message("Build shared libs: ${BUILD_SHARED_LIBS}")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # necessary for puting the executable and dlls in the same folder
set(CMAKE_PROGRAM_PATH ${CMAKE_BINARY_DIR}/bin) # points other CMake searches to the binary directory

# messages for posterity
message("CMake runtime output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message("CMake program path: ${CMAKE_PROGRAM_PATH}")
message("Cmake source dir: ${CMAKE_SOURCE_DIR}")
message("Cmake binary dir: ${CMAKE_BINARY_DIR}")

# set the cmake module path (for import libraries and other cmake files)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/FetchContentModules")
include(AddSFML)
include(AddCatch2)


add_subdirectory(external/flatbuffers)
include(Addflatbuffers)
add_subdirectory(src)

add_subdirectory(tests)

# declaring some variables and shit

set(RESOURCES_PATH "${CMAKE_SOURCE_DIR}/resources/")
set(FB_BINARIES_PATH "${RESOURCES_PATH}binaries/")

target_compile_definitions(EntityManager PRIVATE
    FB_BINARIES_PATH="${FB_BINARIES_PATH}")
    
target_compile_definitions(Assets PRIVATE RESOURCES_PATH="${RESOURCES_PATH}")
target_compile_definitions(Assets PRIVATE
    FB_BINARIES_PATH="${FB_BINARIES_PATH}")
    



install(TARGETS SteamRot) # creates an install folder under installDir (in CMakePresets.json)

# useful function and variables
function(check_targets target_list) # this function can be called in any cmakelists.txt file to check if targets are being propagated
    foreach(target ${${target_list}})
        if(TARGET ${target})
            message(STATUS "In directory ${CMAKE_CURRENT_SOURCE_DIR}, target '${target}' has been defined.")
        else()
            message(WARNING "In directory ${CMAKE_CURRENT_SOURCE_DIR}, target '${target}' has not been defined.")
        endif()
    endforeach()
endfunction()



set(MY_TARGET_LIST 
    SteamRot
    GameEngine
    sfml-graphics
    Assets
    flatbuffers
    Scene
    utils
    )

check_targets(MY_TARGET_LIST)

# for if you need to print out all available cmake variables into the cmake terminal
#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
 #  message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

