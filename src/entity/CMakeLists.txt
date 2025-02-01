add_library(entity
    EntityManager.cpp
    EntityMemoryPool.cpp
    ArchetypeManager.cpp
    Archetype.cpp
)

target_include_directories(entity PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(entity
  PUBLIC
  nlohmann_json::nlohmann_json
  SFML::Graphics
)
