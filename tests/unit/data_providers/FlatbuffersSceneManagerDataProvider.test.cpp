/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersSceneManagerDataProvider
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneManagerDataProvider.h"
#include "events_generated.h"
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

  REQUIRE(state.subscriptions.size() == 1);
  REQUIRE(state.subscriptions[0]->m_trigger_event_type ==
          steamrot::EventType_EVENT_CHANGE_SCENE);
}
