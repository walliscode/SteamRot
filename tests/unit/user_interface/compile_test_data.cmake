# Compile entity_test_data.json to binary for unit tests

set(UI_ELEMENT_TEST_JSON "${CMAKE_CURRENT_SOURCE_DIR}/data/ui_element_test_data.json")
set(UI_ELEMENT_TEST_BIN "${CMAKE_CURRENT_SOURCE_DIR}/data/ui_element_test_data.bin")
set(UI_ELEMENT_SCHEMA "${CMAKE_SOURCE_DIR}/src/types/flatbuffers/entities/user_interface.fbs")

# Add custom command to compile JSON to binary
add_custom_command(
  OUTPUT "${UI_ELEMENT_TEST_BIN}"
  COMMAND flatc
  ARGS --binary
  ARGS -o "${CMAKE_CURRENT_SOURCE_DIR}/data"
  ARGS "${UI_ELEMENT_SCHEMA}"
  ARGS "${UI_ELEMENT_TEST_JSON}"
  DEPENDS "${UI_ELEMENT_TEST_JSON}" "${UI_ELEMENT_SCHEMA}"
  COMMENT "Compiling user_interface test data: ui_element_test_data.json -> ui_element_test_data.bin"
  VERBATIM
)

# Create a custom target that depends on the compiled test data
add_custom_target(compile_ui_element_unit_test_data ALL
  DEPENDS "${UI_ELEMENT_TEST_BIN}"
  COMMENT "Compiling ui element unit test data"
)

# Make it depend on schema generation
add_dependencies(compile_ui_element_unit_test_data  flatbuffers_generate_headers)
