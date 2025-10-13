include(CTest)
include(Catch)

# Test utilities and shared context
add_subdirectory(context)

# Existing test structure (by subsystem)
add_subdirectory(assets)
add_subdirectory(data_handlers)
add_subdirectory(display)
add_subdirectory(components)
add_subdirectory(entity)
add_subdirectory(events)
add_subdirectory(scenes)
add_subdirectory(systems)
add_subdirectory(logic)
add_subdirectory(user_interface)

# New test structure (by test type)
add_subdirectory(unit)
add_subdirectory(integration)
add_subdirectory(system)
add_subdirectory(perf)

