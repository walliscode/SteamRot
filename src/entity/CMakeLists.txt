add_library(entity
    EntityManager.cpp
    EntityConfigurationFactory.cpp
    ArchetypeManager.cpp
    EntityHelpers.cpp
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
  data_handlers
  components
  logger
  flatbuffers
  flatbuffers_headers
)
