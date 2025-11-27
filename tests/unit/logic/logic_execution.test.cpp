/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for logic_execution free functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "logic_execution.h"
#include "LogicFactory.h"
#include "TestFixture.h"
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "UIRenderLogic.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

using namespace steamrot;
using namespace steamrot::logic::execution;

/////////////////////////////////////////////////
/// ExecuteLogicVector tests
/////////////////////////////////////////////////

TEST_CASE("ExecuteLogicVector handles empty vector", "[unit][logic_execution]") {
  LogicVector empty_vector;

  // Should not throw with empty vector
  REQUIRE_NOTHROW(ExecuteLogicVector(empty_vector));
}

TEST_CASE("ExecuteLogicVector executes all logics in vector",
          "[unit][logic_execution]") {

  PathProvider path_provider{EnvironmentType::Test};
  tests::TestFixture test_context;

  LogicVector logics;
  logics.push_back(
      std::make_unique<UIRenderLogic>(test_context.GetSceneContext()));
  logics.push_back(
      std::make_unique<UICollisionLogic>(test_context.GetSceneContext()));

  // Should execute without throwing
  REQUIRE_NOTHROW(ExecuteLogicVector(logics));
}

TEST_CASE("ExecuteLogicVector handles nullptr in vector gracefully",
          "[unit][logic_execution]") {

  PathProvider path_provider{EnvironmentType::Test};
  tests::TestFixture test_context;

  LogicVector logics;
  logics.push_back(
      std::make_unique<UIRenderLogic>(test_context.GetSceneContext()));
  logics.push_back(nullptr); // Nullptr entry
  logics.push_back(
      std::make_unique<UICollisionLogic>(test_context.GetSceneContext()));

  // Should handle nullptr gracefully without throwing
  REQUIRE_NOTHROW(ExecuteLogicVector(logics));
}

/////////////////////////////////////////////////
/// ExecuteLogicsByType tests
/////////////////////////////////////////////////

TEST_CASE("ExecuteLogicsByType handles empty collection",
          "[unit][logic_execution]") {
  LogicCollection empty_collection;

  // Should not throw with empty collection
  REQUIRE_NOTHROW(ExecuteLogicsByType(empty_collection, LogicType::Action));
  REQUIRE_NOTHROW(ExecuteLogicsByType(empty_collection, LogicType::Render));
  REQUIRE_NOTHROW(ExecuteLogicsByType(empty_collection, LogicType::Collision));
  REQUIRE_NOTHROW(ExecuteLogicsByType(empty_collection, LogicType::Movement));
}

TEST_CASE("ExecuteLogicsByType handles missing type in collection",
          "[unit][logic_execution]") {

  PathProvider path_provider{EnvironmentType::Test};
  tests::TestFixture test_context;

  LogicCollection collection;
  LogicVector render_logics;
  render_logics.push_back(
      std::make_unique<UIRenderLogic>(test_context.GetSceneContext()));
  collection[LogicType::Render] = std::move(render_logics);

  // Should not throw when requesting a type that doesn't exist
  REQUIRE_NOTHROW(ExecuteLogicsByType(collection, LogicType::Action));
  REQUIRE_NOTHROW(ExecuteLogicsByType(collection, LogicType::Movement));
}

TEST_CASE("ExecuteLogicsByType executes correct type",
          "[unit][logic_execution]") {

  PathProvider path_provider{EnvironmentType::Test};
  tests::TestFixture test_context;

  LogicCollection collection;

  LogicVector action_logics;
  action_logics.push_back(
      std::make_unique<UIActionLogic>(test_context.GetSceneContext()));
  collection[LogicType::Action] = std::move(action_logics);

  LogicVector render_logics;
  render_logics.push_back(
      std::make_unique<UIRenderLogic>(test_context.GetSceneContext()));
  collection[LogicType::Render] = std::move(render_logics);

  // Should execute without throwing
  REQUIRE_NOTHROW(ExecuteLogicsByType(collection, LogicType::Action));
  REQUIRE_NOTHROW(ExecuteLogicsByType(collection, LogicType::Render));
}

/////////////////////////////////////////////////
/// ExecuteSceneTick tests
/////////////////////////////////////////////////

TEST_CASE("ExecuteSceneTick handles empty collection",
          "[unit][logic_execution]") {
  LogicCollection empty_collection;

  // Should not throw with empty collection
  REQUIRE_NOTHROW(ExecuteSceneTick(empty_collection));
}

TEST_CASE("ExecuteSceneTick executes all logic types in order",
          "[unit][logic_execution]") {

  PathProvider path_provider{EnvironmentType::Test};
  tests::TestFixture test_context;

  LogicCollection collection;

  LogicVector action_logics;
  action_logics.push_back(
      std::make_unique<UIActionLogic>(test_context.GetSceneContext()));
  collection[LogicType::Action] = std::move(action_logics);

  LogicVector collision_logics;
  collision_logics.push_back(
      std::make_unique<UICollisionLogic>(test_context.GetSceneContext()));
  collection[LogicType::Collision] = std::move(collision_logics);

  LogicVector render_logics;
  render_logics.push_back(
      std::make_unique<UIRenderLogic>(test_context.GetSceneContext()));
  collection[LogicType::Render] = std::move(render_logics);

  // Should execute all logic types without throwing
  REQUIRE_NOTHROW(ExecuteSceneTick(collection));
}

/////////////////////////////////////////////////
/// Convenience function tests
/////////////////////////////////////////////////

TEST_CASE("ExecuteActionLogics executes action logics",
          "[unit][logic_execution]") {

  PathProvider path_provider{EnvironmentType::Test};
  tests::TestFixture test_context;

  LogicCollection collection;
  LogicVector action_logics;
  action_logics.push_back(
      std::make_unique<UIActionLogic>(test_context.GetSceneContext()));
  collection[LogicType::Action] = std::move(action_logics);

  REQUIRE_NOTHROW(ExecuteActionLogics(collection));
}

TEST_CASE("ExecuteMovementLogics handles empty collection",
          "[unit][logic_execution]") {
  LogicCollection empty_collection;

  // Movement logics are not common, so test with empty collection
  REQUIRE_NOTHROW(ExecuteMovementLogics(empty_collection));
}

TEST_CASE("ExecuteCollisionLogics executes collision logics",
          "[unit][logic_execution]") {

  PathProvider path_provider{EnvironmentType::Test};
  tests::TestFixture test_context;

  LogicCollection collection;
  LogicVector collision_logics;
  collision_logics.push_back(
      std::make_unique<UICollisionLogic>(test_context.GetSceneContext()));
  collection[LogicType::Collision] = std::move(collision_logics);

  REQUIRE_NOTHROW(ExecuteCollisionLogics(collection));
}

TEST_CASE("ExecuteRenderLogics executes render logics",
          "[unit][logic_execution]") {

  PathProvider path_provider{EnvironmentType::Test};
  tests::TestFixture test_context;

  LogicCollection collection;
  LogicVector render_logics;
  render_logics.push_back(
      std::make_unique<UIRenderLogic>(test_context.GetSceneContext()));
  collection[LogicType::Render] = std::move(render_logics);

  REQUIRE_NOTHROW(ExecuteRenderLogics(collection));
}
