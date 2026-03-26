# List your schema files organized by directory
# Note: Schema files are organized into subdirectories for clarity,
# but generated headers remain in root for zero include changes
set(schema_files
    # Core types
    ${CMAKE_CURRENT_SOURCE_DIR}/core/types.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/core/scene_types.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/core/engine_snapshot.fbs


    # Engine (already organized)
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/engine_resources_config.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/engine_config.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/engine_state.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/engine_data.fbs

    # Scenes
    ${CMAKE_CURRENT_SOURCE_DIR}/scenes/scene_data.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/scenes/scene_info.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/scenes/scene_collection_data.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/scenes/scene_resources_config.fbs
    # ${CMAKE_CURRENT_SOURCE_DIR}/scenes/scene_state.fbs
    # ${CMAKE_CURRENT_SOURCE_DIR}/scenes/scene_config.fbs

    ${CMAKE_CURRENT_SOURCE_DIR}/scenes/scene_manager_state.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/scenes/scene_manager_data.fbs



    # Entities and components
    ${CMAKE_CURRENT_SOURCE_DIR}/entities/entities.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/entities/user_interface.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/entities/grimoire_machina.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/entities/machina_form.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/entities/view_direction.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/entities/ui_state.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/entities/ui_style.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/entities/joint.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/entities/fragment.fbs

    # Events
    ${CMAKE_CURRENT_SOURCE_DIR}/events/event_packet.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/events/event_bus_data.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/events/subscriber.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/events/user_input.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/events/event_context.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/events/event_payload.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/events/input_action_config.fbs

    # Logic
    ${CMAKE_CURRENT_SOURCE_DIR}/logic/logic_config.fbs

    # Assets
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/asset_config.fbs

    # Configuration

    ${CMAKE_CURRENT_SOURCE_DIR}/configuration/user_preferences.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/configuration/save_data.fbs

    # Testing
    ${CMAKE_CURRENT_SOURCE_DIR}/testing/test_data.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/testing/simulation_data.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/testing/input_data.fbs
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
        ARGS -o "${CMAKE_CURRENT_SOURCE_DIR}/generated/"  "${schema_file}"
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
