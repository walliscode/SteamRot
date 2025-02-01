
add_library(systems
    GameEngine.cpp
    Scene.cpp
    SceneManager.cpp
    SceneMainMenu.cpp
    AssetManager.cpp
    Action.cpp
    DataLogger.cpp

)

message(STATUS "Data out dir: ${DATA_OUT_DIR}")
target_compile_definitions(systems
  PUBLIC
  DATA_OUT_DIR="${DATA_OUT_DIR}")


target_include_directories(systems
  PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include/utils)

target_link_libraries(systems PUBLIC
SFML::Graphics
nlohmann_json::nlohmann_json)
