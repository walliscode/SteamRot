add_executable(test_events
Subscriber.test.cpp
subscriber_factory.test.cpp
UserInputBitset.test.cpp
EventHandler.test.cpp
)

target_include_directories(test_events
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_events
  PRIVATE
  Catch2::Catch2WithMain
  events
  types
)

catch_discover_tests(test_events)

