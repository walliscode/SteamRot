include(CTest)
include(Catch)

# Define STEAMROT_TEST_MODE for all test targets
# This enables test-specific features in the game engine:
# - Runtime logic injection
# - Data injection
# - Headless mode
add_compile_definitions(STEAMROT_TEST_MODE)

# Test utilities and shared context
add_subdirectory(context)

# Test matchers - custom Catch2 matchers
add_subdirectory(matchers)

# Test harness - utilities for data-driven testing
add_subdirectory(harness)
add_subdirectory(reporters)

# Test structure organized by test type
add_subdirectory(unit)
add_subdirectory(integration)
add_subdirectory(system)
add_subdirectory(perf)

