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
    : m_window({sf::VideoMode(steamrot::kWindowSize), "SteamRot"}),
      m_data_manager(), m_event_handler(), m_asset_manager(),
      m_display_manager(m_window) {

  // create the GameContext object and pass by value so that it does not have to
  // stay alive
  GameContext game_context{m_window,      m_event_handler, m_mouse_position,
                           m_loop_number, m_asset_manager, m_data_manager};

  // initialise all objects that need the GameContext
  m_scene_manager = std::make_unique<SceneManager>(game_context);

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
    m_event_handler.CleanUpEventBus();
  }
}

/////////////////////////////////////////////////
void GameEngine::RetrieveEvents() {
  // retrieve events from the event handler
  const EventBus &event_bus = m_event_handler.GetEventBus();

  // create holding bus to add any new events to
  EventBus holding_bus;
  // iterate over the event bus and check for events that are relevant to the
  // game engine
  for (const auto &event : event_bus) {
    // check if the event is relevant to the game engine
    if (m_game_engine_events.contains(event.m_event_type)) {

      // if the event is relevant, process it
      ProcessGameEngineEvents(event, holding_bus);
    }
  }
  // add any new events to the event handler's event bus
  m_event_handler.AddEvents(holding_bus);
}

/////////////////////////////////////////////////
void GameEngine::ProcessGameEngineEvents(const EventPacket &event,
                                         EventBus &holding_bus) {
  // process the event based on its type
  switch (event.m_event_type) {

  case (EventType::EventType_EVENT_CHANGE_SCENE): {
    // check if the event has data
    if (std::holds_alternative<SceneChangeData>(event.m_event_data.value())) {
      // get the data from the event
      const SceneChangeData &scene_change_data =
          std::get<SceneChangeData>(event.m_event_data.value());
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
void GameEngine::ShowTitleScene() {
  std::cout << "Loading title scene..." << std::endl;
  // Load the title scene in the scene manager and get unique id
  uuids::uuid title_scene_id = m_scene_manager->LoadTitleScene();
  std::cout << "Title scene loaded with ID: " << title_scene_id << std::endl;
  // pass the id to the display manager
  m_display_manager.LoadTitleSceneTiles(title_scene_id);
  std::cout << "Title scene tiles loaded" << std::endl;
}
/////////////////////////////////////////////////
void GameEngine::ShowCraftingScene() {
  std::cout << "Loading crafting scene..." << std::endl;
  // Load the crafting scene in the scene manager and get unique id
  uuids::uuid crafting_scene_id = m_scene_manager->LoadCraftingScene();
  std::cout << "Crafting scene loaded with ID: " << crafting_scene_id
            << std::endl;
  // pass the id to the display manager
  m_display_manager.LoadCraftingSceneTiles(crafting_scene_id);
  std::cout << "Crafting scene tiles loaded" << std::endl;
}

////////////////////////////////////////////////////////////
void GameEngine::ShutDown() {}

} // namespace steamrot
