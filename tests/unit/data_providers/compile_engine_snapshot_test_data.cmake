# Compile engine_snapshot_test_data.json to binary format for testing

set(ENGINE_SNAPSHOT_TEST_JSON "${CMAKE_CURRENT_SOURCE_DIR}/data/engine_snapshot_test_data.json")
set(ENGINE_SNAPSHOT_TEST_BIN "${CMAKE_CURRENT_SOURCE_DIR}/data/engine_snapshot_test_data.bin")
set(ENGINE_SNAPSHOT_SCHEMA "${CMAKE_SOURCE_DIR}/src/types/flatbuffers/core/engine_snapshot.fbs")

# Custom command to compile JSON to binary using flatc
add_custom_command(
  OUTPUT "${ENGINE_SNAPSHOT_TEST_BIN}"
  COMMAND flatc --binary -o "${CMAKE_CURRENT_SOURCE_DIR}/data"
  ARGS "${ENGINE_SNAPSHOT_SCHEMA}" "${ENGINE_SNAPSHOT_TEST_JSON}"
  DEPENDS "${ENGINE_SNAPSHOT_TEST_JSON}" "${ENGINE_SNAPSHOT_SCHEMA}"
  COMMENT "Compiling engine_snapshot_test_data.json to binary format"
)

# Custom target to ensure compilation happens
add_custom_target(compile_engine_snapshot_unit_test_data ALL
  DEPENDS "${ENGINE_SNAPSHOT_TEST_BIN}"
)

# Ensure FlatBuffers headers are generated first
add_dependencies(compile_engine_snapshot_unit_test_data flatbuffers_generate_headers)
