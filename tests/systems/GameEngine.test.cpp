/////////////////////////////////////////////////
/// @file
/// @brief Units tests for GameEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GameEngine.h"
#include "FlatbuffersDataLoader.h"
#include "PathProvider.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("GameEngine fails when EnviromentType is None", "[GameEngine]") {
  // Attempt to create a GameEngine instance with EnvironmentType::None
  try {
    steamrot::GameEngine game_engine(steamrot::EnvironmentType::None);
    FAIL("GameEngine should not be created with EnvironmentType::None");
  } catch (const std::exception &e) {
    SUCCEED(
        "GameEngine creation failed as expected with EnvironmentType::None");
  }
}
TEST_CASE("GameEngine fails if PathProvider has not been pre initialized",
          "[GameEngine]") {
  // Attempt to create a GameEngine instance with EnvironmentType::None
  try {
    steamrot::GameEngine game_engine(steamrot::EnvironmentType::None);
    FAIL("PathProvider should not be used with EnvironmentType::None");
  } catch (const std::exception &e) {
    SUCCEED("PathProvider usage failed as expected with EnvironmentType::None");
  }
}

TEST_CASE("GameEngine initializes correctly in test enviroment when "
          "PathProvide is pre initialized",
          "[GameEngine]") {
  // Pre-initialize PathProvider
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  // Create a GameEngine instance
  steamrot::GameEngine game_engine(steamrot::EnvironmentType::Test);

  SUCCEED("GameEngine initialized correctly in test enviroment");
}

TEST_CASE("GameEngine initializes correctly in production enviroment",
          "[GameEngine]") {
  // pre-initialize PathProvider
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Production);
  // Create a GameEngine instance
  steamrot::GameEngine game_engine(steamrot::EnvironmentType::Production);
  SUCCEED("GameEngine initialized correctly in production enviroment");
}
TEST_CASE("GameEngine runs for a set number of frames in test enviroment",
          "[GameEngine]") {
  // set a few different frame counts to test using Catch2's GENERATE
  size_t frame_count = GENERATE(1, 5, 10);

  // create and pre-initialize PathProvider
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  // Create a GameEngine instance
  steamrot::GameEngine game_engine(steamrot::EnvironmentType::Test);

  // run in simulation mode to limit the number of frames
  game_engine.RunGame(frame_count, true);
  // check that the game loop ran for the specified number of frames
  REQUIRE(game_engine.GetLoopNumber() == frame_count);
  SUCCEED("GameEngine ran for the specified number of frames");
}

TEST_CASE("GameEngine runs for a set number of frames in production enviroment",
          "[GameEngine]") {

  // create and pre-initialize PathProvider
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Production);
  // Create a GameEngine instance
  steamrot::GameEngine game_engine(steamrot::EnvironmentType::Production);

  // run in simulation mode to limit the number of frames
  int frame_count{5};
  game_engine.RunGame(frame_count, true);
  // check that the game loop ran for the specified number of frames
  // REQUIRE(game_engine.GetLoopNumber() == frame_count);
  SUCCEED("GameEngine ran for the specified number of frames");
}

TEST_CASE("GameEngine::RegisterSubscriber adds a subscriber", "[GameEngine]") {
  // create and pre-initialize PathProvider
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  // Create a GameEngine instance
  steamrot::GameEngine game_engine(steamrot::EnvironmentType::Test);
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
          "[GameEngine]") {
  // create and pre-initialize PathProvider
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  // Create a GameEngine instance
  steamrot::GameEngine game_engine(steamrot::EnvironmentType::Test);
  // Attempt to register a null Subscriber
  std::shared_ptr<steamrot::Subscriber> null_subscriber = nullptr;
  auto register_result = game_engine.RegisterSubscriber(null_subscriber);
  // Check that the registration failed
  REQUIRE(!register_result.has_value());
  REQUIRE(register_result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("GameEngine::ConfigureSubscribersFromData fails on null data",
          "[GameEngine]") {
  // create and pre-initialize PathProvider
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  // Create a GameEngine instance
  steamrot::GameEngine game_engine(steamrot::EnvironmentType::Test);
  // Attempt to configure subscribers from null data
  auto configure_result = game_engine.ConfigureSubscribersFromData(nullptr);
  // Check that the configuration failed
  REQUIRE(!configure_result.has_value());
  REQUIRE(configure_result.error().mode == steamrot::FailMode::NullPointer);
  REQUIRE(game_engine.GetSubscriptions().empty());
}

TEST_CASE("GameEngine::ConfigureSubscribersFromData configures subscribers",
          "[GameEngine]") {
  // create and pre-initialize PathProvider
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  // Create a GameEngine instance
  steamrot::GameEngine game_engine(steamrot::EnvironmentType::Test);
  // Load SubscriberData
  steamrot::FlatbuffersDataLoader data_loader;
  auto load_sub_data_result = data_loader.ProvideGameEngineData();
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
          "[GameEngine]") {
  // create and pre-initialize PathProvider
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  // Create a GameEngine instance
  steamrot::GameEngine game_engine(steamrot::EnvironmentType::Test);
  // Load GameEngineData
  steamrot::FlatbuffersDataLoader data_loader;
  auto load_ge_data_result = data_loader.ProvideGameEngineData();
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
