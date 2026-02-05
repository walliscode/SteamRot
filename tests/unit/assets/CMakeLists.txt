add_executable(test_assets
  AssetManager.test.cpp
)

target_include_directories(test_assets
PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(test_assets
  PRIVATE
  Catch2::Catch2WithMain
  assets
  types
  context_test_helpers
)

catch_discover_tests(test_assets)

