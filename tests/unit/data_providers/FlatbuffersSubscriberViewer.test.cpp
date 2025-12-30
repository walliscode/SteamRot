/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersSubscriberViewer
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSubscriberViewer.h"
#include "FlatbuffersDataLoader.h"
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
