/////////////////////////////////////////////////
/// @file
/// @brief Free functions for game loop execution.
///
/// These functions extract the core game loop patterns from GameEngine,
/// enabling reuse by both the game engine and test harness.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DisplayManager.h"
#include "GameResources.h"
#include "SceneManager.h"

namespace steamrot::game_loop {

/////////////////////////////////////////////////
/// @brief Update game resources each tick
///
/// Performs per-tick updates to game resources, such as:
/// - Updating mouse position from window
///
/// @param game_resources Reference to GameResources to update
/////////////////////////////////////////////////
void UpdateGameResources(GameResources &game_resources);

/////////////////////////////////////////////////
/// @brief Execute a single headless game loop iteration
///
/// Performs a complete iteration without display rendering:
/// 1. Update game resources (mouse position, etc.)
/// 2. Process event tick start (preload, waiting room, subscribers)
/// 3. Update scene manager (process subscriptions, update scenes)
/// 4. Process event tick end (tick global bus)
/// 5. Increment loop number
///
/// Note: Does NOT call display manager render cycle.
///
/// @param game_resources Reference to GameResources
/// @param scene_manager Reference to SceneManager
/////////////////////////////////////////////////
void ExecuteHeadlessIteration(GameResources &game_resources,
                              SceneManager &scene_manager);

/////////////////////////////////////////////////
/// @brief Execute a single full game loop iteration with display
///
/// Performs a complete iteration including display rendering:
/// 1. Update game resources (mouse position, etc.)
/// 2. Process event tick start (preload, waiting room, subscribers)
/// 3. Update scene manager (process subscriptions, update scenes)
/// 4. Call display manager render cycle
/// 5. Process event tick end (tick global bus)
/// 6. Increment loop number
///
/// @param game_resources Reference to GameResources
/// @param scene_manager Reference to SceneManager
/// @param display_manager Reference to DisplayManager
/////////////////////////////////////////////////
void ExecuteFullIteration(GameResources &game_resources,
                          SceneManager &scene_manager,
                          DisplayManager &display_manager);

} // namespace steamrot::game_loop
