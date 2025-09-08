/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for LogicFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "LogicFactory.h"
#include "TestContext.h"
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "logic_helpers.h"
#include "scene_types_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LogicFactory constructed without errors", "[LogicFactory]") {

  // create a Testcontext to provide mock dependencies

  steamrot::tests::TestContext test_context;
  // create a LogicFactory instance
  steamrot::LogicFactory logic_factory(
      steamrot::SceneType::SceneType_TEST,
      test_context.GetLogicContextForTestScene());

  REQUIRE_NOTHROW(logic_factory);
}

TEST_CASE("LogicFactory creates the correct Logic instances with a test Scene",
          "[LogicFactory]") {

  // create a LogicContext with mock dependencies
  steamrot::tests::TestContext test_context{
      steamrot::SceneType::SceneType_TEST};

  // create a LogicFactory instance
  steamrot::LogicFactory logic_factory(
      steamrot::SceneType::SceneType_TEST,
      test_context.GetLogicContextForTestScene());

  auto logic_map_result = logic_factory.CreateLogicMap();
  if (!logic_map_result.has_value()) {
    FAIL("LogicFactory failed to create logic map: " +
         logic_map_result.error().message);
  }

  steamrot::LogicCollection &logic_collection = logic_map_result.value();

  steamrot::tests::CheckStaticLogicCollections(
      logic_collection, steamrot::SceneType::SceneType_TEST);
}

TEST_CASE("LogicFactory creates correct Logic instances for TitleScene",
          "[LogicFactory]") {

  steamrot::tests::TestContext test_context{
      steamrot::SceneType::SceneType_TITLE};
  // create a LogicFactory instance
  steamrot::LogicFactory logic_factory(
      steamrot::SceneType::SceneType_TITLE,
      test_context.GetLogicContextForTitleScene());

  auto logic_map_result = logic_factory.CreateLogicMap();
  if (!logic_map_result.has_value()) {
    FAIL("LogicFactory failed to create logic map: " +
         logic_map_result.error().message);
  }

  steamrot::LogicCollection &logic_collection = logic_map_result.value();

  steamrot::tests::CheckStaticLogicCollections(
      logic_collection, steamrot::SceneType::SceneType_TITLE);
}
TEST_CASE("LogicFactory creates correct Logic instances for CraftingScene",
          "[LogicFactory]") {

  steamrot::tests::TestContext test_context{
      steamrot::SceneType::SceneType_CRAFTING};
  // create a LogicFactory instance
  steamrot::LogicFactory logic_factory(
      steamrot::SceneType::SceneType_CRAFTING,
      test_context.GetLogicContextForCraftingScene());

  auto logic_map_result = logic_factory.CreateLogicMap();
  if (!logic_map_result.has_value()) {
    FAIL("LogicFactory failed to create logic map: " +
         logic_map_result.error().message);
  }

  steamrot::LogicCollection &logic_collection = logic_map_result.value();

  steamrot::tests::CheckStaticLogicCollections(
      logic_collection, steamrot::SceneType::SceneType_CRAFTING);
}
