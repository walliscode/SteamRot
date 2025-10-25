include(CTest)
include(Catch)

# Test utilities and shared context
add_subdirectory(context)

# Test harness - utilities for data-driven testing
add_subdirectory(harness)

# Test structure organized by test type
add_subdirectory(unit)
add_subdirectory(integration)
add_subdirectory(system)
add_subdirectory(perf)

