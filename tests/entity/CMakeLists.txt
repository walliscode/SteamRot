add_executable(test_entity
  EntityManager.test.cpp
  ArchetypeManager.test.cpp
  FlatbuffersConfigurator.test.cpp
  configuration_helpers.cpp

)

target_include_directories(test_entity
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_entity
  PRIVATE
  Catch2::Catch2WithMain
  entity
)

catch_discover_tests(test_entity)
