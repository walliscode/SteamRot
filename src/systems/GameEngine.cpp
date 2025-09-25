////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "GameEngine.h"
#include "FailInfo.h"
#include "SubscriberFactory.h"
#include <SFML/Graphics.hpp>

#include <cstddef>
#include <expected>
#include <iostream>
#include <variant>
#include <vector>

namespace steamrot {

///////////////////////////////////////////////////////////

GameEngine::GameEngine(EnvironmentType env_type)
    : m_window({sf::VideoMode({800, 600}), "SteamRot"}),
      m_scene_manager(GameContext{m_window, m_event_handler, m_loop_number,
                                  m_asset_manager, env_type}),
      m_display_manager(m_window, m_scene_manager) {}

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

    // Handle subscriptions
    auto process_subscriptions_result = ProcessSubscriptions();
    if (!process_subscriptions_result.has_value()) {
      std::cerr << "Failed to process subscriptions: "
                << process_subscriptions_result.error().message << "\n";
      m_window.close();
    }

    // Handle all system updates
    UpdateSystems();

    // statement to handle simulation mode
    if (simulation && (number_of_loops == m_loop_number))
      break;

    // Increment the loop counter
    m_loop_number++;
  }
}

////////////////////////////////////////////////////////////
void GameEngine::UpdateSystems() {
  // Update Scenes
  m_scene_manager.UpdateSceneManager();

  // Call Render Cycle
  auto call_render_cycle_result = m_display_manager.CallRenderCycle();
}

////////////////////////////////////////////////////////////
size_t GameEngine::GetLoopNumber() const { return m_loop_number; }

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
  SubscriberFactory subscriber_factory(m_event_handler);
  // loop through the SubscriberData and create subscribers and register them
  for (const auto &subscription : *subscriptions) {

    // create and register subscriber with EventHandler
    auto create_result = subscriber_factory.CreateAndRegisterSubscriber(
        subscription->event_type_data());
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
        m_window.close();
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
const sf::RenderWindow &GameEngine::GetWindow() const { return m_window; }

} // namespace steamrot
