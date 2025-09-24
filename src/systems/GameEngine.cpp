////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "GameEngine.h"
#include <SFML/Graphics.hpp>

#include <cstddef>
#include <iostream>

namespace steamrot {

///////////////////////////////////////////////////////////

GameEngine::GameEngine(EnvironmentType env_type)
    : m_window({sf::VideoMode({800, 600}), "SteamRot"}),
      m_scene_manager(GameContext{m_window, m_event_handler, m_loop_number,
                                  m_asset_manager, env_type}),
      m_display_manager(m_window, m_scene_manager) {}

////////////////////////////////////////////////////////////
void GameEngine::RunGame(size_t number_of_loops, bool simulation) {

  // set up resources for the game engine
  StartUp();
  //
  // Start the game loop
  RunGameLoop(number_of_loops, simulation);
  //
  // // Shut down the game engine
  // ShutDown();
};

/////////////////////////////////////////////////
void GameEngine::StartUp() {
  // limit window framerate
  m_window.setFramerateLimit(60);

  // load default assets
  auto load_assets_result = m_asset_manager.LoadDefaultAssets();
  if (!load_assets_result)
    if (!load_assets_result) {
      std::cerr << "Failed to load default assets: "
                << load_assets_result.error().message << "\n";
      m_window.close();
    }

  // load the title scene
  auto load_scene_result = m_scene_manager.LoadTitleScene();
  if (!load_scene_result)
    if (!load_scene_result) {
      std::cerr << "Failed to load title scene: "
                << load_scene_result.error().message << "\n";
      m_window.close();
    }
}

void GameEngine::RunGameLoop(size_t number_of_loops, bool simulation) {

  // Run the program as long as the window is open
  while (m_window.isOpen()) {

    // [TODO: Move this to the event handler]
    // add in manual exit strategy
    while (const std::optional event = m_window.pollEvent()) {
      // "close requested" event: we close the window
      if (event->is<sf::Event::Closed>())
        m_window.close();

      // close on escape key press

      if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
          m_window.close();
      } // if the event is a close event, set the close window flag to true
    }
    // Handle all system updates
    UpdateSystems();

    // statement to handle simulation mode
    if (simulation && (number_of_loops == m_loop_number))
      break;
  }
}

////////////////////////////////////////////////////////////
void GameEngine::UpdateSystems() {
  // Update Scenes
  m_scene_manager.UpdateScenes();

  // Call Render Cycle
  auto call_render_cycle_result = m_display_manager.CallRenderCycle();
}

////////////////////////////////////////////////////////////
size_t GameEngine::GetLoopNumber() const { return m_loop_number; }

////////////////////////////////////////////////////////////
void GameEngine::ShutDown() {}

} // namespace steamrot
