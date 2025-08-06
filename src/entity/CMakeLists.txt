add_library(entity
    EntityManager.cpp
    EntityConfigurator.cpp
    FlatbuffersConfigurator.cpp
    ArchetypeManager.cpp
    emp_helpers.cpp
  )

target_include_directories(entity
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}

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
