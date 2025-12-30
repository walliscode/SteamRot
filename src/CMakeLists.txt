# Create an interface library for environment configuration
# This ensures all targets use consistent path configuration
add_library(environment_config INTERFACE)

# Set environment to use data/ directory for production builds
# Tests will not link to this target and will default to tests/data
target_compile_definitions(environment_config INTERFACE STEAMROT_ENV_DEBUG)

add_subdirectory(assets)
add_subdirectory(components)
add_subdirectory(configuration)
add_subdirectory(display)
add_subdirectory(entity)
add_subdirectory(events)
add_subdirectory(error_loop)
add_subdirectory(scenes)
add_subdirectory(resources)
add_subdirectory(engine)
add_subdirectory(logger)
add_subdirectory(interfaces)
add_subdirectory(data_providers)
add_subdirectory(logic)
add_subdirectory(user_interface)
add_subdirectory(types)
add_subdirectory(context)


