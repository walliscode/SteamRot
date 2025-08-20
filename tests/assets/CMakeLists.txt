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
  asset_helpers
)

catch_discover_tests(test_assets)

add_library(asset_helpers
  asset_helpers.cpp
)

target_include_directories(asset_helpers
PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(asset_helpers
  PUBLIC
  Catch2::Catch2WithMain
  assets
)
