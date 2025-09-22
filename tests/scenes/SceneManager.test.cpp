/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for SceneManager class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneManager.h"

#include "FlatbuffersDataLoader.h"
#include "GameContext.h"
#include "PathProvider.h"
#include "TestContext.h"

#include "asset_helpers.h"
#include "events_generated.h"
#include "scene_types_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SceneManager is constructed without any errors", "[SceneManager]") {

  steamrot::tests::TestContext test_context;
  steamrot::GameContext game_context =
      test_context
          .GetGameContext(); // Get the game context from the test context
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};
  REQUIRE_NOTHROW(scene_manager);

  // test number of subscriptions
  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
  auto load_sm_data_result = data_loader.ProvideSceneManagerData();
  if (!load_sm_data_result.has_value()) {
    FAIL("Failed to load SceneManager data: " +
         load_sm_data_result.error().message);
  }
  const steamrot::SceneManagerData *sm_data = load_sm_data_result.value();
}

TEST_CASE("SceneManager::RegistersSubscriber  adds a subscriber",
          "[SceneManager]") {
  steamrot::tests::TestContext test_context;
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};
  // Create a Subscriber
  steamrot::Subscriber subscriber{steamrot::EventType_EVENT_CHANGE_SCENE};
  // Register the Subscriber
  auto register_result = scene_manager.RegisterSubscriber(
      std::make_shared<steamrot::Subscriber>(subscriber));
  if (!register_result.has_value()) {
    FAIL("Failed to register subscriber: " + register_result.error().message);
  }
  // Check that the subscriber was added
  REQUIRE(scene_manager.GetSubscriptions().size() == 1);
}

TEST_CASE("SceneManager::RegistersSubscriber fails to add duplicate subscriber",
          "[SceneManager]") {
  steamrot::tests::TestContext test_context;
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};
  // Create a Subscriber
  auto subscriber = std::make_shared<steamrot::Subscriber>(
      steamrot::EventType_EVENT_CHANGE_SCENE);
  // Register the Subscriber
  auto register_result = scene_manager.RegisterSubscriber(subscriber);
  if (!register_result.has_value()) {
    FAIL("Failed to register subscriber: " + register_result.error().message);
  }
  // Attempt to register the same Subscriber again
  auto duplicate_register_result = scene_manager.RegisterSubscriber(subscriber);
  // Check that the second registration failed
  REQUIRE(!duplicate_register_result.has_value());
  REQUIRE(duplicate_register_result.error().mode ==
          steamrot::FailMode::NotAddedToMap);
  REQUIRE(scene_manager.GetSubscriptions().size() == 1);
}

TEST_CASE("SceneManager::RegistersSubscriber fails to add null subscriber",
          "[SceneManager]") {
  steamrot::tests::TestContext test_context;
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};
  // Attempt to register a null Subscriber
  std::shared_ptr<steamrot::Subscriber> null_subscriber = nullptr;
  auto register_result = scene_manager.RegisterSubscriber(null_subscriber);
  // Check that the registration failed
  REQUIRE(!register_result.has_value());
  REQUIRE(register_result.error().mode == steamrot::FailMode::NullPointer);
  REQUIRE(scene_manager.GetSubscriptions().empty());
}

TEST_CASE("SceneManager::ConfigureSubscribersFromData configures subscribers",
          "[SceneManager]") {
  steamrot::tests::TestContext test_context;
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};
  // Load SceneManagerData
  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
  auto load_sm_data_result = data_loader.ProvideSceneManagerData();
  if (!load_sm_data_result.has_value()) {
    FAIL("Failed to load SceneManager data: " +
         load_sm_data_result.error().message);
  }
  const steamrot::SceneManagerData *sm_data = load_sm_data_result.value();
  // check that the EventHandler subscriptions are empty
  REQUIRE(test_context.GetGameContext()
              .event_handler.GetSubcriberRegister()
              .empty());
  // Configure subscribers from data
  auto configure_result =
      scene_manager.ConfigureSubscribersFromData(sm_data->subscriptions());
  if (!configure_result.has_value()) {
    FAIL("Failed to configure subscribers: " +
         configure_result.error().message);
  }
  // Check that the correct number of subscribers were added
  REQUIRE(scene_manager.GetSubscriptions().size() ==
          sm_data->subscriptions()->size());
  REQUIRE(test_context.GetGameContext()
              .event_handler.GetSubcriberRegister()
              .size() == sm_data->subscriptions()->size());

  // Check that specific subscribers were added using contains
  REQUIRE(scene_manager.GetSubscriptions().contains(
      steamrot::EventType_EVENT_CHANGE_SCENE));
}

TEST_CASE("SceneManager::ConfigureSubscribersFromData fails on null data",
          "[SceneManager]") {
  steamrot::tests::TestContext test_context;
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};
  // Attempt to configure subscribers from null data
  auto configure_result = scene_manager.ConfigureSubscribersFromData(nullptr);
  // Check that the configuration failed
  REQUIRE(!configure_result.has_value());
  REQUIRE(configure_result.error().mode == steamrot::FailMode::NullPointer);
  REQUIRE(scene_manager.GetSubscriptions().empty());
}

TEST_CASE("SceneManager::ConfigureSceneManagerFromData configures without "
          "errors",
          "[SceneManager]") {
  steamrot::tests::TestContext test_context;
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};
  // Load SceneManagerData
  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
  auto load_sm_data_result = data_loader.ProvideSceneManagerData();
  if (!load_sm_data_result.has_value()) {
    FAIL("Failed to load SceneManager data: " +
         load_sm_data_result.error().message);
  }
  const steamrot::SceneManagerData *sm_data = load_sm_data_result.value();
  // Configure SceneManager from data
  auto configure_result = scene_manager.ConfigureSceneManagerFromData(sm_data);
  if (!configure_result.has_value()) {
    FAIL("Failed to configure SceneManager: " +
         configure_result.error().message);
  }
  // Check that the correct number of subscribers were added
  REQUIRE(scene_manager.GetSubscriptions().size() ==
          sm_data->subscriptions()->size());
}
TEST_CASE("SceneManager's AddSceneFromDefault creates a configured "
          "TitleScene",
          "[SceneManager]") {

  steamrot::tests::TestContext test_context;

  steamrot::GameContext game_context =
      test_context.GetGameContext(); // Get the game context from the
                                     // test context
  steamrot::SceneManager scene_manager{game_context};

  // define the scene type
  const steamrot::SceneType scene_type = steamrot::SceneType::SceneType_TITLE;

  // add the scene from default data
  auto title_result = scene_manager.AddSceneFromDefault(scene_type);

  if (!title_result.has_value()) {
    FAIL("Failed to create test scene from default: " +
         title_result.error().message);
  }

  // check AssetManger config
  steamrot::tests::check_asset_configuration(scene_type,
                                             game_context.asset_manager);
}

TEST_CASE("SceneManager's AddSceneFromDefault creates a configured "
          "CraftingScene",
          "[SceneManager]") {

  steamrot::tests::TestContext test_context;
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};

  // define the scene type
  const steamrot::SceneType scene_type =
      steamrot::SceneType::SceneType_CRAFTING;
  // add the scene from default data
  auto crafting_result = scene_manager.AddSceneFromDefault(scene_type);
  if (!crafting_result.has_value()) {
    FAIL("Failed to create test scene from default: " +
         crafting_result.error().message);
  }
  // check AssetManger config
  steamrot::tests::check_asset_configuration(
      scene_type, test_context.GetGameContext().asset_manager);
}

TEST_CASE("SceneManager LoadTitleScene returns monostate", "[SceneManager]") {

  steamrot::tests::TestContext test_context;
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};
  auto load_title_result = scene_manager.LoadTitleScene();

  if (!load_title_result.has_value()) {
    FAIL("Failed to load title scene: " + load_title_result.error().message);
  }

  // check AssetManger config
  steamrot::tests::check_asset_configuration(
      steamrot::SceneType::SceneType_TITLE,
      test_context.GetGameContext().asset_manager);
}

TEST_CASE("SceneManager LoadCraftingScene returns monostate",
          "[SceneManager]") {

  steamrot::tests::TestContext test_context;
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};
  auto load_crafting_result = scene_manager.LoadCraftingScene();
  if (!load_crafting_result.has_value()) {
    FAIL("Failed to load crafting scene: " +
         load_crafting_result.error().message);
  }

  // check AssetManger config
  steamrot::tests::check_asset_configuration(
      steamrot::SceneType::SceneType_CRAFTING,
      test_context.GetGameContext().asset_manager);
}

TEST_CASE("SceneManager::ProvideTextures returns empty map for no "
          "scene IDs",
          "[SceneManager]") {
  steamrot::tests::TestContext test_context;
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};
  // create an empty vector of scene IDs
  std::vector<uuids::uuid> scene_ids;
  // call ProvideTextures with the empty vector
  auto textures_result = scene_manager.ProvideTextures(scene_ids);
  if (!textures_result.has_value()) {
    FAIL(textures_result.error().message);
  }
  // check that the returned map is empty
  REQUIRE(textures_result->empty());
}

TEST_CASE("SceneManager::ProvideTextures returns FailInfo for "
          "invalid scene ID",
          "[SceneManager]") {
  steamrot::tests::TestContext test_context;
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};
  // create a vector with an invalid scene ID
  std::vector<uuids::uuid> scene_ids = {uuids::uuid{}};
  // call ProvideTextures with the invalid scene ID
  auto textures_result = scene_manager.ProvideTextures(scene_ids);
  // check that the result is a failure
  REQUIRE(!textures_result.has_value());
  REQUIRE(textures_result.error().message ==
          "Scene ID not found in SceneManager");
}

TEST_CASE("SceneManager::ProvideTextures returns textures for valid "
          "scene IDs",
          "[SceneManager]") {
  steamrot::tests::TestContext test_context;
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};
  // Add a Title scene and a Crafting scene to the SceneManager
  auto title_result = scene_manager.LoadTitleScene();
  if (!title_result.has_value()) {
    FAIL("Failed to add Title scene: " + title_result.error().message);
  }
  // create a vector with the valid scene IDs
  std::vector<uuids::uuid> scene_ids = {title_result.value()};

  // call ProvideTextures with the valid scene IDs
  auto textures_result = scene_manager.ProvideTextures(scene_ids);
  if (!textures_result.has_value()) {
    FAIL("Failed to provide textures: " + textures_result.error().message);
  }
  // check that the returned map has the correct number of textures
  REQUIRE(!textures_result->empty());
  REQUIRE(textures_result->size() == scene_ids.size());
}

TEST_CASE("SceneManager::ProvideAvailableSceneInfo returns available "
          "SceneInfo",
          "[SceneManager]") {
  steamrot::tests::TestContext test_context;
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};
  // add a Title scene
  auto title_result = scene_manager.LoadTitleScene();
  if (!title_result.has_value()) {
    FAIL("Failed to add Title scene: " + title_result.error().message);
  }
  // call ProvideSceneInfo
  auto scene_info_result = scene_manager.ProvideAvailableSceneInfo();
  if (!scene_info_result.has_value()) {
    FAIL("Failed to provide scene info: " + scene_info_result.error().message);
  }
  // check that the returned vector has the correct number of
  // SceneInfo entries
  REQUIRE(!scene_info_result->empty());
  REQUIRE(scene_info_result->size() == 1);
  REQUIRE(scene_info_result->at(0).type ==
          steamrot::SceneType::SceneType_TITLE);
  REQUIRE(scene_info_result->at(0).id == title_result.value());
}

TEST_CASE("SceneManager RegistersSubscribers", "[SceneManager]") {
  // set up SceneManager
  steamrot::tests::TestContext test_context;
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};
}
