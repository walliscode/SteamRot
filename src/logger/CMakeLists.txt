add_library(logger
  Logger.cpp
  log_handler.cpp)

target_include_directories(logger
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}

)

target_link_libraries(logger
PUBLIC
spdlog
config)
