# CMake script to compile fragment and joint JSON files to binary using flatc
#
# This script discovers all .json files in data/assets/fragments/ and data/assets/joints/
# directories and compiles them to .bin files using flatc.

# Function to compile a single fragment JSON file
function(compile_fragment_file json_file schema_file)
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
    COMMENT "Compiling fragment: ${file_name}.json -> ${file_name}.bin"
    VERBATIM
  )

  # Store the binary file path for later use
  set(FRAGMENT_BINARIES ${FRAGMENT_BINARIES} "${binary_file}" PARENT_SCOPE)
endfunction()

# Function to compile a single joint JSON file
function(compile_joint_file json_file schema_file)
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
    COMMENT "Compiling joint: ${file_name}.json -> ${file_name}.bin"
    VERBATIM
  )

  # Store the binary file path for later use
  set(JOINT_BINARIES ${JOINT_BINARIES} "${binary_file}" PARENT_SCOPE)
endfunction()


# Lists to accumulate all binary files
set(FRAGMENT_BINARIES "")
set(JOINT_BINARIES "")

# Path to the fragment.fbs schema
set(fragment_schema "${CMAKE_SOURCE_DIR}/src/types/flatbuffers/entities/fragment.fbs")

# Path to the joint.fbs schema
set(joint_schema "${CMAKE_SOURCE_DIR}/src/types/flatbuffers/entities/joint.fbs")

# Find all .json files in fragments directory
file(GLOB fragment_json_files
  "${CMAKE_SOURCE_DIR}/data/assets/fragments/*.json"
)

# Compile each fragment file found
foreach(json_file ${fragment_json_files})
  compile_fragment_file(${json_file} ${fragment_schema})
endforeach()

# Find all .json files in joints directory
file(GLOB joint_json_files
  "${CMAKE_SOURCE_DIR}/data/assets/joints/*.json"
)

# Compile each joint file found
foreach(json_file ${joint_json_files})
  compile_joint_file(${json_file} ${joint_schema})
endforeach()

# Create a custom target that depends on all compiled fragment and joint data
add_custom_target(compile_fragments_and_joints ALL
    DEPENDS ${FRAGMENT_BINARIES} ${JOINT_BINARIES}
    COMMENT "Compiling all fragment and joint files"
  )

# Make compile_fragments_and_joints depend on the schema being compiled first
add_dependencies(compile_fragments_and_joints flatbuffers_generate_headers)
