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
    : m_asset_manager(env_type),
      m_window({sf::VideoMode({800, 600}), "SteamRot"}),
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

    // Handle all system updates
    UpdateSystems();

    // statement to handle simulation mode
    if (simulation && (number_of_loops == m_loop_number))
      break;

    std::cout << "Loop number: " << m_loop_number << "\n";
    m_loop_number++;
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
