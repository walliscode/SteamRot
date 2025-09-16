# List your schema files
set(schema_files
    ${CMAKE_CURRENT_SOURCE_DIR}/ui_style.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/scenes.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/user_input.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/entities.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/types.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/user_interface.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/grimoire_machina.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/assets.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/fragments.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/joints.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/events.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/scene_types.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/subscriber_config.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/event_packet_data.fbs



)

set(generated_headers)
foreach(schema_file ${schema_files})
  get_filename_component(schema_name ${schema_file} NAME_WE)
  set(schema_header "${CMAKE_CURRENT_SOURCE_DIR}/${schema_name}_generated.h")
  list(APPEND generated_headers "${schema_header}")

  add_custom_command(
        OUTPUT "${schema_header}"
        COMMAND flatc
        ARGS --cpp
        ARGS -o "${CMAKE_CURRENT_SOURCE_DIR}"  "${schema_file}"
        DEPENDS "${schema_file}"
        COMMAND ${CMAKE_COMMAND}
        ARGS -E echo "Generating FlatBuffers header for ${schema_name}"
        VERBATIM
    )
endforeach()

# Custom target to generate all headers
add_custom_target(flatbuffers_generate_headers ALL
    DEPENDS ${generated_headers}
)

# Interface library for consumers
add_library(flatbuffers_headers INTERFACE)
add_dependencies(flatbuffers_headers flatbuffers_generate_headers)

# Export the include directory and generated headers
target_include_directories(flatbuffers_headers INTERFACE
${CMAKE_CURRENT_SOURCE_DIR}
)
