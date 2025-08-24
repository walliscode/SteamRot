////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "GameEngine.h"
#include "EventHandler.h"
#include "GameContext.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>

#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_iostream.hpp>
#include <stdexcept>

using namespace magic_enum::bitwise_operators;

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
void GameEngine::StartUp() {

  // Start Title Scene
  ShowTitleScene();
}

void GameEngine::RunGameLoop() {

  // Run the program as long as the window is open
  while (m_window.isOpen()) {

    // handle loop number increase at beginning of loop
    m_loop_number++;

    // Handle and events from external sources, such as the window
    m_event_handler.PreloadEvents(m_window);

    // Retreive and process events at the GameEngine level
    RetrieveEvents();

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

    // update EventBus lifetimes
    m_event_handler.TickGlobalEventBus();
  }
}

/////////////////////////////////////////////////
void GameEngine::RetrieveEvents() {
  // retrieve events from the event handler
  const EventBus &event_bus = m_event_handler.GetGlobalEventBus();

  EventBus holding_bus;
  // iterate over the event bus and check for events that are relevant to the
  // game engine
  for (const auto &event : event_bus) {

    ProcessGameEngineEvents(event, holding_bus);
  }
  // add any new events to the event handler's event bus
  m_event_handler.AddToGlobalEventBus(holding_bus);
}

/////////////////////////////////////////////////
void GameEngine::ProcessGameEngineEvents(const EventPacket &event,
                                         EventBus &holding_bus) {
  // process the event based on its type
  switch (event.m_event_type) {

  case (EventType::EventType_EVENT_CHANGE_SCENE): {
    // check if the event has data
    if (std::holds_alternative<SceneChangeData>(event.m_event_data)) {
      std::cout << "Scene change event received: "
                << magic_enum::enum_name(event.m_event_type) << std::endl;
      // get the data from the event
      const SceneChangeData &scene_change_data =
          std::get<SceneChangeData>(event.m_event_data);
      // check if the scene change data has a new scene type
      if (scene_change_data.second == SceneType::SceneType_TITLE) {
        ShowTitleScene();
      } else if (scene_change_data.second == SceneType::SceneType_CRAFTING) {
        ShowCraftingScene();
      }
    }
    break;
  }
  case (EventType::EventType_EVENT_QUIT_GAME): {
    // quit the game
    m_window.close();
  }
  default:
    // no action, do nothing
    break;
  }
}

////////////////////////////////////////////////////////////
void GameEngine::UpdateSystems() {}

////////////////////////////////////////////////////////////
void GameEngine::PassRenderPackage() {

};

////////////////////////////////////////////////////////////
size_t GameEngine::getLoopNumber() { return m_loop_number; }

/////////////////////////////////////////////////
sf::RenderWindow &GameEngine::GetWindow() { return m_window; }

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
void GameEngine::ShowTitleScene() {}
/////////////////////////////////////////////////
void GameEngine::ShowCraftingScene() {}

////////////////////////////////////////////////////////////
void GameEngine::ShutDown() {}

} // namespace steamrot
