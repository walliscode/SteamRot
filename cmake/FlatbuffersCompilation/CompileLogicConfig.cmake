# CMake script to compile logic_config JSON file to binary using flatc
#
# This script compiles the logic_config.json file to logic_config.bin using flatc.

# Function to compile the logic_config JSON file
function(compile_logic_config_file json_file schema_file)
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
    COMMENT "Compiling logic config: ${file_name}.json -> ${file_name}.bin"
    VERBATIM
  )

  # Store the binary file path for later use
  set(LOGIC_CONFIG_BINARY "${binary_file}" PARENT_SCOPE)
endfunction()


# Path to the logic_config.fbs schema
set(logic_config_schema "${CMAKE_SOURCE_DIR}/src/types/flatbuffers/logic/logic_config.fbs")

# Path to the logic_config.json file
set(logic_config_json "${CMAKE_SOURCE_DIR}/data/defaults/logic_config/logic_config.json")

# Compile the logic_config file
compile_logic_config_file(${logic_config_json} ${logic_config_schema})

# Create a custom target that depends on the compiled logic config
add_custom_target(compile_logic_config ALL
    DEPENDS ${LOGIC_CONFIG_BINARY}
    COMMENT "Compiling logic config file"
  )

# Make compile_logic_config depend on the schema being compiled first
add_dependencies(compile_logic_config flatbuffers_generate_headers)
