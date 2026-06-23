add_library(assets_for_tests
  test_fonts.cpp
)

target_include_directories(assets_for_tests PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(assets_for_tests PUBLIC
    types
    data_providers

)
