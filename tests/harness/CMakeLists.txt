add_library(harness
  # test_harness.cpp
  # test_data_loader.cpp
  # test_data_comparison.cpp
  TestEngine.cpp
  # simulation_runner.cpp
  # input_simulation.cpp
  # event_simulation.cpp

)

target_include_directories(harness
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(harness
  PRIVATE
  engine
)

