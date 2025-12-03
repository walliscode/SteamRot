/////////////////////////////////////////////////
/// @file
/// @brief Units tests for GameEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GameEngine.h"
#include "FlatbuffersDataLoader.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Mouse.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("GameEngine initializes correctly", "[unit][GameEngine]") {
  steamrot::GameEngine game_engine;

  SUCCEED("GameEngine initialized correctly");
}

TEST_CASE("GameEngine::RegisterSubscriber adds a subscriber",
          "[unit][GameEngine]") {
  steamrot::GameEngine game_engine;
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
  steamrot::GameEngine game_engine;
  // Attempt to register a null Subscriber
  std::shared_ptr<steamrot::Subscriber> null_subscriber = nullptr;
  auto register_result = game_engine.RegisterSubscriber(null_subscriber);
  // Check that the registration failed
  REQUIRE(!register_result.has_value());
  REQUIRE(register_result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("GameEngine::ConfigureSubscribersFromData fails on null data",
          "[unit][GameEngine]") {
  steamrot::GameEngine game_engine;
  // Attempt to configure subscribers from null data
  auto configure_result = game_engine.ConfigureSubscribersFromData(nullptr);
  // Check that the configuration failed
  REQUIRE(!configure_result.has_value());
  REQUIRE(configure_result.error().mode == steamrot::FailMode::NullPointer);
  REQUIRE(game_engine.GetSubscriptions().empty());
}

TEST_CASE("GameEngine::ConfigureSubscribersFromData configures subscribers",
          "[unit][GameEngine]") {
  steamrot::GameEngine game_engine;
  // Load SubscriberData
  steamrot::FlatbuffersDataLoader data_loader;
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
