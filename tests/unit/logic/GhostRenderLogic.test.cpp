/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the GhostRenderLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Fragment.h"
#include "GhostRenderLogic.h"
#include "Joint.h"
#include "MachinaFormScaffold.h"
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

TEST_CASE("GhostRenderLogic::RunLogic does not throw when instance is monostate",
          "[unit][GhostRenderLogic]") {
  steamrot::tests::TestFixture fixture;

  REQUIRE(std::holds_alternative<std::monostate>(
      fixture.GetSceneContext().mr_ghost.m_instance));

  steamrot::logic::GhostRenderLogic logic{fixture.GetSceneContext()};
  REQUIRE_NOTHROW(logic.RunLogic());
}

TEST_CASE("GhostRenderLogic::RunLogic does not throw when a FragmentInstance "
          "is active",
          "[unit][GhostRenderLogic]") {
  steamrot::tests::TestFixture fixture;

  steamrot::Fragment fragment;
  fixture.GetSceneContext().mr_ghost.m_instance =
      steamrot::FragmentInstance{&fragment};
  fixture.GetSceneContext().mr_ghost.m_position = {50.f, 50.f};

  steamrot::logic::GhostRenderLogic logic{fixture.GetSceneContext()};
  REQUIRE_NOTHROW(logic.RunLogic());
}

TEST_CASE("GhostRenderLogic::RunLogic does not throw when a JointInstance "
          "is active",
          "[unit][GhostRenderLogic]") {
  steamrot::tests::TestFixture fixture;

  steamrot::Joint joint;
  fixture.GetSceneContext().mr_ghost.m_instance =
      steamrot::JointInstance{&joint};
  fixture.GetSceneContext().mr_ghost.m_position = {50.f, 50.f};

  steamrot::logic::GhostRenderLogic logic{fixture.GetSceneContext()};
  REQUIRE_NOTHROW(logic.RunLogic());
}
