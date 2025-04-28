add_library(entity
    EntityManager.cpp
    EntityConfigurationFactory.cpp
    EntityMemoryPool.cpp
    ArchetypeManager.cpp
    Archetype.cpp
)

target_include_directories(entity
  PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/include/utils
)

target_link_libraries(entity
  PUBLIC
  nlohmann_json::nlohmann_json
  SFML::Graphics
  components
  steamrot_enums
)
