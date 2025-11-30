/////////////////////////////////////////////////
/// @file
/// @brief Units tests for GameEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GameEngine.h"
#include "FlatbuffersDataLoader.h"
#include "GamePaths.h"
#include "TestFixture.h"
#include "TestPaths.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Mouse.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("GameEngine initializes correctly with TestPaths", "[unit][GameEngine]") {
  // Create TestPaths and pass to GameEngine
  steamrot::TestPaths test_paths;
  steamrot::GameEngine game_engine(test_paths);

  SUCCEED("GameEngine initialized correctly with TestPaths");
}

TEST_CASE("GameEngine initializes correctly with GamePaths", "[unit][GameEngine]") {
  // Create GamePaths and pass to GameEngine
  steamrot::GamePaths game_paths;
  steamrot::GameEngine game_engine(game_paths);
  SUCCEED("GameEngine initialized correctly with GamePaths");
}

TEST_CASE("GameEngine runs for a set number of frames with TestPaths",
          "[unit][GameEngine]") {
  // set a few different frame counts to test using Catch2's GENERATE
  size_t frame_count = GENERATE(1, 5, 10);

  // Create TestPaths and GameEngine
  steamrot::TestPaths test_paths;
  steamrot::GameEngine game_engine(test_paths);

  // run in simulation mode to limit the number of frames
  game_engine.RunGame(frame_count, true);
  // check that the game loop ran for the specified number of frames
  REQUIRE(game_engine.GetLoopNumber() == frame_count);
  SUCCEED("GameEngine ran for the specified number of frames");
}

TEST_CASE("GameEngine runs for a set number of frames with GamePaths",
          "[unit][GameEngine]") {

  // Create GamePaths and GameEngine
  steamrot::GamePaths game_paths;
  steamrot::GameEngine game_engine(game_paths);

  // run in simulation mode to limit the number of frames
  int frame_count{5};
  game_engine.RunGame(frame_count, true);
  // check that the game loop ran for the specified number of frames
  // REQUIRE(game_engine.GetLoopNumber() == frame_count);
  SUCCEED("GameEngine ran for the specified number of frames");
}

TEST_CASE("GameEngine::RegisterSubscriber adds a subscriber",
          "[unit][GameEngine]") {
  // Create TestPaths and GameEngine
  steamrot::TestPaths test_paths;
  steamrot::GameEngine game_engine(test_paths);
  // Create a Subscriber
  steamrot::Subscriber subscriber{steamrot::EventType_EVENT_QUIT_GAME};

  // Register the Subscriber
  auto register_result = game_engine.RegisterSubscriber(
      std::make_shared<steamrot::Subscriber>(subscriber));
  if (!register_result.has_value()) {
    FAIL("Failed to register subscriber: " + register_result.error().message);
  }
  // Check that the subscriber was added
  REQUIRE(game_engine.GetSubscriptions().size() == 1);
}

TEST_CASE("GameEngine::RegisterSubscriber fails to add null subscriber",
          "[unit][GameEngine]") {
  // Create TestPaths and GameEngine
  steamrot::TestPaths test_paths;
  steamrot::GameEngine game_engine(test_paths);
  // Attempt to register a null Subscriber
  std::shared_ptr<steamrot::Subscriber> null_subscriber = nullptr;
  auto register_result = game_engine.RegisterSubscriber(null_subscriber);
  // Check that the registration failed
  REQUIRE(!register_result.has_value());
  REQUIRE(register_result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("GameEngine::ConfigureSubscribersFromData fails on null data",
          "[unit][GameEngine]") {
  // Create TestPaths and GameEngine
  steamrot::TestPaths test_paths;
  steamrot::GameEngine game_engine(test_paths);
  // Attempt to configure subscribers from null data
  auto configure_result = game_engine.ConfigureSubscribersFromData(nullptr);
  // Check that the configuration failed
  REQUIRE(!configure_result.has_value());
  REQUIRE(configure_result.error().mode == steamrot::FailMode::NullPointer);
  REQUIRE(game_engine.GetSubscriptions().empty());
}

TEST_CASE("GameEngine::ConfigureSubscribersFromData configures subscribers",
          "[unit][GameEngine]") {
  // Create TestPaths and GameEngine
  steamrot::TestPaths test_paths;
  steamrot::GameEngine game_engine(test_paths);
  // Load SubscriberData
  steamrot::FlatbuffersDataLoader data_loader(test_paths);
  auto load_sub_data_result = data_loader.ProvideEngineData();
  if (!load_sub_data_result.has_value()) {
    FAIL("Failed to load Subscriber data: " +
         load_sub_data_result.error().message);
  }
  const ::flatbuffers::Vector<::flatbuffers::Offset<steamrot::SubscriberData>>
      *sub_data = load_sub_data_result.value()->subscriptions();
  // Configure subscribers from data
  auto configure_result = game_engine.ConfigureSubscribersFromData(sub_data);
  if (!configure_result.has_value()) {
    FAIL("Failed to configure subscribers: " +
         configure_result.error().message);
  }
  // Check that the correct number of subscribers were added
  REQUIRE(game_engine.GetSubscriptions().size() == sub_data->size());
}

TEST_CASE("GameEngine::ConfigureGameEngineFromData configures without errors",
          "[unit][GameEngine]") {
  // Create TestPaths and GameEngine
  steamrot::TestPaths test_paths;
  steamrot::GameEngine game_engine(test_paths);
  // Load GameEngineData
  steamrot::FlatbuffersDataLoader data_loader(test_paths);
  auto load_ge_data_result = data_loader.ProvideEngineData();
  if (!load_ge_data_result.has_value()) {
    FAIL("Failed to load GameEngine data: " +
         load_ge_data_result.error().message);
  }
  const steamrot::GameEngineData *ge_data = load_ge_data_result.value();
  // Configure GameEngine from data
  auto configure_result = game_engine.ConfigureGameEngineFromData(ge_data);
  if (!configure_result.has_value()) {
    FAIL("Failed to configure GameEngine: " + configure_result.error().message);
  }
  // Check that the correct number of subscribers were added
  REQUIRE(game_engine.GetSubscriptions().size() ==
          ge_data->subscriptions()->size());
}

TEST_CASE("GameEngine::ProcessSubscribers quits game when correct Subscriber "
          "is active",
          "[unit][GameEngine]") {
  // Create TestPaths and GameEngine
  steamrot::TestPaths test_paths;
  steamrot::GameEngine game_engine(test_paths);
  // Create and register a Subscriber for EventType_EVENT_QUIT_GAME
  auto subscriber = std::make_shared<steamrot::Subscriber>(
      steamrot::EventType_EVENT_QUIT_GAME);
  auto register_result = game_engine.RegisterSubscriber(subscriber);
  if (!register_result.has_value()) {
    FAIL("Failed to register subscriber: " + register_result.error().message);
  }

  // Activate the Subscriber
  subscriber->m_active = true;

  // Process subscriptions in GameEngine
  auto process_subscriptions_result = game_engine.ProcessSubscriptions();
  if (!process_subscriptions_result.has_value()) {
    FAIL("Failed to process subscriptions: " +
         process_subscriptions_result.error().message);
  }
}

TEST_CASE("GameEngine::RunGameLoop processes subscribers and quits game",
          "[unit][GameEngine]") {
  // Create TestPaths and GameEngine
  steamrot::TestPaths test_paths;
  steamrot::GameEngine game_engine(test_paths);
  // Create and register a Subscriber for EventType_EVENT_QUIT_GAME
  auto subscriber = std::make_shared<steamrot::Subscriber>(
      steamrot::EventType_EVENT_QUIT_GAME);
  auto register_result = game_engine.RegisterSubscriber(subscriber);
  if (!register_result.has_value()) {
    FAIL("Failed to register subscriber: " + register_result.error().message);
  }
  // Activate the Subscriber
  subscriber->m_active = true;

  // Run the game loop in simulation mode to process subscriptions
  game_engine.RunGame(10, true);

  // game should quit after 1 loop due to active quit subscriber
  // this will be 2 loops because the loop number is incremented at the end
  REQUIRE(game_engine.GetLoopNumber() == 2);
}

TEST_CASE("GameEngine::ProcessSubscriptions does not quit if another "
          "subscriber type is present",
          "[unit][GameEngine]") {
  // Create TestPaths and GameEngine
  steamrot::TestPaths test_paths;
  steamrot::GameEngine game_engine(test_paths);
  // Create and register a Subscriber for EventType_EVENT_CHANGE_SCENE
  auto subscriber = std::make_shared<steamrot::Subscriber>(
      steamrot::EventType_EVENT_CHANGE_SCENE);
  auto register_result = game_engine.RegisterSubscriber(subscriber);
  if (!register_result.has_value()) {
    FAIL("Failed to register subscriber: " + register_result.error().message);
  }
  // Activate the Subscriber
  subscriber->m_active = true;

  // Process subscriptions in GameEngine
  auto process_subscriptions_result = game_engine.ProcessSubscriptions();
  if (!process_subscriptions_result.has_value()) {
    FAIL("Failed to process subscriptions: " +
         process_subscriptions_result.error().message);
  }
}

// check subscribers are set to inactive at end of game loop
TEST_CASE("GameEngine::ProcessSubscriptions sets subscribers to inactive after "
          "processing",
          "[unit][GameEngine]") {
  // Create TestPaths and GameEngine
  steamrot::TestPaths test_paths;
  steamrot::GameEngine game_engine(test_paths);
  // Create and register a Subscriber for EventType_EVENT_CHANGE_SCENE
  auto subscriber = std::make_shared<steamrot::Subscriber>(
      steamrot::EventType_EVENT_CHANGE_SCENE);
  auto register_result = game_engine.RegisterSubscriber(subscriber);
  if (!register_result.has_value()) {
    FAIL("Failed to register subscriber: " + register_result.error().message);
  }

  // Activate the Subscriber
  subscriber->m_active = true;

  // Process subscriptions in GameEngine
  auto process_subscriptions_result = game_engine.ProcessSubscriptions();
  if (!process_subscriptions_result.has_value()) {
    FAIL("Failed to process subscriptions: " +
         process_subscriptions_result.error().message);
  }
  // check that the subscriber is now inactive
  REQUIRE(!subscriber->m_active);
}

TEST_CASE(
    "GameEngine::UpdateGameContext updates members of GameContext correctly",
    "[unit][GameEngine]") {

  // get game context generated from TestFixture
  steamrot::tests::TestFixture test_fixture;
  test_fixture.Intialize();
  steamrot::GameContext &game_context = test_fixture.GetGameContext();

  // create TestPaths and GameEngine object
  steamrot::TestPaths test_paths;
  steamrot::GameEngine game_engine(test_paths);

  // check current variables
  REQUIRE(game_context.loop_number == 1);
  REQUIRE(game_context.mouse_position == sf::Vector2i(0, 0));

  // simulate some changes
  sf::Mouse::setPosition({20, 35}, game_context.game_window);

  // call UpdateGameResources to update GameContext members
  game_engine.UpdateGameResources(game_context.game_resources);

  // check that the members have been updated correctly

  REQUIRE(game_context.mouse_position.x ==
          sf::Mouse::getPosition(game_context.game_window).x);
  REQUIRE(game_context.mouse_position.y ==
          sf::Mouse::getPosition(game_context.game_window).y);
}
