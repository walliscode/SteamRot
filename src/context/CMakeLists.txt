add_library(context
  GameContext.cpp
  GameContextBuilder.cpp
  ContextConfigurator.cpp
  ContextDirector.cpp
)

target_include_directories(context
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(context PUBLIC
  SFML::Graphics
  systems
  logic
  flatbuffers_headers
)
