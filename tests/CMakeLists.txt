
add_executable(steamrot_tests
"unit_tests/GameEngine.test.cpp")
target_link_libraries(steamrot_tests
  PRIVATE
  Catch2::Catch2WithMain
  systems)

message (STATUS "Tests enabled")

add_test(NAME steamrot_tests COMMAND steamrot_tests)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(steamrot_tests)
