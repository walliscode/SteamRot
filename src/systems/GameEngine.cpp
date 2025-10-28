////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "GameEngine.h"
#include "FailInfo.h"
#include "FlatbuffersDataLoader.h"
#include "GameContext.h"
#include "ResourceConfigurator.h"
#include "SubscriberFactory.h"
#include "events_generated.h"
#include <SFML/Graphics.hpp>

#include <cstddef>
#include <expected>
#include <iostream>
#include <variant>
#include <vector>

namespace steamrot {

///////////////////////////////////////////////////////////

GameEngine::GameEngine(EnvironmentType env_type)
    : m_game_context(m_game_resources), m_scene_manager(m_game_context),
      m_display_manager(m_game_resources.game_window, m_scene_manager) {
  // Set initial environment type and loop number
  m_game_resources.env_type = env_type;
  m_game_resources.loop_number = 1;
}

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
  FlatbuffersDataLoader data_loader;

  // Configure GameResources from resource data
  auto game_resources_result = data_loader.ProvideGameResourcesData();
  if (!game_resources_result) {
    std::cerr << "Failed to load game resources data: "
              << game_resources_result.error().message << "\n";
    if (m_game_resources.game_window.isOpen()) {
      m_game_resources.game_window.close();
    }
    return;
  }

  auto configure_resources_result =
      ConfigureGameResources(m_game_resources, game_resources_result.value());
  if (!configure_resources_result) {
    std::cerr << "Failed to configure game resources: "
              << configure_resources_result.error().message << "\n";
    if (m_game_resources.game_window.isOpen()) {
      m_game_resources.game_window.close();
    }
    return;
  }

  // configure the GameEngine from data
  auto load_data_result = data_loader.ProvideGameEngineData();
  if (!load_data_result) {
    std::cerr << "Failed to load game engine data: "
              << load_data_result.error().message << "\n";
    m_game_resources.game_window.close();
    return;
  }
  auto configure_result = ConfigureGameEngineFromData(load_data_result.value());
  if (!configure_result) {
    std::cerr << "Failed to configure game engine: "
              << configure_result.error().message << "\n";
    m_game_resources.game_window.close();
    return;
  }

  // load default assets
  auto load_assets_result = m_game_resources.asset_manager.LoadDefaultAssets();
  if (!load_assets_result) {
    std::cerr << "Failed to load default assets: "
              << load_assets_result.error().message << "\n";
    m_game_resources.game_window.close();
    return;
  }

  // Configure the SceneManager from data
  auto configure_sm_result = m_scene_manager.ConfigureSceneManagerFromData(
      data_loader.ProvideSceneManagerData().value());
  if (!configure_sm_result) {
    std::cerr << "Failed to configure scene manager: "
              << configure_sm_result.error().message << "\n";
    m_game_resources.game_window.close();
    return;
  }
  // load the title scene
  auto load_scene_result = m_scene_manager.LoadTitleScene();
  if (!load_scene_result) {
    std::cerr << "Failed to load title scene: "
              << load_scene_result.error().message << "\n";
    m_game_resources.game_window.close();
    return;
  }
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

////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////
size_t GameEngine::GetLoopNumber() const {
  return m_game_resources.loop_number;
}

////////////////////////////////////////////////////////////
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
    if (subscriber->IsActive()) {

      // get the event data
      const EventData &event_data = subscriber->GetEventData();

      // switch on the EventType
      switch (subscriber->GetEventType()) {
      case EventType::EventType_EVENT_QUIT_GAME: {
        // close the window to quit the game
        m_game_resources.game_window.close();
        break;
      }
      default:
        break;
      }

      // FINALLY set the subscriber to inactive
      auto set_inactive_result = subscriber->SetInactive();
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
