add_library(error_loop
  error_loop.cpp
)

target_include_directories(error_loop
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(error_loop
  PRIVATE
  SFML::Graphics
  types
)
