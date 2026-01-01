add_executable(test_harness
  FlatbuffersTestDataProvider.test.cpp
)

target_link_directories(test_harness
  PRIVATE
  Catch2::Catch2WithMain
  harness
)


