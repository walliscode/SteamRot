# Unit tests directory
# Unit tests test individual classes/functions in isolation with mocked dependencies
# Run with: ctest -L unit

# Unit tests organized by subsystem
add_subdirectory(assets)
add_subdirectory(user_interface)
add_subdirectory(components)
add_subdirectory(data_handlers)
add_subdirectory(data_providers)
add_subdirectory(display)
add_subdirectory(entity)
add_subdirectory(events)
add_subdirectory(logic)
add_subdirectory(scenes)
add_subdirectory(engine)

