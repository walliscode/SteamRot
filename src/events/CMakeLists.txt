add_library(events
EventHandler.cpp
event_factory.cpp
event_bus_conversion.cpp
event_handler_tick.cpp
subscriber_factory.cpp
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
logger
types

)

# # Apply project-specific compile and link options
# if(DEFINED PROJECT_COMPILE_OPTIONS)
#   target_compile_options(events PRIVATE ${PROJECT_COMPILE_OPTIONS})
# endif()
# if(DEFINED PROJECT_LINK_OPTIONS)
#   target_link_options(events PRIVATE ${PROJECT_LINK_OPTIONS})
# endif()
