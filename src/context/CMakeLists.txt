add_library(context
  GameContext.cpp
  SceneContext.cpp
)

target_include_directories(context
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(context
  PRIVATE
  SFML::Graphics
  types
  resources
  assets
  events
  entity

)
