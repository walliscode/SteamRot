add_executable(test_entity
  entity_memory.test.cpp
  EntityManager.test.cpp
  ArchetypeManager.test.cpp
  FlatbuffersEntityConfigurator.test.cpp
  archetype_helpers.test.cpp
)

target_include_directories(test_entity
PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_entity
  PUBLIC
  Catch2::Catch2WithMain
  entity
  context_test_helpers
  test_harness
)

catch_discover_tests(test_entity)

