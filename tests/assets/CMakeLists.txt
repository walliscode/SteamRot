add_executable(test_assets
  AssetManager.test.cpp
)

target_link_libraries(test_assets
  PRIVATE
  Catch2::Catch2WithMain
  assets
)

catch_discover_tests(test_assets)
