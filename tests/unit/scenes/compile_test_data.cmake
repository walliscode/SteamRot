# Compile test_scene.scene_data.json to binary for unit tests

set(SCENE_TEST_JSON "${CMAKE_CURRENT_SOURCE_DIR}/data/test_scene.scene_data.json")
set(SCENE_TEST_BIN "${CMAKE_CURRENT_SOURCE_DIR}/data/test_scene.scene_data.bin")
set(SCENE_DATA_SCHEMA "${CMAKE_SOURCE_DIR}/src/types/flatbuffers/scenes/scene_data.fbs")

# Add custom command to compile JSON to binary
add_custom_command(
  OUTPUT "${SCENE_TEST_BIN}"
  COMMAND flatc
  ARGS --binary
  ARGS -o "${CMAKE_CURRENT_SOURCE_DIR}/data"
  ARGS "${SCENE_DATA_SCHEMA}"
  ARGS "${SCENE_TEST_JSON}"
  DEPENDS "${SCENE_TEST_JSON}" "${SCENE_DATA_SCHEMA}"
  COMMENT "Compiling scene test data: test_scene.scene_data.json -> test_scene.scene_data.bin"
  VERBATIM
)

# Create a custom target that depends on the compiled test data
add_custom_target(compile_scene_unit_test_data ALL
  DEPENDS "${SCENE_TEST_BIN}"
  COMMENT "Compiling scene unit test data"
)

# Make it depend on schema generation
add_dependencies(compile_scene_unit_test_data flatbuffers_generate_headers)
