add_library(entity
    EntityManager.cpp
    FlatbuffersEntityConfigurator.cpp
    ArchetypeManager.cpp
    entity_memory.cpp
)

target_include_directories(entity
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}

)

target_link_libraries(entity
  PUBLIC
  SFML::Graphics
  components
  types
  events
  data_providers
  user_interface
)
