
# Add include directories
target_include_directories(SteamRot PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/systems/game
)

# Add the GameEngine source files
target_sources(SteamRot PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/systems/game/GameEngine.cpp
)
