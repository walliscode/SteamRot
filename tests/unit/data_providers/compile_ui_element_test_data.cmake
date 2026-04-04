# Compile entity_test_data.json to binary for unit tests

set(UI_ELEMENT_TEST_JSON "${CMAKE_CURRENT_SOURCE_DIR}/data/ui_element_test_data.json")
set(UI_ELEMENT_TEST_BIN "${CMAKE_CURRENT_SOURCE_DIR}/data/ui_element_test_data.bin")
set(UI_ELEMENT_SCHEMA "${CMAKE_SOURCE_DIR}/src/types/flatbuffers/user_interface/user_interface.fbs")

# Error test data files
set(ERROR_CONTAINER_NO_CHILDREN_JSON "${CMAKE_CURRENT_SOURCE_DIR}/data/error_container_no_children.json")
set(ERROR_CONTAINER_NO_CHILDREN_BIN "${CMAKE_CURRENT_SOURCE_DIR}/data/error_container_no_children.bin")

set(ERROR_CONTAINER_ONE_CHILD_JSON "${CMAKE_CURRENT_SOURCE_DIR}/data/error_container_one_child.json")
set(ERROR_CONTAINER_ONE_CHILD_BIN "${CMAKE_CURRENT_SOURCE_DIR}/data/error_container_one_child.bin")

set(ERROR_CONTAINER_WRONG_FIRST_CHILD_JSON "${CMAKE_CURRENT_SOURCE_DIR}/data/error_container_wrong_first_child.json")
set(ERROR_CONTAINER_WRONG_FIRST_CHILD_BIN "${CMAKE_CURRENT_SOURCE_DIR}/data/error_container_wrong_first_child.bin")

set(ERROR_CONTAINER_WRONG_SECOND_CHILD_JSON "${CMAKE_CURRENT_SOURCE_DIR}/data/error_container_wrong_second_child.json")
set(ERROR_CONTAINER_WRONG_SECOND_CHILD_BIN "${CMAKE_CURRENT_SOURCE_DIR}/data/error_container_wrong_second_child.bin")

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

add_custom_command(
  OUTPUT "${ERROR_CONTAINER_NO_CHILDREN_BIN}"
  COMMAND flatc
  ARGS --binary
  ARGS -o "${CMAKE_CURRENT_SOURCE_DIR}/data"
  ARGS "${UI_ELEMENT_SCHEMA}"
  ARGS "${ERROR_CONTAINER_NO_CHILDREN_JSON}"
  DEPENDS "${ERROR_CONTAINER_NO_CHILDREN_JSON}" "${UI_ELEMENT_SCHEMA}"
  COMMENT "Compiling error test data: error_container_no_children.json -> error_container_no_children.bin"
  VERBATIM
)

add_custom_command(
  OUTPUT "${ERROR_CONTAINER_ONE_CHILD_BIN}"
  COMMAND flatc
  ARGS --binary
  ARGS -o "${CMAKE_CURRENT_SOURCE_DIR}/data"
  ARGS "${UI_ELEMENT_SCHEMA}"
  ARGS "${ERROR_CONTAINER_ONE_CHILD_JSON}"
  DEPENDS "${ERROR_CONTAINER_ONE_CHILD_JSON}" "${UI_ELEMENT_SCHEMA}"
  COMMENT "Compiling error test data: error_container_one_child.json -> error_container_one_child.bin"
  VERBATIM
)

add_custom_command(
  OUTPUT "${ERROR_CONTAINER_WRONG_FIRST_CHILD_BIN}"
  COMMAND flatc
  ARGS --binary
  ARGS -o "${CMAKE_CURRENT_SOURCE_DIR}/data"
  ARGS "${UI_ELEMENT_SCHEMA}"
  ARGS "${ERROR_CONTAINER_WRONG_FIRST_CHILD_JSON}"
  DEPENDS "${ERROR_CONTAINER_WRONG_FIRST_CHILD_JSON}" "${UI_ELEMENT_SCHEMA}"
  COMMENT "Compiling error test data: error_container_wrong_first_child.json -> error_container_wrong_first_child.bin"
  VERBATIM
)

add_custom_command(
  OUTPUT "${ERROR_CONTAINER_WRONG_SECOND_CHILD_BIN}"
  COMMAND flatc
  ARGS --binary
  ARGS -o "${CMAKE_CURRENT_SOURCE_DIR}/data"
  ARGS "${UI_ELEMENT_SCHEMA}"
  ARGS "${ERROR_CONTAINER_WRONG_SECOND_CHILD_JSON}"
  DEPENDS "${ERROR_CONTAINER_WRONG_SECOND_CHILD_JSON}" "${UI_ELEMENT_SCHEMA}"
  COMMENT "Compiling error test data: error_container_wrong_second_child.json -> error_container_wrong_second_child.bin"
  VERBATIM
)

# Create a custom target that depends on the compiled test data
add_custom_target(compile_ui_element_unit_test_data ALL
  DEPENDS "${UI_ELEMENT_TEST_BIN}"
          "${ERROR_CONTAINER_NO_CHILDREN_BIN}"
          "${ERROR_CONTAINER_ONE_CHILD_BIN}"
          "${ERROR_CONTAINER_WRONG_FIRST_CHILD_BIN}"
          "${ERROR_CONTAINER_WRONG_SECOND_CHILD_BIN}"
  COMMENT "Compiling ui element unit test data"
)

# Make it depend on schema generation
add_dependencies(compile_ui_element_unit_test_data  flatbuffers_generate_headers)
