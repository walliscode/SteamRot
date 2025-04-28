add_library(data
  EntityConfigData.cpp
)

target_include_directories(data
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(data PUBLIC
nlohmann_json::nlohmann_json
components)

