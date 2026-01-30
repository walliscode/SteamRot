# Compile entity test data
include(compile_test_data.cmake)

add_executable(test_entity
  entity_memory.test.cpp
  archetypes.test.cpp
  EntityManager.test.cpp
  ArchetypeManager.test.cpp
  FlatbuffersEntityConfigurator.test.cpp
  # FlatbuffersEntityImporter.test.cpp
  FlatbuffersUIElementProvider.test.cpp

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


)

# Ensure test data is compiled before building tests
add_dependencies(test_entity compile_entity_unit_test_data)

catch_discover_tests(test_entity)

