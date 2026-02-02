add_executable(simulation_tests
  all_simulations.test.cpp
)

target_link_libraries(simulation_tests
  PUBLIC
  Catch2::Catch2WithMain
  harness
)

catch_discover_tests(simulation_tests)

