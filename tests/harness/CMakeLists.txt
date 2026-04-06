add_library(harness
  TestEngine.cpp
  harness_runner.cpp
  SimulationRunner.cpp
  input_simulation.cpp
  add_uuids.cpp
  pixel_capture.cpp
)

target_include_directories(harness
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(harness
  PUBLIC
  engine
  types
  matchers
  SFML::Graphics
)

