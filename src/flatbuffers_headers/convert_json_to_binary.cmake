# Set data directory types (root dirs)
set(DIRECTORY_ROOTS
    ${CMAKE_SOURCE_DIR}/data
    ${CMAKE_SOURCE_DIR}/tests/data
)

# Clear the global binary list at configure time
set(FLATBUFFERS_ALL_GENERATED_BINARIES)

# Macro to define FlatBuffers generation for a given schema/json type
# Arguments:
#   1 - schema name without extension (e.g. themes, scenes, fragments)
#   2 - json file extension pattern (e.g. .themes.json, .scenes.json, .fragment.json)
#   3 - subdirectory under root (e.g. themes, scenes, fragments)
macro(flatbuffers_generate_for_type schema_name json_ext subdir)
  set(schema "${CMAKE_CURRENT_SOURCE_DIR}/${schema_name}.fbs")
  foreach(root_dir ${DIRECTORY_ROOTS})
    set(data_dir "${root_dir}/${subdir}")
    file(GLOB jsons "${data_dir}/*${json_ext}")

    foreach(json_file ${jsons})
      get_filename_component(json_we ${json_file} NAME_WE)
      set(bin_file "${data_dir}/${json_we}${json_ext}.bin")

      # Accumulate all binaries in a global variable
      list(APPEND FLATBUFFERS_ALL_GENERATED_BINARIES "${bin_file}")

      add_custom_command(
                OUTPUT "${bin_file}"
                COMMAND flatc
                    --binary
                    -o "${data_dir}"
                    "${schema}"
                    "${json_file}"
                DEPENDS "${schema}" "${json_file}"
                COMMAND ${CMAKE_COMMAND}
                    -E echo "Generating binary FlatBuffer ${bin_file} from ${json_file} using ${schema}"
                VERBATIM
            )
    endforeach()
  endforeach()
endmacro()

# Call macro for each supported type
flatbuffers_generate_for_type(themes ".themes.json" "themes")
flatbuffers_generate_for_type(scenes ".scenes.json" "scenes")
flatbuffers_generate_for_type(fragments ".fragment.json" "fragments")

# This target will actually trigger all the binary generation
add_custom_target(flatbuffers_generate_binaries ALL
    DEPENDS ${FLATBUFFERS_ALL_GENERATED_BINARIES}
)
