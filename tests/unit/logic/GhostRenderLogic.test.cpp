/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the GhostRenderLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GhostRenderLogic.h"
#include "MrGhost.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GhostRenderLogic can be constructed without errors",
          "[unit][GhostRenderLogic]") {
  steamrot::tests::TestFixture fixture;
  REQUIRE_NOTHROW(
      steamrot::logic::GhostRenderLogic{fixture.GetSceneContext()});
}

TEST_CASE("GhostRenderLogic::GetLogicType returns GhostRender",
          "[unit][GhostRenderLogic]") {
  steamrot::tests::TestFixture fixture;
  steamrot::logic::GhostRenderLogic logic{fixture.GetSceneContext()};
  REQUIRE(logic.GetLogicType() == steamrot::LogicType::GhostRender);
}

TEST_CASE("GhostRenderLogic::RunLogic returns early without throwing when no "
          "GrimoireMachina is available",
          "[unit][GhostRenderLogic]") {
  steamrot::tests::TestFixture fixture;
  // No GrimoireMachina set up on the AssetManager
  steamrot::logic::GhostRenderLogic logic{fixture.GetSceneContext()};
  REQUIRE_NOTHROW(logic.RunLogic());
}

TEST_CASE("GhostRenderLogic::RunLogic does not throw when selection is "
          "monostate and GrimoireMachina is available",
          "[unit][GhostRenderLogic]") {
  steamrot::tests::TestFixture fixture;
  auto set_up_result =
      fixture.GetSceneContext().asset_manager.SetUpEmptyGrimoireMachina();
  if (!set_up_result.has_value()) {
    FAIL("Failed to set up GrimoireMachina: " + set_up_result.error().message);
  }

  REQUIRE(
      std::holds_alternative<std::monostate>(fixture.GetSceneContext().mr_ghost.m_selection));

  steamrot::logic::GhostRenderLogic logic{fixture.GetSceneContext()};
  REQUIRE_NOTHROW(logic.RunLogic());
}

TEST_CASE("GhostRenderLogic::RunLogic does not throw when a FragmentTag is "
          "selected",
          "[unit][GhostRenderLogic]") {
  steamrot::tests::TestFixture fixture;
  auto set_up_result =
      fixture.GetSceneContext().asset_manager.SetUpEmptyGrimoireMachina();
  if (!set_up_result.has_value()) {
    FAIL("Failed to set up GrimoireMachina: " + set_up_result.error().message);
  }

  fixture.GetSceneContext().mr_ghost.m_selection =
      steamrot::FragmentTag{"stone"};
  fixture.GetSceneContext().mr_ghost.m_position = {50.f, 50.f};

  steamrot::logic::GhostRenderLogic logic{fixture.GetSceneContext()};
  REQUIRE_NOTHROW(logic.RunLogic());
}

TEST_CASE("GhostRenderLogic::RunLogic does not throw when a JointTag is "
          "selected",
          "[unit][GhostRenderLogic]") {
  steamrot::tests::TestFixture fixture;
  auto set_up_result =
      fixture.GetSceneContext().asset_manager.SetUpEmptyGrimoireMachina();
  if (!set_up_result.has_value()) {
    FAIL("Failed to set up GrimoireMachina: " + set_up_result.error().message);
  }

  fixture.GetSceneContext().mr_ghost.m_selection = steamrot::JointTag{"hinge"};
  fixture.GetSceneContext().mr_ghost.m_position = {50.f, 50.f};

  steamrot::logic::GhostRenderLogic logic{fixture.GetSceneContext()};
  REQUIRE_NOTHROW(logic.RunLogic());
}
