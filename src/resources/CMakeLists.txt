add_library(resources
  GameResources.h
  SceneResources.h
  ResourceConfigurator.h
  ResourceConfigurator.cpp
)

target_link_libraries(resources
  PUBLIC
    sfml-graphics
    assets
    events
    config
    entity
    flatbuffers_headers
)

target_include_directories(resources
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
