
# Compile entity_test_data.json to binary for unit tests

set(SCENE_TEST_JSON "${CMAKE_CURRENT_SOURCE_DIR}/test.scene_data.json")
set(SCENE_TEST_BIN "${CMAKE_CURRENT_SOURCE_DIR}/test.scene_data.bin")
set(SCENE_DATA_SCHEMA "${CMAKE_SOURCE_DIR}/src/types/flatbuffers/entities/scene_data.fbs")

# Add custom command to compile JSON to binary
add_custom_command(
  OUTPUT "${SCENE_TEST_BIN}"
  COMMAND flatc
  ARGS --binary
  ARGS -o "${CMAKE_CURRENT_SOURCE_DIR}/data"
  ARGS "${SCENE_DATA_SCHEMA}"
  ARGS "${SCENE_TEST_JSON}"
  DEPENDS "${SCENE_TEST_JSON}" "${SCENE_DATA_SCHEMA}"
COMMENT "Compiling scene test data: test.scene_data.json -> test.scene_data.bin"
  VERBATIM
)

# Create a custom target that depends on the compiled test data
add_custom_target(compile_test_scene_data ALL
  DEPENDS "${SCENE_TEST_BIN}"
COMMENT "Compiling test scene data"
)

# Make it depend on schema generation
add_dependencies(compile_test_scene_data flatbuffers_generate_headers)
