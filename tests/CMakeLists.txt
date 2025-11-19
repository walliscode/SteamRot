include(CTest)
include(Catch)

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

