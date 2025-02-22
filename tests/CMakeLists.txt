
add_executable(steamrot_tests
"unit_tests/GameEngine.test.cpp")
target_link_libraries(steamrot_tests
  PRIVATE
  Catch2::Catch2WithMain
  systems)

message (STATUS "Tests enabled")


list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(steamrot_tests)
