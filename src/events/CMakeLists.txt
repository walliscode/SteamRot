add_library(events
EventHandler.cpp
event_helpers.cpp
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
)
