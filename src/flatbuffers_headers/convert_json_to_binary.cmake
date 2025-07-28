# set data directory types
set(DIRECTORY_ROOTS
${CMAKE_SOURCE_DIR}/data
${CMAKE_SOURCE_DIR}/tests/data
)


# -- FlatBuffers generation for themes.fbs --
set(themes_schema "${CMAKE_CURRENT_SOURCE_DIR}/themes.fbs")
set(THEMES_DIR "${data_dir}/themes")

# All JSON files for themes.fbs
file(GLOB themes_jsons "${THEMES_DIR}/*.themes.json")

set(themes_generated_binaries)

foreach(json_file ${themes_jsons})
  get_filename_component(json_we ${json_file} NAME_WE) # strips .json
  set(bin_file "${THEMES_DIR}/${json_we}.themes.bin")
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
set(fragments_schema "${CMAKE_CURRENT_SOURCE_DIR}/fragments.fbs")
## cycle through all directory roots
foreach(root_dir ${DIRECTORY_ROOTS})
  message(STATUS "Processing FlatBuffers fragments in: ${fragments_dir}")
  set(fragments_dir "${root_dir}/fragments")
  # All JSON files for fragments.fbs
  file(GLOB fragments_jsons "${fragments_dir}/*.fragment.json")
  if(NOT EXISTS "${fragments_jsons}")
    message(STATUS "No fragment JSON files found in ${fragments_dir}. Skipping.")
  else()
    message(STATUS "Found fragment files in ${fragments_dir}: ${fragments_jsons}")
  endif()
  set(fragments_generated_binaries)
  foreach(json_file ${fragments_jsons})
    get_filename_component(json_we ${json_file} NAME_WE) # strips .json
    set(bin_file "${root_dir}/${json_we}.fragment.bin")
    list(APPEND fragments_generated_binaries "${bin_file}")
    add_custom_command(
      OUTPUT "${bin_file}"
      COMMAND flatc
        --binary
        -o "${fragments_dir}"
        "${fragments_schema}"
        "${json_file}"
      DEPENDS "${fragments_schema}" "${json_file}"
      COMMAND ${CMAKE_COMMAND}
        -E echo "Generating binary FlatBuffer ${bin_file}
         from ${json_file} using ${fragments_schema}"
      VERBATIM
    )
  endforeach()
endforeach()

add_custom_target(flatbuffers_generate_themes_binaries ALL
  DEPENDS
  ${themes_generated_binaries}
  ${scenes_generated_binaries}
  ${fragments_generated_binaries}
)
