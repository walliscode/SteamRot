add_executable(test_entity
  EntityManager.test.cpp
  ArchetypeManager.test.cpp
  FlatbuffersConfigurator.test.cpp


)

target_include_directories(test_entity
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_entity
  PRIVATE
  Catch2::Catch2WithMain
  entity
  entity_helpers
)

catch_discover_tests(test_entity)

add_library(entity_helpers
configuration_helpers.cpp
)

target_include_directories(entity_helpers
PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(entity_helpers
  PUBLIC
  Catch2::Catch2WithMain
  entity
)
