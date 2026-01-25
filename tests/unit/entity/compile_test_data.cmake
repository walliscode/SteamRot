# Compile entity_test_data.json to binary for unit tests

set(ENTITY_TEST_JSON "${CMAKE_CURRENT_SOURCE_DIR}/data/entity_test_data.json")
set(ENTITY_TEST_BIN "${CMAKE_CURRENT_SOURCE_DIR}/data/entity_test_data.bin")
set(ENTITIES_SCHEMA "${CMAKE_SOURCE_DIR}/src/types/flatbuffers/entities/entities.fbs")

set(UI_ELEMENT_TEST_JSON "${CMAKE_CURRENT_SOURCE_DIR}/data/ui_element_test_data.json")
set(UI_ELEMENT_TEST_BIN "${CMAKE_CURRENT_SOURCE_DIR}/data/ui_element_test_data.bin")
set(UI_ELEMENT_SCHEMA "${CMAKE_SOURCE_DIR}/src/types/flatbuffers/entities/user_interface.fbs")

# Add custom command to compile JSON to binary
add_custom_command(
  OUTPUT "${ENTITY_TEST_BIN}"
  COMMAND flatc
  ARGS --binary
  ARGS -o "${CMAKE_CURRENT_SOURCE_DIR}/data"
  ARGS "${ENTITIES_SCHEMA}"
  ARGS "${ENTITY_TEST_JSON}"
  DEPENDS "${ENTITY_TEST_JSON}" "${ENTITIES_SCHEMA}"
  COMMENT "Compiling entity test data: entity_test_data.json -> entity_test_data.bin"
  VERBATIM
)

add_custom_command(
  OUTPUT "${UI_ELEMENT_TEST_BIN}"
  COMMAND flatc
  ARGS --binary
  ARGS -o "${CMAKE_CURRENT_SOURCE_DIR}/data"
  ARGS "${UI_ELEMENT_SCHEMA}"
  ARGS "${UI_ELEMENT_TEST_JSON}"
  DEPENDS "${UI_ELEMENT_TEST_JSON}" "${UI_ELEMENT_SCHEMA}"
  COMMENT "Compiling UI element test data: ui_element_test_data.json -> ui_element_test_data.bin"
  VERBATIM
)

# Create a custom target that depends on the compiled test data
add_custom_target(compile_entity_unit_test_data ALL
  DEPENDS "${ENTITY_TEST_BIN}" "${UI_ELEMENT_TEST_BIN}"
  COMMENT "Compiling entity unit test data"
)

# Make it depend on schema generation
add_dependencies(compile_entity_unit_test_data flatbuffers_generate_headers)
