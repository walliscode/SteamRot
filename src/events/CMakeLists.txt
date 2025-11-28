add_library(events
EventHandler.cpp
event_factory.cpp
event_bus_conversion.cpp
event_handler_tick.cpp
Subscriber.cpp
SubscriberFactory.cpp
)

target_include_directories(events
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(events PUBLIC
SFML::Graphics
flatbuffers
flatbuffers_headers
stduuid
magic_enum::magic_enum
logger
matchers
)
