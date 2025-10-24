# Unit tests directory
# Unit tests test individual classes/functions in isolation with mocked dependencies
# Run with: ctest -L unit

# Test utilities (matchers, helpers)
add_subdirectory(matchers)

# Unit tests organized by subsystem
add_subdirectory(assets)
add_subdirectory(components)
add_subdirectory(data_handlers)
add_subdirectory(display)
add_subdirectory(entity)
add_subdirectory(events)
add_subdirectory(logic)
add_subdirectory(scenes)
add_subdirectory(systems)
add_subdirectory(user_interface)
