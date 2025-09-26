add_executable(test_events
Subscriber.test.cpp
SubscriberFactory.test.cpp
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
)

catch_discover_tests(test_events)

add_library(mock_subscriber_data
INTERFACE
)

target_include_directories(mock_subscriber_data
INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
