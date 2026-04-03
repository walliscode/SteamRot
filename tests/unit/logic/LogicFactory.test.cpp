/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for LogicFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "LogicFactory.h"
#include "EventPayload.h"
#include "GhostItemActionLogic.h"
#include "GhostItemRenderLogic.h"
#include "GrimoireMachinaActionLogic.h"
#include "GrimoireMachinaCollisionLogic.h"
#include "GrimoireMachinaPositioningLogic.h"
#include "GrimoireMachinaRenderLogic.h"
#include "TestFixture.h"
#include "TestLogic.h"
#include "UIActionLogic.h"
#include "UICollisionLogic.h"
#include "UIRenderLogic.h"
#include "UIStateLogic.h"
#include <catch2/catch_test_macros.hpp>
#include <expected>

TEST_CASE("LogicFactory constructed without errors", "[unit][LogicFactory]") {

  // create a Testcontext to provide mock dependencies
  steamrot::tests::TestFixture test_context;

  // create a LogicFactory instance
  steamrot::logic::LogicFactory logic_factory(test_context.GetSceneContext());

  REQUIRE_NOTHROW(logic_factory);
}

TEST_CASE("LogicFactory::ProvideLogicCollection returns unexpected for unknown "
          "SceneType",
          "[unit][LogicFactory]") {
  // create a Testcontext to provide mock dependencies
  steamrot::tests::TestFixture test_context;
  // create a LogicFactory instance
  steamrot::logic::LogicFactory logic_factory(test_context.GetSceneContext());
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
  steamrot::logic::LogicFactory logic_factory(test_context.GetSceneContext());

  // call ProvideLogicCollection with SceneType_TITLE
  auto result =
      logic_factory.ProvideLogicCollection(steamrot::SceneType::TITLE);

  if (!result.has_value()) {
    FAIL("ProvideLogicCollection returned unexpected: " +
         result.error().message);
  }

  // assign the returned LogicCollection
  const auto &logic_collection = result.value();

  ///// CHECKING COLLISION LOGICS /////
  auto collision_it = logic_collection.find(steamrot::LogicGrouping::Collision);
  if (collision_it == logic_collection.end()) {
    FAIL("LogicCollection does not contain Collision LogicType");
  }
  const auto &collision_logics = collision_it->second;

  REQUIRE(collision_logics.size() == 1);
  REQUIRE(dynamic_cast<steamrot::logic::UICollisionLogic *>(
      collision_logics[0].get()));

  ///// CHECKING ACTION LOGICS /////
  auto action_it = logic_collection.find(steamrot::LogicGrouping::Action);
  if (action_it == logic_collection.end()) {
    FAIL("LogicCollection does not contain Action LogicType");
  }
  const auto &action_logics = action_it->second;
  REQUIRE(action_logics.size() == 2);
  REQUIRE(
      dynamic_cast<steamrot::logic::UIActionLogic *>(action_logics[0].get()));
  REQUIRE(
      dynamic_cast<steamrot::logic::UIStateLogic *>(action_logics[1].get()));

  ///// CHECKING RENDER LOGICS /////
  auto render_it = logic_collection.find(steamrot::LogicGrouping::Render);
  if (render_it == logic_collection.end()) {
    FAIL("LogicCollection does not contain Render LogicType");
  }
  const auto &render_logics = render_it->second;
  REQUIRE(render_logics.size() == 1); // No render logics added yet
  REQUIRE(dynamic_cast<steamrot::logic::UIRenderLogic *>(
      render_logics[0].get())); // Placeholder check
}

TEST_CASE("LogicFactory::ProvideLogicCollection returns valid LogicCollection "
          "for SceneType_CRAFTING",
          "[unit][LogicFactory]") {
  // create a Testcontext to provide mock dependencies
  steamrot::tests::TestFixture test_context;
  // create a LogicFactory instance
  steamrot::logic::LogicFactory logic_factory(test_context.GetSceneContext());
  // call ProvideLogicCollection with SceneType_CRAFTING
  auto result =
      logic_factory.ProvideLogicCollection(steamrot::SceneType::CRAFTING);
  if (!result.has_value()) {
    FAIL("ProvideLogicCollection returned unexpected: " +
         result.error().message);
  }
  // assign the returned LogicCollection
  const auto &logic_collection = result.value();

  ///// CHECKING COLLISION LOGICS /////
  auto collision_it = logic_collection.find(steamrot::LogicGrouping::Collision);
  if (collision_it == logic_collection.end()) {
    FAIL("LogicCollection does not contain Collision LogicType");
  }
  const auto &collision_logics = collision_it->second;
  REQUIRE(collision_logics.size() == 2);
  REQUIRE(dynamic_cast<steamrot::logic::UICollisionLogic *>(
      collision_logics[0].get()));
  REQUIRE(dynamic_cast<steamrot::logic::GrimoireMachinaCollisionLogic *>(
      collision_logics[1].get()));

  ///// CHECKING ACTION LOGICS /////
  auto action_it = logic_collection.find(steamrot::LogicGrouping::Action);
  if (action_it == logic_collection.end()) {
    FAIL("LogicCollection does not contain Action LogicType");
  }
  const auto &action_logics = action_it->second;
  REQUIRE(action_logics.size() == 4);
  REQUIRE(
      dynamic_cast<steamrot::logic::UIActionLogic *>(action_logics[0].get()));
  REQUIRE(
      dynamic_cast<steamrot::logic::UIStateLogic *>(action_logics[1].get()));
  REQUIRE(dynamic_cast<steamrot::logic::GrimoireMachinaActionLogic *>(
      action_logics[2].get()));
  REQUIRE(dynamic_cast<steamrot::logic::GhostItemActionLogic *>(
      action_logics[3].get()));

  ///// CHECKING RENDER LOGICS /////
  auto render_it = logic_collection.find(steamrot::LogicGrouping::Render);
  if (render_it == logic_collection.end()) {
    FAIL("LogicCollection does not contain Render LogicType");
  }
  const auto &render_logics = render_it->second;
  REQUIRE(render_logics.size() == 3);
  REQUIRE(
      dynamic_cast<steamrot::logic::UIRenderLogic *>(render_logics[0].get()));
  REQUIRE(dynamic_cast<steamrot::logic::GrimoireMachinaRenderLogic *>(
      render_logics[1].get()));
  REQUIRE(dynamic_cast<steamrot::logic::GhostItemRenderLogic *>(
      render_logics[2].get()));

  ///// CHECKING MOVEMENT LOGICS /////
  auto movement_it = logic_collection.find(steamrot::LogicGrouping::Movement);
  if (movement_it == logic_collection.end()) {
    FAIL("LogicCollection does not contain Movement LogicType");
  }
  const auto &movement_logics = movement_it->second;
  REQUIRE(movement_logics.size() == 1);
  REQUIRE(dynamic_cast<steamrot::logic::GrimoireMachinaPositioningLogic *>(
      movement_logics[0].get()));
}

TEST_CASE("LogicFactory::ConfigureLogicObject returns unexpected if LogicType "
          "not set on Logic object",
          "[unit][LogicFactory]") {
  // create a Testcontext to provide mock dependencies
  steamrot::tests::TestFixture test_context;
  // create a LogicFactory instance
  steamrot::logic::LogicFactory logic_factory(test_context.GetSceneContext());
  // create a dummy Logic object with LogicType::None
  class DummyLogic : public steamrot::logic::Logic {
  public:
    DummyLogic(const steamrot::SceneContext &scene_context)
        : steamrot::logic::Logic(scene_context) {}

    void ProcessLogic() override {
      // do nothing
    }

    steamrot::LogicType GetLogicType() const override {
      return steamrot::LogicType::None; // Return None to trigger the error
                                        // case
    }
  };
  DummyLogic dummy_logic(test_context.GetSceneContext());
  // call ConfigureLogicObject with the dummy Logic
  auto result = logic_factory.ConfigureLogicObject(dummy_logic);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NotImplemented);
  REQUIRE(
      result.error().message ==
      "LogicFactory::ConfigureLogicObject: LogicType not set on Logic object");
}

TEST_CASE("LogicFactoru::ConfigureLogicObject correctly configures a TestLogic "
          "object",
          "[unit][LogicFactory]") {
  // create a Testcontext to provide mock dependencies
  steamrot::tests::TestFixture test_context;
  // create a LogicFactory instance
  steamrot::logic::LogicFactory logic_factory(test_context.GetSceneContext());
  // create a TestLogic object with LogicType::Test
  steamrot::tests::TestLogic test_logic(test_context.GetSceneContext());
  // call ConfigureLogicObject with the TestLogic object
  auto result = logic_factory.ConfigureLogicObject(test_logic);
  REQUIRE(result.has_value());
  // check that the TestLogic object has the expected Subscribers configured
  const auto &subscribers = test_logic.GetSubscribers();
  REQUIRE(subscribers.size() == 1);
  // test each Subscriber
  auto subscriber_one = subscribers[0];
  REQUIRE(subscriber_one->event_type == steamrot::EventType::USER_INPUT);
  REQUIRE(std::holds_alternative<steamrot::InputPayload>(
      subscriber_one->filter_payload));
  const auto &payload =
      std::get<steamrot::InputPayload>(subscriber_one->filter_payload);
  REQUIRE(payload.action == steamrot::InputPayload::InputAction::SELECT);
}
