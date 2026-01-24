add_library(user_interface
# UI element configuration moved to data_providers
)

target_include_directories(user_interface
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(user_interface
  PRIVATE
  SFML::Graphics
  types
  events
  data_providers

)

