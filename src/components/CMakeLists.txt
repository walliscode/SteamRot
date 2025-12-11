add_library(components
Component.cpp
CMeta.cpp
CUserInterface.cpp
CMachinaForm.cpp
CGrimoireMachina.cpp
CUIState.cpp
)

target_include_directories(components
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(components
  PUBLIC
  SFML::Graphics
  flatbuffers
  flatbuffers_headers
  logger
  types


)

