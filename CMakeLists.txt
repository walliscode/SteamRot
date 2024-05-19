# Root CMakeLists.txt

cmake_minimum_required(VERSION 3.8)
project(SteamRot VERSION 1.0 LANGUAGES CXX)

# Enable Hot Reload for MSVC compilers if supported
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT 
    "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Set the cmake path for fetch content
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/FetchContentModules")
include(AddSFML)
include(AddCatch2)

# Add the executable
add_executable(SteamRot "src/main.cpp")

# Define the assets path
add_definitions(-DASSETS_PATH=\"${CMAKE_SOURCE_DIR}/assets\")

# Include sub-directories
add_subdirectory("src")

# Link SFML library
target_link_libraries(SteamRot sfml-system sfml-window sfml-graphics)

# Set warning level to /W4 and ensure no conflicting warning levels are set
if (MSVC)
  target_compile_options(SteamRot PRIVATE /W4)
endif()

# Set the C++ standard
set_property(TARGET SteamRot PROPERTY CXX_STANDARD 20)

# Set the directory for DLLs
set(DLL_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Define SFML DLL paths
if (WIN32)
  set(SFML_GRAPHICS_DLL "${CMAKE_BINARY_DIR}/_deps/sfml-build/lib/sfml-graphics-d-2.dll")
  set(SFML_WINDOW_DLL "${CMAKE_BINARY_DIR}/_deps/sfml-build/lib/sfml-window-d-2.dll")
  set(SFML_SYSTEM_DLL "${CMAKE_BINARY_DIR}/_deps/sfml-build/lib/sfml-system-d-2.dll")
  set(OPENAL_DLL "${CMAKE_BINARY_DIR}/_deps/sfml-src/extlibs/bin/x64/openal32.dll")

  # Ensure the output directory exists
  file(MAKE_DIRECTORY "${DLL_OUTPUT_DIRECTORY}")

  # Add custom commands to copy each DLL after build
  add_custom_command(TARGET SteamRot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${SFML_GRAPHICS_DLL}"
      "${DLL_OUTPUT_DIRECTORY}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${SFML_WINDOW_DLL}"
      "${DLL_OUTPUT_DIRECTORY}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${SFML_SYSTEM_DLL}"
      "${DLL_OUTPUT_DIRECTORY}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${OPENAL_DLL}"
      "${DLL_OUTPUT_DIRECTORY}"
  )
endif()