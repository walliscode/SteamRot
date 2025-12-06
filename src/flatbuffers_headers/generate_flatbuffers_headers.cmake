# List your schema files organized by directory
# Note: Schema files are organized into subdirectories for clarity,
# but generated headers remain in root for zero include changes
set(schema_files
    # Core types
    ${CMAKE_CURRENT_SOURCE_DIR}/core/types.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/core/scene_types.fbs

    # Engine (already organized)
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/engine_resources_config.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/engine_config.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/engine_state.fbs

    # Scenes
    ${CMAKE_CURRENT_SOURCE_DIR}/scenes/scene_data.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/scenes/scene_resources.fbs
    # ${CMAKE_CURRENT_SOURCE_DIR}/scenes/scene_state.fbs
    # ${CMAKE_CURRENT_SOURCE_DIR}/scenes/scene_config.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/scenes/scene_manager_data.fbs


    # Entities and components
    ${CMAKE_CURRENT_SOURCE_DIR}/entities/entities.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/entities/user_interface.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/entities/grimoire_machina.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/entities/ui_state.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/entities/ui_style.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/entities/joints.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/entities/fragments.fbs

    # Events
    ${CMAKE_CURRENT_SOURCE_DIR}/events/events.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/events/event_packet_data.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/events/event_bus_data.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/events/subscriber_data.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/events/user_input.fbs

    # Logic
    ${CMAKE_CURRENT_SOURCE_DIR}/logic/logic_data.fbs

    # Assets
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/assets.fbs

    # Configuration
    ${CMAKE_CURRENT_SOURCE_DIR}/configuration/context_data.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/configuration/user_preferences.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/configuration/save_data.fbs

    # Testing
    ${CMAKE_CURRENT_SOURCE_DIR}/testing/test_data.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/testing/simulation.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/testing/event_test_data.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/testing/input_test_data.fbs
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
