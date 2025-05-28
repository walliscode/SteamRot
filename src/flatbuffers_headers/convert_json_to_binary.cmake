set(schema_files
    "${CMAKE_CURRENT_SOURCE_DIR}/themes.fbs"
    # Add more schemas here
)

set(all_generated_binaries)

foreach(schema_file ${schema_files})
  get_filename_component(schema_name ${schema_file} NAME_WE)
  file(GLOB schema_jsons "${CMAKE_CURRENT_SOURCE_DIR}/*.${schema_name}.json")

  foreach(json_file ${schema_jsons})
    get_filename_component(json_we ${json_file} NAME_WE) # strips .json
    set(bin_file
      "${CMAKE_CURRENT_SOURCE_DIR}/${json_we}.bin"
    ) # keeps .themes, replaces .json
    list(APPEND all_generated_binaries "${bin_file}")

    add_custom_command(
            OUTPUT "${bin_file}"
            COMMAND flatc
            ARGS --binary
            ARGS -o
              "${CMAKE_CURRENT_SOURCE_DIR}"
              "${CMAKE_CURRENT_SOURCE_DIR}/${schema_file}"
              "${json_file}"
            DEPENDS "${schema_file}" "${json_file}"
            COMMAND ${CMAKE_COMMAND}
            ARGS -E echo "Generating binary FlatBuffer ${bin_file}
            from ${json_file} using ${schema_file}"
            VERBATIM
        )
  endforeach()
endforeach()

add_custom_target(flatbuffers_generate_binaries ALL
    DEPENDS ${all_generated_binaries}
)
