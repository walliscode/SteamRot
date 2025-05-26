add_library(UIEngine
UIEngine.cpp)


target_include_directories(UIEngine
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(UIEngine
    PUBLIC
        SFML::Graphics
        nlohmann_json::nlohmann_json
components)
