# CMake script to compile test data JSON files to binary using flatc
#
# This script discovers all .test_data.json files in data/ subdirectories
# within each test executable directory (e.g., tests/components/data/,
# tests/entity/data/, etc.) and compiles them to .test_data.bin files using flatc.

# Function to compile a single test data JSON file
function(compile_test_data_file json_file schema_file)
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
    COMMENT "Compiling test data: ${file_name}.test_data.json -> ${file_name}.bin"
    VERBATIM
  )

  # Store the binary file path for later use
  set(TEST_DATA_BINARIES ${TEST_DATA_BINARIES} "${binary_file}" PARENT_SCOPE)
endfunction()


# List to accumulate all binary files
set(TEST_DATA_BINARIES "")



# Path to the test_data.fbs schema (in testing subdirectory after Phase 3 reorganization)
set(test_data_schema "${CMAKE_SOURCE_DIR}/src/flatbuffers_headers/testing/test_data.fbs")

message("looking for files in ${CMAKE_SOURCE_DIR}")
# Find all .test_data.json files in all data directories

file(GLOB_RECURSE test_data_json_files
  "${CMAKE_SOURCE_DIR}/tests/*.test_data.json"

)
message(STATUS "Found JSON files: ${test_data_json_files}")

# Compile each test data file found
foreach(json_file ${test_data_json_files})

  compile_test_data_file(${json_file} ${test_data_schema})
endforeach()


# Create a custom target that depends on all compiled test data
add_custom_target(compile_test_data ALL
    DEPENDS ${TEST_DATA_BINARIES}
    COMMENT "Compiling all test data files"
  )

# Make compile_test_data depend on the schema being compiled first
add_dependencies(compile_test_data flatbuffers_generate_headers)

