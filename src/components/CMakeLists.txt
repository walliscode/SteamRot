add_library(components

CMeta.cpp
)

target_include_directories(components
  PUBLIC
  ${CMAKE_SOURCE_DIR}/include

)




target_link_libraries(components
  PUBLIC
  nlohmann_json::nlohmann_json
  SFML::Graphics
  )

