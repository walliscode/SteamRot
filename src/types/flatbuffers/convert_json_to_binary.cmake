# Set data directory types (root dirs)
set(DIRECTORY_ROOTS
    ${CMAKE_SOURCE_DIR}/data/defaults
    ${CMAKE_SOURCE_DIR}/tests/data/defaults
)

# User data directories (for saves and user-modifiable content)
set(USER_DIRECTORY_ROOTS
    ${CMAKE_SOURCE_DIR}/data/user
    ${CMAKE_SOURCE_DIR}/tests/data/user
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

      # Main prod directory path
      set(prod_dir "${CMAKE_SOURCE_DIR}/data/defaults")
      set(test_dir "${CMAKE_SOURCE_DIR}/tests/data/defaults")
      set(prod_subdir "${prod_dir}/${data_subdir}")
      set(test_subdir "${test_dir}/${data_subdir}")

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

# Call macro for each supported type (now with schema subdirectory parameter)
flatbuffers_generate_for_type(ui_style "entities" ".styles.json" "ui_styles")
flatbuffers_generate_for_type(scene_data "scenes" ".scene_data.json" "scenes")
flatbuffers_generate_for_type(fragments "scenes" ".fragment.json" "fragments")
flatbuffers_generate_for_type(assets "assets" ".json" "asset_manager")
flatbuffers_generate_for_type(scene_manager_data "scenes" ".scene_manager_data.json" "scene_manager")
flatbuffers_generate_for_type(context_data "configuration" ".json" "context")
flatbuffers_generate_for_type(user_preferences "configuration" ".preferences.json" "preferences")

# Macro for engine-specific types (to handle engine subdirectory)
# Arguments:
#   1 - schema name without extension
#   2 - json file extension pattern
#   3 - data subdirectory under root
macro(flatbuffers_generate_for_engine_type schema_name json_ext data_subdir)
  set(schema "${CMAKE_CURRENT_SOURCE_DIR}/engine/${schema_name}.fbs")
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

      # Main prod directory path
      set(prod_dir "${CMAKE_SOURCE_DIR}/data/defaults")
      set(test_dir "${CMAKE_SOURCE_DIR}/tests/data/defaults")
      set(prod_subdir "${prod_dir}/${data_subdir}")
      set(test_subdir "${test_dir}/${data_subdir}")

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

# Engine-specific schemas in engine subdirectory
flatbuffers_generate_for_engine_type(engine_resources_config ".engine_resources_config.json" "engine")
flatbuffers_generate_for_engine_type(engine_config ".engine_config.json" "engine")
flatbuffers_generate_for_engine_type(engine_state ".engine_state.json" "engine")

# Macro for user-specific data (saves, etc.) - doesn't copy between prod/test
# Arguments:
#   1 - schema name without extension
#   2 - schema subdirectory (e.g. configuration)
#   3 - json file extension pattern (e.g. .save.json)
#   4 - data subdirectory under user root (e.g. saves)
macro(flatbuffers_generate_for_user_type schema_name schema_subdir json_ext data_subdir)
  set(schema "${CMAKE_CURRENT_SOURCE_DIR}/${schema_subdir}/${schema_name}.fbs")
  foreach(root_dir ${USER_DIRECTORY_ROOTS})
    set(data_dir "${root_dir}/${data_subdir}")
    file(GLOB json_candidates "${data_dir}/*${json_ext}")

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
      string(REGEX REPLACE "\\.json$" "" bin_base "${json_name}")
      # Use .save extension for save files instead of .bin
      set(bin_file "${data_dir}/${bin_base}.save")

      add_custom_command(
                OUTPUT "${bin_file}"
                COMMAND flatc
                    --binary
                    -o "${data_dir}"
                    "${schema}"
                    "${json_file}"
                COMMAND ${CMAKE_COMMAND}
                    -E echo "Generating save file ${bin_file} from ${json_file} using ${schema}"
                DEPENDS "${schema}" "${json_file}"
                VERBATIM
            )
      list(APPEND FLATBUFFERS_ALL_GENERATED_BINARIES "${bin_file}")
    endforeach()
  endforeach()
endmacro()

# User-specific data types
flatbuffers_generate_for_user_type(save_data "configuration" ".json" "saves")

# This target will actually trigger all the binary generation
add_custom_target(flatbuffers_generate_binaries ALL
    DEPENDS ${FLATBUFFERS_ALL_GENERATED_BINARIES}
)
