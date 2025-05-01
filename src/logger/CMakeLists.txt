add_library(logger
  Logger.cpp)

target_include_directories(logger
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}

)

target_link_libraries(logger
PUBLIC
spdlog
steamrot_directory_paths)
