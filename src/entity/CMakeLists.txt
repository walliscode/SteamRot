add_library(entity
    EntityManager.cpp
    FlatbuffersEntityConfigurator.cpp
    FlatbuffersEntityImporter.cpp
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
  interfaces
  events
  user_interface
  configuration
)
