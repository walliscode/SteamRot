add_library(interfaces
INTERFACE
)

target_include_directories(interfaces
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}


  # ${CMAKE_SOURCE_DIR}/src/types/user_interface/
  # ${CMAKE_SOURCE_DIR}/src/types/user_interface/styles/
  # ${CMAKE_SOURCE_DIR}/src/types/core/
  # ${CMAKE_SOURCE_DIR}/src/types/events/
  # ${CMAKE_SOURCE_DIR}/src/types/assets/
  # ${CMAKE_SOURCE_DIR}/src/types/flatbuffers/generated/

)

target_link_libraries(interfaces
  INTERFACE
  types
  SFML::Graphics
  stduuid

)

