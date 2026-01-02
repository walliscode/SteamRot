add_executable(test_harness
  FlatbuffersTestDataLoader.test.cpp
  FlatbuffersTestDataProvider.test.cpp

)


target_link_libraries(test_harness
  PRIVATE
  Catch2::Catch2WithMain
  harness
)

catch_discover_tests(test_harness)



