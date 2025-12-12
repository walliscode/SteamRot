/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersSubscriberViewer
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSubscriberViewer.h"
#include "FlatbuffersDataLoader.h"
#include "events_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersSubscriberViewer handles null subscriber data",
          "[unit][FlatbuffersSubscriberViewer]") {

  // Create viewer with null data
  steamrot::FlatbuffersSubscriberViewer viewer(nullptr);

  auto result = viewer.GetSubscribers();
  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  // Should return empty vector for null input
  const auto &subscribers = result.value();
  REQUIRE(subscribers.empty());
}

TEST_CASE("FlatbuffersSubscriberViewer::GetSubscribers returns valid "
          "subscribers from engine data",
          "[unit][FlatbuffersSubscriberViewer]") {

  // Load engine state data which contains subscribers
  steamrot::FlatbuffersDataLoader loader;
  auto engine_state_result = loader.ProvideEngineStateFbs();

  if (!engine_state_result.has_value()) {
    FAIL(engine_state_result.error().message);
  }

  const auto *engine_state = engine_state_result.value();
  REQUIRE(engine_state != nullptr);

  // Create viewer with engine state subscriptions
  steamrot::FlatbuffersSubscriberViewer viewer(engine_state->subscriptions());

  auto result = viewer.GetSubscribers();
  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &subscribers = result.value();
  // Subscribers should be a valid vector (may be empty)
  REQUIRE_NOTHROW(subscribers.size());

  // Verify all subscribers are valid shared pointers
  for (const auto &subscriber : subscribers) {
    REQUIRE(subscriber != nullptr);
  }
}

TEST_CASE("FlatbuffersSubscriberViewer::GetSubscribers returns valid "
          "subscribers from scene manager data",
          "[unit][FlatbuffersSubscriberViewer]") {

  // Load scene manager data which contains subscribers
  steamrot::FlatbuffersDataLoader loader;
  auto scene_manager_result = loader.ProvideSceneManagerData();

  if (!scene_manager_result.has_value()) {
    FAIL(scene_manager_result.error().message);
  }

  const auto *scene_manager_data = scene_manager_result.value();
  REQUIRE(scene_manager_data != nullptr);
  REQUIRE(scene_manager_data->state() != nullptr);

  // Create viewer with scene manager subscriptions
  steamrot::FlatbuffersSubscriberViewer viewer(
      scene_manager_data->state()->subscriptions());

  auto result = viewer.GetSubscribers();
  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &subscribers = result.value();
  // Subscribers should be a valid vector (may be empty)
  REQUIRE_NOTHROW(subscribers.size());

  // Verify all subscribers are valid shared pointers
  for (const auto &subscriber : subscribers) {
    REQUIRE(subscriber != nullptr);
  }
}

TEST_CASE("FlatbuffersSubscriberViewer skips entries with NONE event type",
          "[unit][FlatbuffersSubscriberViewer]") {

  // This test verifies the behavior mentioned in the implementation
  // where NONE event types are skipped
  steamrot::FlatbuffersDataLoader loader;
  auto engine_state_result = loader.ProvideEngineStateFbs();

  REQUIRE(engine_state_result.has_value());
  const auto *engine_state = engine_state_result.value();

  steamrot::FlatbuffersSubscriberViewer viewer(engine_state->subscriptions());

  // Should successfully return subscribers, skipping invalid ones
  auto result = viewer.GetSubscribers();
  REQUIRE(result.has_value());

  const auto &subscribers = result.value();
  // All returned subscribers should be valid (no NONE event types)
  for (const auto &subscriber : subscribers) {
    REQUIRE(subscriber != nullptr);
    REQUIRE(subscriber->m_trigger_event_type != steamrot::EventType_NONE);
  }
}
