/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for Logic base class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Subscriber.h"
#include "TestFixture.h"
#include "UIRenderLogic.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

TEST_CASE("Logic::GetSubscribers returns empty vector initially",
          "[unit][Logic]") {

  steamrot::tests::TestFixture test_context;

  // Create a concrete Logic instance (using UIRenderLogic as an example)
  steamrot::UIRenderLogic logic(test_context.GetSceneContext());

  // Verify subscribers vector is empty initially
  const auto &subscribers = logic.GetSubscribers();
  REQUIRE(subscribers.empty());
}

TEST_CASE("Logic::AddSubscriber adds a subscriber to the vector",
          "[unit][Logic]") {

  steamrot::tests::TestFixture test_context;

  // Create a concrete Logic instance
  steamrot::UIRenderLogic logic(test_context.GetSceneContext());

  // Create a subscriber
  auto subscriber = std::make_shared<steamrot::Subscriber>(
      steamrot::EventType::EventType_EVENT_USER_INPUT);

  // Add subscriber to logic
  logic.AddSubscriber(subscriber);

  // Verify subscriber was added
  const auto &subscribers = logic.GetSubscribers();
  REQUIRE(subscribers.size() == 1);
  REQUIRE(subscribers[0] == subscriber);
}

TEST_CASE("Logic::AddSubscriber can add multiple subscribers",
          "[unit][Logic]") {

  steamrot::tests::TestFixture test_context;

  // Create a concrete Logic instance
  steamrot::UIRenderLogic logic(test_context.GetSceneContext());

  // Create multiple subscribers
  auto subscriber1 = std::make_shared<steamrot::Subscriber>(
      steamrot::EventType::EventType_EVENT_USER_INPUT);
  auto subscriber2 = std::make_shared<steamrot::Subscriber>(
      steamrot::EventType::EventType_EVENT_CHANGE_SCENE);
  auto subscriber3 = std::make_shared<steamrot::Subscriber>(
      steamrot::EventType::EventType_EVENT_QUIT_GAME);

  // Add subscribers to logic
  logic.AddSubscriber(subscriber1);
  logic.AddSubscriber(subscriber2);
  logic.AddSubscriber(subscriber3);

  // Verify all subscribers were added
  const auto &subscribers = logic.GetSubscribers();
  REQUIRE(subscribers.size() == 3);
  REQUIRE(subscribers[0] == subscriber1);
  REQUIRE(subscribers[1] == subscriber2);
  REQUIRE(subscribers[2] == subscriber3);
}

TEST_CASE("Logic::RunLogic can be called on Logic with subscribers",
          "[unit][Logic]") {

  steamrot::tests::TestFixture test_context;

  // Create a concrete Logic instance
  steamrot::UIRenderLogic logic(test_context.GetSceneContext());

  // Create and add a subscriber
  auto subscriber = std::make_shared<steamrot::Subscriber>(
      steamrot::EventType::EventType_EVENT_USER_INPUT);
  logic.AddSubscriber(subscriber);

  // Verify RunLogic doesn't throw with subscribers present
  REQUIRE_NOTHROW(logic.RunLogic());
}
