/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for LogicFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "LogicFactory.h"
#include "CraftingRenderLogic.h"
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "UIRenderLogic.h"
#include "logic_helpers.h"
#include "scene_types_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LogicFactory constructed without errors", "[LogicFactory]") {

  // create a LogicContext with mock dependencies
  steamrot::LogicContext logic_context = steamrot::tests::CreateLogicContext();
  // create a LogicFactory instance
  steamrot::LogicFactory logic_factory(steamrot::SceneType::SceneType_TEST,
                                       logic_context);

  REQUIRE_NOTHROW(logic_factory);
}

TEST_CASE("LogicFactory creates the correct Logic instances with a test Scene",
          "[LogicFactory]") {

  // create a LogicContext with mock dependencies
  steamrot::LogicContext logic_context = steamrot::tests::CreateLogicContext();
  // create a LogicFactory instance
  steamrot::LogicFactory logic_factory(steamrot::SceneType::SceneType_TEST,
                                       logic_context);

  auto logic_map_result = logic_factory.CreateLogicMap();
  if (!logic_map_result.has_value()) {
    FAIL("LogicFactory failed to create logic map: " +
         logic_map_result.error().message);
  }

  steamrot::LogicCollection &logic_collection = logic_map_result.value();

  REQUIRE_NOTHROW(logic_collection.at(steamrot::LogicType::Collision));
  REQUIRE_NOTHROW(logic_collection.at(steamrot::LogicType::Action));
  REQUIRE_NOTHROW(logic_collection.at(steamrot::LogicType::Render));

  // evaluate collision logics
  steamrot::LogicVector &collision_logics =
      logic_collection.at(steamrot::LogicType::Collision);
  REQUIRE(collision_logics.size() == 1);
  REQUIRE(
      dynamic_cast<steamrot::UICollisionLogic *>(collision_logics[0].get()));

  // evaluate action logics
  steamrot::LogicVector &action_logics =
      logic_collection.at(steamrot::LogicType::Action);
  REQUIRE(action_logics.size() == 1);
  REQUIRE(dynamic_cast<steamrot::UIActionLogic *>(action_logics[0].get()));

  // evaluate render logics
  steamrot::LogicVector &render_logics =
      logic_collection.at(steamrot::LogicType::Render);
  REQUIRE(render_logics.size() == 1);
  REQUIRE(dynamic_cast<steamrot::UIRenderLogic *>(render_logics[0].get()));
}

TEST_CASE("LogicFactory creates correct Logic instances for TitleScene",
          "[LogicFactory]") {
  // create a LogicContext with mock dependencies
  steamrot::LogicContext logic_context = steamrot::tests::CreateLogicContext();
  // create a LogicFactory instance
  steamrot::LogicFactory logic_factory(steamrot::SceneType::SceneType_TITLE,
                                       logic_context);

  auto logic_map_result = logic_factory.CreateLogicMap();
  if (!logic_map_result.has_value()) {
    FAIL("LogicFactory failed to create logic map: " +
         logic_map_result.error().message);
  }

  steamrot::LogicCollection &logic_collection = logic_map_result.value();

  REQUIRE_NOTHROW(logic_collection.at(steamrot::LogicType::Collision));
  REQUIRE_NOTHROW(logic_collection.at(steamrot::LogicType::Action));
  REQUIRE_NOTHROW(logic_collection.at(steamrot::LogicType::Render));

  // evaluate collision logics
  steamrot::LogicVector &collision_logics =
      logic_collection.at(steamrot::LogicType::Collision);
  REQUIRE(collision_logics.size() == 1);
  REQUIRE(
      dynamic_cast<steamrot::UICollisionLogic *>(collision_logics[0].get()));

  // evaluate action logics
  steamrot::LogicVector &action_logics =
      logic_collection.at(steamrot::LogicType::Action);
  REQUIRE(action_logics.size() == 1);
  REQUIRE(dynamic_cast<steamrot::UIActionLogic *>(action_logics[0].get()));

  // evaluate render logics
  steamrot::LogicVector &render_logics =
      logic_collection.at(steamrot::LogicType::Render);
  REQUIRE(render_logics.size() == 1);
  REQUIRE(dynamic_cast<steamrot::UIRenderLogic *>(render_logics[0].get()));
}

TEST_CASE("LogicFactory creates correct Logic instances for CraftingScene",
          "[LogicFactory]") {
  // create a LogicContext with mock dependencies
  steamrot::LogicContext logic_context = steamrot::tests::CreateLogicContext();
  // create a LogicFactory instance
  steamrot::LogicFactory logic_factory(steamrot::SceneType::SceneType_CRAFTING,
                                       logic_context);

  auto logic_map_result = logic_factory.CreateLogicMap();
  if (!logic_map_result.has_value()) {
    FAIL("LogicFactory failed to create logic map: " +
         logic_map_result.error().message);
  }

  steamrot::LogicCollection &logic_collection = logic_map_result.value();

  REQUIRE_NOTHROW(logic_collection.at(steamrot::LogicType::Collision));
  REQUIRE_NOTHROW(logic_collection.at(steamrot::LogicType::Action));
  REQUIRE_NOTHROW(logic_collection.at(steamrot::LogicType::Render));

  // evaluate collision logics
  steamrot::LogicVector &collision_logics =
      logic_collection.at(steamrot::LogicType::Collision);
  REQUIRE(collision_logics.size() == 1);
  REQUIRE(
      dynamic_cast<steamrot::UICollisionLogic *>(collision_logics[0].get()));

  // evaluate action logics
  steamrot::LogicVector &action_logics =
      logic_collection.at(steamrot::LogicType::Action);
  REQUIRE(action_logics.size() == 1);
  REQUIRE(dynamic_cast<steamrot::UIActionLogic *>(action_logics[0].get()));

  // evaluate render logics
  steamrot::LogicVector &render_logics =
      logic_collection.at(steamrot::LogicType::Render);
  REQUIRE(render_logics.size() == 2);
  REQUIRE(
      dynamic_cast<steamrot::CraftingRenderLogic *>(render_logics[0].get()));
  REQUIRE(dynamic_cast<steamrot::UIRenderLogic *>(render_logics[1].get()));
}
