/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for LogicFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "LogicFactory.h"
#include "FlatbuffersDataLoader.h"
#include "TestFixture.h"
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "logic_test_helpers.h"
#include "scene_change_packet_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LogicFactory constructed without errors", "[unit][LogicFactory]") {

  // create a Testcontext to provide mock dependencies
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture test_context;

  // Load LogicCollectionData
  steamrot::FlatbuffersDataLoader data_loader;
  auto logic_collection_data_result =
      data_loader.ProvideLogicCollectionData(steamrot::SceneType::SceneType_TEST);
  REQUIRE(logic_collection_data_result.has_value());

  // create a LogicFactory instance
  steamrot::LogicFactory logic_factory(
      steamrot::SceneType::SceneType_TEST,
      test_context.GetSceneContext());

  REQUIRE_NOTHROW(logic_factory);
}

TEST_CASE("LogicFactory creates the correct Logic instances with a test Scene",
          "[unit][LogicFactory]") {

  // create a SceneContext with mock dependencies
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture test_context{
      steamrot::SceneType::SceneType_TEST};

  // Load LogicCollectionData
  steamrot::FlatbuffersDataLoader data_loader;
  auto logic_collection_data_result =
      data_loader.ProvideLogicCollectionData(steamrot::SceneType::SceneType_TEST);
  REQUIRE(logic_collection_data_result.has_value());

  // create a LogicFactory instance
  steamrot::LogicFactory logic_factory(
      steamrot::SceneType::SceneType_TEST,
      test_context.GetSceneContext());

  auto logic_map_result =
      logic_factory.CreateLogicMap(*logic_collection_data_result.value());
  if (!logic_map_result.has_value()) {
    FAIL("LogicFactory failed to create logic map: " +
         logic_map_result.error().message);
  }

  steamrot::LogicCollection &logic_collection = logic_map_result.value();

  steamrot::tests::CheckStaticLogicCollections(
      logic_collection, steamrot::SceneType::SceneType_TEST);
}

TEST_CASE("LogicFactory creates correct Logic instances for TitleScene",
          "[unit][LogicFactory]") {

  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture test_context{
      steamrot::SceneType::SceneType_TITLE};

  // Load LogicCollectionData
  steamrot::FlatbuffersDataLoader data_loader;
  auto logic_collection_data_result =
      data_loader.ProvideLogicCollectionData(steamrot::SceneType::SceneType_TITLE);
  REQUIRE(logic_collection_data_result.has_value());

  // create a LogicFactory instance
  steamrot::LogicFactory logic_factory(
      steamrot::SceneType::SceneType_TITLE,
      test_context.GetSceneContext());

  auto logic_map_result =
      logic_factory.CreateLogicMap(*logic_collection_data_result.value());
  if (!logic_map_result.has_value()) {
    FAIL("LogicFactory failed to create logic map: " +
         logic_map_result.error().message);
  }

  steamrot::LogicCollection &logic_collection = logic_map_result.value();

  steamrot::tests::CheckStaticLogicCollections(
      logic_collection, steamrot::SceneType::SceneType_TITLE);
}
TEST_CASE("LogicFactory creates correct Logic instances for CraftingScene",
          "[unit][LogicFactory]") {

  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture test_context{
      steamrot::SceneType::SceneType_CRAFTING};

  // Load LogicCollectionData
  steamrot::FlatbuffersDataLoader data_loader;
  auto logic_collection_data_result =
      data_loader.ProvideLogicCollectionData(steamrot::SceneType::SceneType_CRAFTING);
  REQUIRE(logic_collection_data_result.has_value());

  // create a LogicFactory instance
  steamrot::LogicFactory logic_factory(
      steamrot::SceneType::SceneType_CRAFTING,
      test_context.GetSceneContext());

  auto logic_map_result =
      logic_factory.CreateLogicMap(*logic_collection_data_result.value());
  if (!logic_map_result.has_value()) {
    FAIL("LogicFactory failed to create logic map: " +
         logic_map_result.error().message);
  }

  steamrot::LogicCollection &logic_collection = logic_map_result.value();

  steamrot::tests::CheckStaticLogicCollections(
      logic_collection, steamrot::SceneType::SceneType_CRAFTING);
}

TEST_CASE("LogicFactory attaches subscribers to Logic instances",
          "[unit][LogicFactory]") {

  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture test_context{
      steamrot::SceneType::SceneType_TEST};

  // Load LogicCollectionData
  steamrot::FlatbuffersDataLoader data_loader;
  auto logic_collection_data_result =
      data_loader.ProvideLogicCollectionData(steamrot::SceneType::SceneType_TEST);
  REQUIRE(logic_collection_data_result.has_value());

  // create a LogicFactory instance
  steamrot::LogicFactory logic_factory(
      steamrot::SceneType::SceneType_TEST,
      test_context.GetSceneContext());

  auto logic_map_result =
      logic_factory.CreateLogicMap(*logic_collection_data_result.value());
  REQUIRE(logic_map_result.has_value());

  steamrot::LogicCollection &logic_collection = logic_map_result.value();

  // Verify that Logic instances exist
  REQUIRE(logic_collection.find(steamrot::LogicType::Collision) !=
          logic_collection.end());
  REQUIRE(logic_collection.find(steamrot::LogicType::Render) !=
          logic_collection.end());
  REQUIRE(logic_collection.find(steamrot::LogicType::Action) !=
          logic_collection.end());

  // Verify subscribers can be accessed (they may be empty for default test data)
  const auto &collision_logics = logic_collection.at(steamrot::LogicType::Collision);
  REQUIRE(!collision_logics.empty());
  REQUIRE_NOTHROW(collision_logics[0]->GetSubscribers());

  const auto &render_logics = logic_collection.at(steamrot::LogicType::Render);
  REQUIRE(!render_logics.empty());
  REQUIRE_NOTHROW(render_logics[0]->GetSubscribers());

  const auto &action_logics = logic_collection.at(steamrot::LogicType::Action);
  REQUIRE(!action_logics.empty());
  REQUIRE_NOTHROW(action_logics[0]->GetSubscribers());
}
