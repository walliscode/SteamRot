/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "GameEngine.h"
#include "FailInfo.h"
#include "FlatbuffersDataLoader.h"
#include "GameContext.h"
#include "SubscriberFactory.h"
#include "events_generated.h"
#include "resources_configuration.h"
#include <SFML/Graphics.hpp>

#include <cstddef>
#include <expected>
#include <iostream>
#include <variant>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////

GameEngine::GameEngine(EnvironmentType env_type)
    : Engine(env_type), m_scene_manager(m_game_context),
      m_display_manager(m_game_resources.game_window, m_scene_manager) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> GameEngine::ConfigureGameEngineFromData(
    const GameEngineData *game_engine_data) {

  if (!game_engine_data) {
    FailInfo fail_info(FailMode::NullPointer,
                       "GameEngineData is a null pointer");
    return std::unexpected(fail_info);
  }

  // configure Subscribers from data
  auto configure_result =
      ConfigureSubscribersFromData(game_engine_data->subscriptions());
  if (!configure_result.has_value()) {
    return std::unexpected(configure_result.error());
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> GameEngine::ConfigureFromData() {
  FlatbuffersDataLoader data_loader;

  // Configure GameResources from resource data
  auto game_resources_result = data_loader.ProvideGameResourcesData();
  if (!game_resources_result) {
    return std::unexpected(game_resources_result.error());
  }

  auto configure_resources_result = resources::ConfigureGameResources(
      m_game_resources, game_resources_result.value());
  if (!configure_resources_result) {
    return std::unexpected(configure_resources_result.error());
  }

  // configure the GameEngine from data
  auto load_data_result = data_loader.ProvideGameEngineData();
  if (!load_data_result) {
    return std::unexpected(load_data_result.error());
  }
  auto configure_result = ConfigureGameEngineFromData(load_data_result.value());
  if (!configure_result) {
    return std::unexpected(configure_result.error());
  }

  // load default assets
  auto load_assets_result = m_game_resources.asset_manager.LoadDefaultAssets();
  if (!load_assets_result) {
    return std::unexpected(load_assets_result.error());
  }

  // Configure the SceneManager from data
  auto scene_manager_data_result = data_loader.ProvideSceneManagerData();
  if (!scene_manager_data_result) {
    return std::unexpected(scene_manager_data_result.error());
  }
  auto configure_sm_result =
      m_scene_manager.ConfigureSceneManagerFromData(scene_manager_data_result.value());
  if (!configure_sm_result) {
    return std::unexpected(configure_sm_result.error());
  }

  // load the title scene
  auto load_scene_result = m_scene_manager.LoadTitleScene();
  if (!load_scene_result) {
    return std::unexpected(load_scene_result.error());
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
void GameEngine::ExecuteTick() {
  UpdateSystems();
}

/////////////////////////////////////////////////
void GameEngine::RunGame(size_t number_of_loops, bool simulation) {

  // set up resources for the game engine using base class StartUp
  auto startup_result = StartUp();
  if (!startup_result.has_value()) {
    std::cerr << "Failed to start up game engine: "
              << startup_result.error().message << "\n";
    if (m_game_resources.game_window.isOpen()) {
      m_game_resources.game_window.close();
    }
    return;
  }

  // Start the game loop
  RunGameLoop(number_of_loops, simulation);
}

void GameEngine::RunGameLoop(size_t number_of_loops, bool simulation) {

  // Run the program as long as the window is open
  while (m_game_resources.game_window.isOpen()) {

    // Handle all system updates
    UpdateSystems();

    // statement to handle simulation mode
    if (simulation && (number_of_loops == m_game_resources.loop_number))
      break;

    // Increment the loop counter
    m_game_resources.loop_number++;
  }
}

/////////////////////////////////////////////////
void GameEngine::UpdateSystems() {
  // Update Game Resources
  UpdateGameResources(m_game_resources);

  // Preload Events, namely any external events that need adding to the waiting
  // room
  m_game_resources.event_handler.PreloadEvents(m_game_resources.game_window);
  // Process Waiting Room Event Bus into Global Event Bus
  m_game_resources.event_handler.ProcessWaitingRoomEventBus();
  // Update Subscribers from Global Event Bus
  m_game_resources.event_handler.UpateSubscribersFromGlobalEventBus();

  // Handle subscriptions for the GameEngine
  auto process_subscriptions_result = ProcessSubscriptions();
  if (!process_subscriptions_result.has_value()) {
    std::cerr << "Failed to process subscriptions: "
              << process_subscriptions_result.error().message << "\n";
    m_game_resources.game_window.close();
  }
  // Update EventHandler

  // Update Scenes
  m_scene_manager.UpdateSceneManager();

  // Call Render Cycle
  auto call_render_cycle_result = m_display_manager.CallRenderCycle();

  // Tick the Global Event Bus
  m_game_resources.event_handler.TickGlobalEventBus();
}

/////////////////////////////////////////////////
size_t GameEngine::GetLoopNumber() const {
  return m_game_resources.loop_number;
}

/////////////////////////////////////////////////
void GameEngine::ShutDown() {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
GameEngine::RegisterSubscriber(std::shared_ptr<Subscriber> subscriber) {
  // add in guard statements and potential fail modes
  if (!subscriber) {
    FailInfo fail_info(FailMode::NullPointer, "Subscriber is a null pointer");
    return std::unexpected(fail_info);
  }

  // add the subscriber to the vector
  m_subscriptions.push_back(subscriber);

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
GameEngine::ConfigureSubscribersFromData(
    const ::flatbuffers::Vector<::flatbuffers::Offset<steamrot::SubscriberData>>
        *subscriptions) {

  if (!subscriptions) {
    FailInfo fail_info(FailMode::NullPointer,
                       "Subscriptions data is a null pointer");
    return std::unexpected(fail_info);
  }

  // set up SubscriberFactory
  SubscriberFactory subscriber_factory(m_game_resources.event_handler);
  // loop through the SubscriberData and create subscribers and register them
  for (const auto &subscription : *subscriptions) {

    // create and register subscriber with EventHandler
    auto create_result =
        subscriber_factory.CreateAndRegisterSubscriber(*subscription);
    if (!create_result.has_value()) {
      return std::unexpected(create_result.error());
    }
    // register produced Subscriber with SceneManager
    auto register_result = RegisterSubscriber(create_result.value());
    if (!register_result.has_value()) {
      return std::unexpected(register_result.error());
    }
  }
  return std::monostate{};
}
/////////////////////////////////////////////////
const std::vector<std::shared_ptr<Subscriber>> &
GameEngine::GetSubscriptions() const {
  return m_subscriptions;
};

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> GameEngine::ProcessSubscriptions() {

  // cycle through all subscribers and process active ones
  for (const auto &subscriber : m_subscriptions) {

    // only process active subscribers
    if (subscriber->m_active) {

      // switch on the EventType
      switch (subscriber->m_trigger_event_type) {
      case EventType::EventType_EVENT_QUIT_GAME: {
        // close the window to quit the game
        m_game_resources.game_window.close();
        break;
      }
      default:
        break;
      }

      // FINALLY set the subscriber to inactive
      subscriber->m_active = false;
    }
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
const sf::RenderWindow &GameEngine::GetWindow() const {
  return m_game_resources.game_window;
}

/////////////////////////////////////////////////
const GameContext &GameEngine::GetGameContext() const { return m_game_context; }

/////////////////////////////////////////////////
void GameEngine::UpdateGameResources(GameResources &game_resources) {

  // update mouse position
  game_resources.mouse_position =
      sf::Mouse::getPosition(game_resources.game_window);
}

} // namespace steamrot
