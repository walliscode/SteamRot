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
    file(GLOB json_candidates "${data_dir}/*${json_ext}")  # Only files with your custom extension

    # Filter files to exactly match pattern
    set(jsons "")
    foreach(json_file ${json_candidates})
      string(LENGTH "${json_file}" json_file_len)
      string(LENGTH "${json_ext}" json_ext_len)
      math(EXPR start_index "${json_file_len}-${json_ext_len}")
      string(SUBSTRING "${json_file}" ${start_index} ${json_ext_len} file_suffix)
      if("${file_suffix}" STREQUAL "${json_ext}")
        list(APPEND jsons "${json_file}")
      endif()
    endforeach()

    foreach(json_file ${jsons})
      get_filename_component(json_name ${json_file} NAME)
      # Remove the trailing .json from the filename (so title.scenes.json -> title.scenes)
      string(REGEX REPLACE "\\.json$" "" bin_base "${json_name}")
      set(bin_file "${data_dir}/${bin_base}.bin")

      # Main prod directory path
      set(prod_dir "${CMAKE_SOURCE_DIR}/data")
      set(test_dir "${CMAKE_SOURCE_DIR}/tests/data")
      set(prod_subdir "${prod_dir}/${subdir}")
      set(test_subdir "${test_dir}/${subdir}")

      # If this is the prod dir, also copy to test dir after generation
      if("${root_dir}" STREQUAL "${prod_dir}")
        set(test_bin_file "${test_subdir}/${bin_base}.bin")
        add_custom_command(
                    OUTPUT "${bin_file}" "${test_bin_file}"
                    COMMAND flatc
                        --binary
                        -o "${data_dir}"
                        "${schema}"
                        "${json_file}"
                    COMMAND ${CMAKE_COMMAND}
                        -E echo "Generating binary FlatBuffer ${bin_file} from ${json_file} using ${schema}"
                    COMMAND ${CMAKE_COMMAND}
                        -E make_directory "${test_subdir}"
                    COMMAND ${CMAKE_COMMAND}
                        -E copy_if_different "${bin_file}" "${test_bin_file}"
                    DEPENDS "${schema}" "${json_file}"
                    VERBATIM
                )
        list(APPEND FLATBUFFERS_ALL_GENERATED_BINARIES "${bin_file}" "${test_bin_file}")
      else()
        add_custom_command(
                  OUTPUT "${bin_file}"
                  COMMAND flatc
                      --binary
                      -o "${data_dir}"
                      "${schema}"
                      "${json_file}"
                  COMMAND ${CMAKE_COMMAND}
                      -E echo "Generating binary FlatBuffer ${bin_file} from ${json_file} using ${schema}"
                  DEPENDS "${schema}" "${json_file}"
                  VERBATIM
              )
        list(APPEND FLATBUFFERS_ALL_GENERATED_BINARIES "${bin_file}")
      endif()
    endforeach()
  endforeach()
endmacro()

# Call macro for each supported type
flatbuffers_generate_for_type(ui_style ".styles.json" "ui_styles")
flatbuffers_generate_for_type(scenes ".scenes.json" "scenes")
flatbuffers_generate_for_type(fragments ".fragment.json" "fragments")
flatbuffers_generate_for_type(assets ".json" "asset_manager")
flatbuffers_generate_for_type(scene_manager ".json" "scene_manager")
flatbuffers_generate_for_type(game_engine ".json" "game_engine")
# This target will actually trigger all the binary generation
add_custom_target(flatbuffers_generate_binaries ALL
    DEPENDS ${FLATBUFFERS_ALL_GENERATED_BINARIES}
)
