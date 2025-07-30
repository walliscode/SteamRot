add_executable(test_entity
  EntityConfigurationFactory.test.cpp
)

target_link_libraries(test_entity
PRIVATE
  Catch2::Catch2WithMain
  entity
)

catch_discover_tests(test_entity)
