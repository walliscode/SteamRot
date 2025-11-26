/////////////////////////////////////////////////
/// @file
/// @brief Implementation of game loop free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "game_loop.h"
#include "event_processing.h"
#include <SFML/Window/Mouse.hpp>

namespace steamrot::game_loop {

/////////////////////////////////////////////////
void UpdateGameResources(GameResources &game_resources) {
  // Update mouse position from the game window
  game_resources.mouse_position =
      sf::Mouse::getPosition(game_resources.game_window);
}

/////////////////////////////////////////////////
void ExecuteHeadlessIteration(GameResources &game_resources,
                              SceneManager &scene_manager) {
  // 1. Update game resources
  UpdateGameResources(game_resources);

  // 2. Process event tick start (no window for headless)
  events::processing::ProcessEventTickStart(game_resources.event_handler,
                                            nullptr);

  // 3. Update scene manager
  scene_manager.UpdateSceneManager();

  // 4. Process event tick end
  events::processing::ProcessEventTickEnd(game_resources.event_handler);

  // 5. Increment loop number
  game_resources.loop_number++;
}

/////////////////////////////////////////////////
void ExecuteFullIteration(GameResources &game_resources,
                          SceneManager &scene_manager,
                          DisplayManager &display_manager) {
  // 1. Update game resources
  UpdateGameResources(game_resources);

  // 2. Process event tick start (with window for SFML events)
  events::processing::ProcessEventTickStart(game_resources.event_handler,
                                            &game_resources.game_window);

  // 3. Update scene manager
  scene_manager.UpdateSceneManager();

  // 4. Call render cycle
  display_manager.CallRenderCycle();

  // 5. Process event tick end
  events::processing::ProcessEventTickEnd(game_resources.event_handler);

  // 6. Increment loop number
  game_resources.loop_number++;
}

} // namespace steamrot::game_loop
