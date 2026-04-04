# Set data directory types (root dirs)
set(DIRECTORY_ROOTS
    ${CMAKE_SOURCE_DIR}/data/defaults
)

# Clear the global binary list at configure time
set(FLATBUFFERS_ALL_GENERATED_BINARIES)

# Macro to define FlatBuffers generation for a given schema/json type
# Arguments:
#   1 - schema name without extension (e.g. themes, scenes, fragments)
#   2 - schema subdirectory (e.g. entities, scenes, configuration)
#   3 - json file extension pattern (e.g. .themes.json, .scenes.json, .fragment.json)
#   4 - data subdirectory under root (e.g. themes, scenes, fragments)
macro(flatbuffers_generate_for_type schema_name schema_subdir json_ext data_subdir)
  set(schema "${CMAKE_CURRENT_SOURCE_DIR}/${schema_subdir}/${schema_name}.fbs")
  foreach(root_dir ${DIRECTORY_ROOTS})
    set(data_dir "${root_dir}/${data_subdir}")
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
    endforeach()
  endforeach()
endmacro()

# Call macro for each supported type (now with schema subdirectory parameter)

# Arguments:
#   1 - schema name without extension (e.g. themes, scenes, fragments)
#   2 - schema subdirectory (e.g. entities, scenes, configuration)
#   3 - json file extension pattern (e.g. .themes.json, .scenes.json, .fragment.json)
#   4 - data subdirectory under root (e.g. themes, scenes, fragments)
#
flatbuffers_generate_for_type(ui_style "user_interface" ".styles.json" "ui_styles")
flatbuffers_generate_for_type(scene_data "scenes" ".scene_data.json" "scenes")
flatbuffers_generate_for_type(fragments "scenes" ".fragment.json" "fragments")
flatbuffers_generate_for_type(scene_manager_data "scenes" ".scene_manager_data.json" "scene_manager")
flatbuffers_generate_for_type(user_preferences "configuration" ".preferences.json" "preferences")
flatbuffers_generate_for_type(engine_data "engine" ".engine_data.json" "engine")
flatbuffers_generate_for_type(input_action_config "events" ".input_action_config.json" "input_actions")




# This target will actually trigger all the binary generation
add_custom_target(flatbuffers_generate_binaries ALL
    DEPENDS ${FLATBUFFERS_ALL_GENERATED_BINARIES}
)
