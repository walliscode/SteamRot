add_library(resources
  EngineResources.cpp
)
target_include_directories(resources
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(resources
  PUBLIC
  types
  assets
  events
  data_providers
)


