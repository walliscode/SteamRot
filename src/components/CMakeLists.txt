add_library(components
CMeta.cpp
CText.cpp
CTransform.cpp
Component.cpp)

target_include_directories(components
  PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/resources/generated_headers
)

# check library exists
if(NOT TARGET nlohmann_json::nlohmann_json)
  message(FATAL_ERROR "json library is missing")
else()
  message(STATUS "json library found")
endif()


target_link_libraries(components
  PUBLIC
  nlohmann_json::nlohmann_json
  SFML::Graphics
  )

