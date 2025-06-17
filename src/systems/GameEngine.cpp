////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "GameEngine.h"
#include "EventHandler.h"
#include "log_handler.h"
#include "uuid.h"

#include <SFML/Graphics.hpp>
#include <cstddef>

#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_iostream.hpp>
#include <memory>
#include <stdexcept>

using namespace magic_enum::bitwise_operators;

namespace steamrot {

///////////////////////////////////////////////////////////
GameEngine::GameEngine()
    : m_display_manager(), m_data_manager(), m_event_handler(),
      m_asset_manager() {

  m_scene_manager = std::make_unique<SceneManager>();
  std::cout << "GameEngine constructor called" << std::endl;
  log_handler::ProcessLog(spdlog::level::level_enum::info,
                          log_handler::LogCode::kNoCode,
                          "GameEngine constructor called");
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
void GameEngine::StartUp() {

  // Start Title Scene
  ShowTitleScene();
}

void GameEngine::RunGameLoop() {

  // Run the program as long as the window is open
  while (m_display_manager.GetWindow().isOpen()) {

    // // handle loop number increase at beginning of loop
    // m_loop_number++;
    //
    // Handle events and return map of user events
    UserEvents user_events =
        m_event_handler.HandleEvents(m_display_manager.GetWindow());

    // Handle all system updates
    UpdateSystems();

    // Pass render textures to the display manager
    PassRenderPackage();

    // statement to test whether to break the loop, must be called at end
    // if (numLoops > 0 && m_loop_number >= numLoops) {
    //   // export data to json, first variable is the directory name, second
    //   is
    //   // the file name
    //   ExportSimulationData("test");
    //   break;
    // };
  }
}
////////////////////////////////////////////////////////////
void GameEngine::UpdateSystems() {

  // call the update function of the scene manager
  m_scene_manager->UpdateScenes();
}

////////////////////////////////////////////////////////////
void GameEngine::PassRenderPackage() {

  // textures package should only live for the duration of the render call so
  // called by value
  TexturesPackage textures_package = m_scene_manager->ProvideTexturesPackage();
  m_display_manager.Render(textures_package);
};

////////////////////////////////////////////////////////////
size_t GameEngine::getLoopNumber() { return m_loop_number; }

////////////////////////////////////////////////////////////
void GameEngine::RunSimulation(int loops) {
  // prevent undefined behaviour of simulation loop
  if (loops <= 0) {
    throw std::invalid_argument("The number of loops must be greater than 0");
  } else {
    GameEngine::RunGame(loops);
  }
}

////////////////////////////////////////////////////////////
void GameEngine::ShowTitleScene() {
  std::cout << "Loading title scene..." << std::endl;
  // Load the title scene in the scene manager and get unique id
  uuids::uuid title_scene_id = m_scene_manager->LoadTitleScene();
  std::cout << "Title scene loaded with ID: " << title_scene_id << std::endl;
  // pass the id to the display manager
  m_display_manager.LoadTitleSceneTiles(title_scene_id);
  std::cout << "Title scene tiles loaded" << std::endl;
}

////////////////////////////////////////////////////////////
void GameEngine::ShutDown() {}

} // namespace steamrot
