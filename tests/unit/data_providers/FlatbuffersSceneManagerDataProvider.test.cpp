/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersSceneManagerDataProvider
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneManagerDataProvider.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersSceneManagerDataProvider is constructed correctly",
          "[unit][FlatbuffersSceneManagerDataProvider]") {

  steamrot::FlatbuffersSceneManagerDataProvider provider;
  REQUIRE_NOTHROW(provider);
}

TEST_CASE("FlatbuffersSceneManagerDataProvider::LoadSceneManagerState loads "
          "correctly",
          "[unit][FlatbuffersSceneManagerDataProvider]") {

  steamrot::FlatbuffersSceneManagerDataProvider provider;
  auto result = provider.LoadSceneManagerState();

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &state = result.value();
  // Subscriptions should be loaded (may be empty)
  REQUIRE_NOTHROW(state.subscriptions);
}

TEST_CASE("FlatbuffersSceneManagerDataProvider::LoadSceneManagerState loads "
          "subscriptions",
          "[unit][FlatbuffersSceneManagerDataProvider]") {

  steamrot::FlatbuffersSceneManagerDataProvider provider;
  auto result = provider.LoadSceneManagerState();

  REQUIRE(result.has_value());

  const auto &state = result.value();
  // Verify subscriptions are loaded - they should be a valid vector
  REQUIRE_NOTHROW(state.subscriptions.size());
}

TEST_CASE(
    "FlatbuffersSceneManagerDataProvider::GetSubscriberViewer returns viewer",
    "[unit][FlatbuffersSceneManagerDataProvider]") {

  steamrot::FlatbuffersSceneManagerDataProvider provider;
  auto result = provider.GetSubscriberViewer();

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &viewer = result.value();
  REQUIRE(viewer != nullptr);

  // Test that viewer can get subscribers
  auto subscribers_result = viewer->GetSubscribers();
  if (!subscribers_result.has_value()) {
    FAIL(subscribers_result.error().message);
  }

  // Subscribers should be a valid vector (may be empty)
  const auto &subscribers = subscribers_result.value();
  REQUIRE_NOTHROW(subscribers.size());
}

TEST_CASE("FlatbuffersSceneManagerDataProvider loads valid SceneManagerState "
          "structure",
          "[unit][FlatbuffersSceneManagerDataProvider]") {

  steamrot::FlatbuffersSceneManagerDataProvider provider;
  auto result = provider.LoadSceneManagerState();

  REQUIRE(result.has_value());

  const auto &state = result.value();
  // Verify the state has subscriptions field
  REQUIRE_NOTHROW(state.subscriptions);

  // Subscriptions should be a valid vector of shared pointers
  for (const auto &subscription : state.subscriptions) {
    REQUIRE(subscription != nullptr);
  }
}

TEST_CASE(
    "FlatbuffersSceneManagerDataProvider::GetSubscriberViewer returns "
    "functional viewer",
    "[unit][FlatbuffersSceneManagerDataProvider]") {

  steamrot::FlatbuffersSceneManagerDataProvider provider;
  auto viewer_result = provider.GetSubscriberViewer();

  REQUIRE(viewer_result.has_value());

  auto &viewer = viewer_result.value();
  REQUIRE(viewer != nullptr);

  // Get subscribers through viewer
  auto subscribers_result = viewer->GetSubscribers();
  REQUIRE(subscribers_result.has_value());

  const auto &subscribers = subscribers_result.value();
  // Verify all subscribers are valid
  for (const auto &subscriber : subscribers) {
    REQUIRE(subscriber != nullptr);
  }
}
