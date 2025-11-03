add_executable(test_entity
  EntityManager.test.cpp
  ArchetypeManager.test.cpp
  ArchetypeUtils.test.cpp
  FlatbuffersConfigurator.test.cpp
  entity_test_helpers.test.cpp
)

target_include_directories(test_entity
PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_entity
  PUBLIC
  Catch2::Catch2WithMain
  entity
  entity_helpers
  context_test_helpers
  test_harness
  )

catch_discover_tests(test_entity)

add_library(entity_helpers
entity_test_helpers.cpp
)

target_include_directories(entity_helpers
PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(entity_helpers
  PUBLIC
  Catch2::Catch2WithMain
  entity
  user_interface_helpers
  test_matchers
)
