add_library(display
    DisplayManager.cpp
    Tile.cpp
    Session.cpp

)

target_include_directories(display
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/include/utils
  ${CMAKE_SOURCE_DIR}/include/constants
  ${CMAKE_SOURCE_DIR}/include/enums
)
target_link_libraries(display PUBLIC
    SFML::Graphics
    nlohmann_json::nlohmann_json
    magic_enum
    systems
    stduuid
    UIEngine
    utilities

)
