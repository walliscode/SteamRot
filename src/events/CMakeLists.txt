add_library(events
EventHandler.cpp
event_factory.cpp
payload_matchers.cpp
SFMLEventConverter.cpp
sfml_event_convert.cpp
)

target_include_directories(events
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(events
  PRIVATE
  SFML::Graphics
  stduuid
  types
  magic_enum

)


