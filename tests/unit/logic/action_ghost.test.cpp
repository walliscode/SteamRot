/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the action_ghost free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_ghost.h"
#include "MrGhost.h"
#include <catch2/catch_test_macros.hpp>

/////////////////////////////////////////////////
// SelectGhostItem
/////////////////////////////////////////////////

TEST_CASE("SelectGhostItem sets a FragmentTag selection on MrGhost",
          "[unit][action_ghost]") {
  steamrot::MrGhost mr_ghost;
  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_selection));

  steamrot::FragmentTag tag{"stone"};
  steamrot::logic::action::ghost::SelectGhostItem(mr_ghost, tag);

  REQUIRE(std::holds_alternative<steamrot::FragmentTag>(mr_ghost.m_selection));
  REQUIRE(std::get<steamrot::FragmentTag>(mr_ghost.m_selection).key == "stone");
}

TEST_CASE("SelectGhostItem sets a JointTag selection on MrGhost",
          "[unit][action_ghost]") {
  steamrot::MrGhost mr_ghost;

  steamrot::JointTag tag{"hinge"};
  steamrot::logic::action::ghost::SelectGhostItem(mr_ghost, tag);

  REQUIRE(std::holds_alternative<steamrot::JointTag>(mr_ghost.m_selection));
  REQUIRE(std::get<steamrot::JointTag>(mr_ghost.m_selection).key == "hinge");
}

TEST_CASE("SelectGhostItem overwrites an existing selection on MrGhost",
          "[unit][action_ghost]") {
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"old"};

  steamrot::FragmentTag new_tag{"new"};
  steamrot::logic::action::ghost::SelectGhostItem(mr_ghost, new_tag);

  REQUIRE(std::get<steamrot::FragmentTag>(mr_ghost.m_selection).key == "new");
}

/////////////////////////////////////////////////
// ClearGhostSelection
/////////////////////////////////////////////////

TEST_CASE("ClearGhostSelection resets an active FragmentTag selection",
          "[unit][action_ghost]") {
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"stone"};

  steamrot::logic::action::ghost::ClearGhostSelection(mr_ghost);

  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_selection));
}

TEST_CASE("ClearGhostSelection resets an active JointTag selection",
          "[unit][action_ghost]") {
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"hinge"};

  steamrot::logic::action::ghost::ClearGhostSelection(mr_ghost);

  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_selection));
}

TEST_CASE("ClearGhostSelection is idempotent on an already-clear MrGhost",
          "[unit][action_ghost]") {
  steamrot::MrGhost mr_ghost;
  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_selection));

  steamrot::logic::action::ghost::ClearGhostSelection(mr_ghost);

  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_selection));
}
