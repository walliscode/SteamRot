# set data directory types
set(DIRECTORY_TYPES
"PRODUCTION"
"TEST")

# -- FlatBuffers generation for themes.fbs --


set(themes_schema "${CMAKE_CURRENT_SOURCE_DIR}/themes.fbs")
set(THEMES_DIR "${data_dir}/themes")

# All JSON files for themes.fbs
file(GLOB themes_jsons "${THEMES_DIR}/*.themes.json")

set(themes_generated_binaries)

foreach(json_file ${themes_jsons})
  get_filename_component(json_we ${json_file} NAME_WE) # strips .json
  set(bin_file "${THEMES_DIR}/${json_we}.themes..bin")
  list(APPEND themes_generated_binaries "${bin_file}")

  add_custom_command(
    OUTPUT "${bin_file}"
    COMMAND flatc
      --binary
      -o "${THEMES_DIR}"
      "${themes_schema}"
      "${json_file}"
    DEPENDS "${themes_schema}" "${json_file}"
    COMMAND ${CMAKE_COMMAND}
      -E echo "Generating binary FlatBuffer ${bin_file}
       from ${json_file} using ${themes_schema}"
    VERBATIM
  )
endforeach()

# generate all scene binaries
set(scenes_schema "${CMAKE_CURRENT_SOURCE_DIR}/scenes.fbs")
set(SCENES_DIR "${data_dir}/scenes")

# All JSON files for scenes.fbs
file(GLOB scenes_jsons "${SCENES_DIR}/*.scenes.json")

# message out detected scene_jsons

set(scenes_generated_binaries)

foreach(json_file ${scenes_jsons})
  get_filename_component(json_we ${json_file} NAME_WE) # strips .json
  set(bin_file "${SCENES_DIR}/${json_we}.scenes.bin")
  list(APPEND scenes_generated_binaries "${bin_file}")
  add_custom_command(
    OUTPUT "${bin_file}"
    COMMAND flatc
      --binary
      -o "${SCENES_DIR}"
      "${scenes_schema}"
      "${json_file}"
    DEPENDS "${scenes_schema}" "${json_file}"
    COMMAND ${CMAKE_COMMAND}
      -E echo "Generating binary FlatBuffer ${bin_file}
       from ${json_file} using ${scenes_schema}"
    VERBATIM
  )
endforeach()

# -- FlatBuffers generation for fragment.fbs --



add_custom_target(flatbuffers_generate_themes_binaries ALL
  DEPENDS
  ${themes_generated_binaries}
  ${scenes_generated_binaries}
  ${fragments_generated_binaries}
)
