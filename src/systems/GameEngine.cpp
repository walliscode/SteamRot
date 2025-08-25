////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "GameEngine.h"
#include "EventHandler.h"
#include "GameContext.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>

namespace steamrot {

///////////////////////////////////////////////////////////
GameEngine::GameEngine(EnvironmentType env_type)
    : m_window({sf::VideoMode({800, 600}), "SteamRot"}), m_event_handler(),
      m_asset_manager() {

  // create the GameContext object and pass by value so that it does not have to
  // stay alive
  GameContext game_context{m_window,      m_event_handler, m_mouse_position,
                           m_loop_number, m_asset_manager, env_type};
}

////////////////////////////////////////////////////////////
void GameEngine::RunGame(size_t numLoops, bool use_test_window) {

  // set up resources for the game engine
  StartUp();

  // Start the game loop
  RunGameLoop();

  // Shut down the game engine
  ShutDown();
};

/////////////////////////////////////////////////
void GameEngine::StartUp() {}

void GameEngine::RunGameLoop() {

  // Run the program as long as the window is open
  while (m_window.isOpen()) {

    // handle loop number increase at beginning of loop
    m_loop_number++;

    // Handle and events from external sources, such as the window
    m_event_handler.PreloadEvents(m_window);

    // Handle all system updates
    UpdateSystems();

    // statement to test whether to break the loop, must be called at end
    // if (numLoops > 0 && m_loop_number >= numLoops) {
    //   // export data to json, first variable is the directory name, second
    //   is
    //   // the file name
    //   ExportSimulationData("test");
    //   break;
    // };

    // update EventBus lifetimes
    m_event_handler.TickGlobalEventBus();
  }
}

////////////////////////////////////////////////////////////
void GameEngine::UpdateSystems() {}

////////////////////////////////////////////////////////////
size_t GameEngine::getLoopNumber() { return m_loop_number; }

/////////////////////////////////////////////////
sf::RenderWindow &GameEngine::GetWindow() { return m_window; }

////////////////////////////////////////////////////////////
void GameEngine::ShutDown() {}

} // namespace steamrot
