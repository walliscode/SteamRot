/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for LogicFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "LogicFactory.h"
#include "CraftingRenderLogic.h"
#include "TestFixture.h"
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "UIRenderLogic.h"
#include "UIStateLogic.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LogicFactory constructed without errors", "[unit][LogicFactory]") {

  // create a Testcontext to provide mock dependencies
  steamrot::tests::TestFixture test_context;

  // create a LogicFactory instance
  steamrot::LogicFactory logic_factory(test_context.GetSceneContext());

  REQUIRE_NOTHROW(logic_factory);
}

TEST_CASE("LogicFactory::ProvideLogicCollection returns unexpected for unknown "
          "SceneType",
          "[unit][LogicFactory]") {
  // create a Testcontext to provide mock dependencies
  steamrot::tests::TestFixture test_context;
  // create a LogicFactory instance
  steamrot::LogicFactory logic_factory(test_context.GetSceneContext());
  // call ProvideLogicCollection with an invalid SceneType
  auto result = logic_factory.ProvideLogicCollection(
      static_cast<steamrot::SceneType>(9999));
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::SceneTypeNotFound);
}

TEST_CASE(
    "LogicFactory::ProvideLogicCollection returns valid LogicCollection for "
    "SceneType_TITLE",
    "[unit][LogicFactory]") {
  // create a Testcontext to provide mock dependencies
  steamrot::tests::TestFixture test_context;
  // create a LogicFactory instance
  steamrot::LogicFactory logic_factory(test_context.GetSceneContext());

  // call ProvideLogicCollection with SceneType_TITLE
  auto result = logic_factory.ProvideLogicCollection(
      steamrot::SceneType::TITLE);

  if (!result.has_value()) {
    FAIL("ProvideLogicCollection returned unexpected: " +
         result.error().message);
  }

  // assign the returned LogicCollection
  const auto &logic_collection = result.value();

  ///// CHECKING COLLISION LOGICS /////
  auto collision_it = logic_collection.find(steamrot::LogicType::Collision);
  if (collision_it == logic_collection.end()) {
    FAIL("LogicCollection does not contain Collision LogicType");
  }
  const auto &collision_logics = collision_it->second;

  REQUIRE(collision_logics.size() == 1);
  REQUIRE(
      dynamic_cast<steamrot::UICollisionLogic *>(collision_logics[0].get()));

  ///// CHECKING ACTION LOGICS /////
  auto action_it = logic_collection.find(steamrot::LogicType::Action);
  if (action_it == logic_collection.end()) {
    FAIL("LogicCollection does not contain Action LogicType");
  }
  const auto &action_logics = action_it->second;
  REQUIRE(action_logics.size() == 2);
  REQUIRE(dynamic_cast<steamrot::UIActionLogic *>(action_logics[0].get()));
  REQUIRE(dynamic_cast<steamrot::UIStateLogic *>(action_logics[1].get()));

  ///// CHECKING RENDER LOGICS /////
  auto render_it = logic_collection.find(steamrot::LogicType::Render);
  if (render_it == logic_collection.end()) {
    FAIL("LogicCollection does not contain Render LogicType");
  }
  const auto &render_logics = render_it->second;
  REQUIRE(render_logics.size() == 1); // No render logics added yet
  REQUIRE(dynamic_cast<steamrot::UIRenderLogic *>(
      render_logics[0].get())); // Placeholder check
}

TEST_CASE("LogicFactory::ProvideLogicCollection returns valid LogicCollection "
          "for SceneType_CRAFTING",
          "[unit][LogicFactory]") {
  // create a Testcontext to provide mock dependencies
  steamrot::tests::TestFixture test_context;
  // create a LogicFactory instance
  steamrot::LogicFactory logic_factory(test_context.GetSceneContext());
  // call ProvideLogicCollection with SceneType_CRAFTING
  auto result = logic_factory.ProvideLogicCollection(
      steamrot::SceneType::CRAFTING);
  if (!result.has_value()) {
    FAIL("ProvideLogicCollection returned unexpected: " +
         result.error().message);
  }
  // assign the returned LogicCollection
  const auto &logic_collection = result.value();

  ///// CHECKING COLLISION LOGICS /////
  auto collision_it = logic_collection.find(steamrot::LogicType::Collision);
  if (collision_it == logic_collection.end()) {
    FAIL("LogicCollection does not contain Collision LogicType");
  }
  const auto &collision_logics = collision_it->second;
  REQUIRE(collision_logics.size() == 1);
  REQUIRE(
      dynamic_cast<steamrot::UICollisionLogic *>(collision_logics[0].get()));

  ///// CHECKING ACTION LOGICS /////
  auto action_it = logic_collection.find(steamrot::LogicType::Action);
  if (action_it == logic_collection.end()) {
    FAIL("LogicCollection does not contain Action LogicType");
  }
  const auto &action_logics = action_it->second;
  REQUIRE(action_logics.size() == 2);
  REQUIRE(dynamic_cast<steamrot::UIActionLogic *>(action_logics[0].get()));
  REQUIRE(dynamic_cast<steamrot::UIStateLogic *>(action_logics[1].get()));

  ///// CHECKING RENDER LOGICS /////
  auto render_it = logic_collection.find(steamrot::LogicType::Render);
  if (render_it == logic_collection.end()) {
    FAIL("LogicCollection does not contain Render LogicType");
  }
  const auto &render_logics = render_it->second;
  REQUIRE(render_logics.size() == 2);
  REQUIRE(dynamic_cast<steamrot::UIRenderLogic *>(render_logics[0].get()));
  REQUIRE(
      dynamic_cast<steamrot::CraftingRenderLogic *>(render_logics[1].get()));
}
