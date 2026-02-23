add_library(events
EventHandler.cpp
event_factory.cpp
payload_matchers.cpp
SFMLInputRegistry.cpp
)

target_include_directories(events
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(events
  PRIVATE
  SFML::Graphics
  stduuid
  logger
  types
  magic_enum

)


