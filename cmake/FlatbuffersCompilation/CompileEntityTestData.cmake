# CMake script to compile entity collection JSON files to binary using flatc
#
# This script discovers all entity_collection_*.json files in tests/unit/entity/data/
# and compiles them to .bin files using flatc.

# Function to compile a single entity collection JSON file
function(compile_entity_collection_file json_file schema_file)
  # Get the filename without extension
  get_filename_component(file_name ${json_file} NAME_WE)
  get_filename_component(file_dir ${json_file} DIRECTORY)

  # Output binary file in the same directory
  set(binary_file "${file_dir}/${file_name}.bin")

  # Add custom command to compile JSON to binary
  add_custom_command(
    OUTPUT "${binary_file}"
    COMMAND flatc
    ARGS --binary
    ARGS -o "${file_dir}"
    ARGS "${schema_file}"
    ARGS "${json_file}"
    DEPENDS "${json_file}" "${schema_file}"
    COMMENT "Compiling entity collection: ${file_name}.json -> ${file_name}.bin"
    VERBATIM
  )

  # Store the binary file path for later use
  set(ENTITY_COLLECTION_BINARIES ${ENTITY_COLLECTION_BINARIES} "${binary_file}" PARENT_SCOPE)
endfunction()

# List to accumulate all binary files
set(ENTITY_COLLECTION_BINARIES "")

# Path to the entities.fbs schema
set(entities_schema "${CMAKE_SOURCE_DIR}/src/types/flatbuffers/entities/entities.fbs")

# Find all entity_collection_*.json files in tests/unit/entity/data/
file(GLOB entity_collection_json_files
  "${CMAKE_SOURCE_DIR}/tests/unit/entity/data/entity_collection_*.json"
)

message(STATUS "Found entity collection JSON files: ${entity_collection_json_files}")

# Compile each entity collection file found
foreach(json_file ${entity_collection_json_files})
  compile_entity_collection_file(${json_file} ${entities_schema})
endforeach()

# Create a custom target that depends on all compiled entity collections
add_custom_target(compile_entity_test_data ALL
    DEPENDS ${ENTITY_COLLECTION_BINARIES}
    COMMENT "Compiling all entity collection test data files"
)

# Make compile_entity_test_data depend on the schema being compiled first
add_dependencies(compile_entity_test_data flatbuffers_generate_headers)
