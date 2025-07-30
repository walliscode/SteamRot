add_library(components
CMeta.cpp
CUserInterface.cpp
CMachinaForm.cpp
CGrimoireMachina.cpp

)

target_include_directories(components
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)




target_link_libraries(components
  PUBLIC
  nlohmann_json::nlohmann_json
  SFML::Graphics
  magic_enum::magic_enum
  flatbuffers
  flatbuffers_headers
  logger
  systems
  user_interface
)

