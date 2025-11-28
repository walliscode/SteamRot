# GenerateTestDashboard.cmake
# Provides a target to generate the test data dashboard
#
# This module creates a CMake target that generates an interactive HTML dashboard
# for browsing and visualizing test_data.json files.
#
# Usage:
#   cmake --build --preset Debug --target generate_test_dashboard
#
# The generated dashboard will be placed at:
#   documentation/generated/test_dashboard.html

find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Define the dashboard generation target
add_custom_target(generate_test_dashboard
    COMMAND ${Python3_EXECUTABLE} -m tools.test_dashboard
            --tests-dir ${CMAKE_SOURCE_DIR}/tests
            --output ${CMAKE_SOURCE_DIR}/documentation/generated/test_dashboard.html
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating test data dashboard..."
    VERBATIM
)

# Add a message to show where the dashboard will be generated
message(STATUS "Test dashboard target available: generate_test_dashboard")
message(STATUS "  Output: documentation/generated/test_dashboard.html")
