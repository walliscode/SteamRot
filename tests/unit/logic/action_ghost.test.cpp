/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the action_ghost free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_ghost.h"
#include "EventPayload.h"
#include "MrGhost.h"
#include "Subscriber.h"
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

/////////////////////////////////////////////////
// ProcessSubscriber
/////////////////////////////////////////////////

TEST_CASE("ProcessSubscriber – SELECT with FragmentTag updates MrGhost",
          "[unit][action_ghost]") {
  steamrot::MrGhost mr_ghost;

  steamrot::Subscriber subscriber;
  subscriber.captured_payload =
      steamrot::GhostPayload{steamrot::GhostPayload::GhostAction::SELECT,
                             steamrot::FragmentTag{"granite"}};

  steamrot::logic::action::ghost::ProcessSubscriber(subscriber, mr_ghost);

  REQUIRE(std::holds_alternative<steamrot::FragmentTag>(mr_ghost.m_selection));
  REQUIRE(std::get<steamrot::FragmentTag>(mr_ghost.m_selection).key ==
          "granite");
}

TEST_CASE("ProcessSubscriber – SELECT with JointTag updates MrGhost",
          "[unit][action_ghost]") {
  steamrot::MrGhost mr_ghost;

  steamrot::Subscriber subscriber;
  subscriber.captured_payload = steamrot::GhostPayload{
      steamrot::GhostPayload::GhostAction::SELECT, steamrot::JointTag{"hinge"}};

  steamrot::logic::action::ghost::ProcessSubscriber(subscriber, mr_ghost);

  REQUIRE(std::holds_alternative<steamrot::JointTag>(mr_ghost.m_selection));
  REQUIRE(std::get<steamrot::JointTag>(mr_ghost.m_selection).key == "hinge");
}

TEST_CASE("ProcessSubscriber – CLEAR resets MrGhost selection",
          "[unit][action_ghost]") {
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"granite"};

  steamrot::Subscriber subscriber;
  subscriber.captured_payload = steamrot::GhostPayload{
      steamrot::GhostPayload::GhostAction::CLEAR, std::monostate{}};

  steamrot::logic::action::ghost::ProcessSubscriber(subscriber, mr_ghost);

  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_selection));
}

TEST_CASE("ProcessSubscriber – no captured payload leaves MrGhost unchanged",
          "[unit][action_ghost]") {
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"granite"};

  steamrot::Subscriber subscriber;
  // captured_payload left as std::nullopt

  steamrot::logic::action::ghost::ProcessSubscriber(subscriber, mr_ghost);

  REQUIRE(std::holds_alternative<steamrot::FragmentTag>(mr_ghost.m_selection));
  REQUIRE(std::get<steamrot::FragmentTag>(mr_ghost.m_selection).key ==
          "granite");
}

TEST_CASE(
    "ProcessSubscriber – non-GhostPayload captured payload leaves MrGhost "
    "unchanged",
    "[unit][action_ghost]") {
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"granite"};

  steamrot::Subscriber subscriber;
  subscriber.captured_payload = std::monostate{};

  steamrot::logic::action::ghost::ProcessSubscriber(subscriber, mr_ghost);

  REQUIRE(std::holds_alternative<steamrot::FragmentTag>(mr_ghost.m_selection));
}

/////////////////////////////////////////////////
// ProcessSubscribers
/////////////////////////////////////////////////

TEST_CASE("ProcessSubscribers – inactive subscriber is skipped",
          "[unit][action_ghost]") {
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"stone"};

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = false;
  subscriber->captured_payload = steamrot::GhostPayload{
      steamrot::GhostPayload::GhostAction::CLEAR, std::monostate{}};

  steamrot::logic::action::ghost::ProcessSubscribers({subscriber}, mr_ghost);

  REQUIRE(std::holds_alternative<steamrot::FragmentTag>(mr_ghost.m_selection));
}

TEST_CASE("ProcessSubscribers – active SELECT subscriber updates MrGhost",
          "[unit][action_ghost]") {
  steamrot::MrGhost mr_ghost;

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->captured_payload =
      steamrot::GhostPayload{steamrot::GhostPayload::GhostAction::SELECT,
                             steamrot::FragmentTag{"granite"}};

  steamrot::logic::action::ghost::ProcessSubscribers({subscriber}, mr_ghost);

  REQUIRE(std::holds_alternative<steamrot::FragmentTag>(mr_ghost.m_selection));
  REQUIRE(std::get<steamrot::FragmentTag>(mr_ghost.m_selection).key ==
          "granite");
}

TEST_CASE("ProcessSubscribers – active CLEAR subscriber clears MrGhost",
          "[unit][action_ghost]") {
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"stone"};

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->captured_payload = steamrot::GhostPayload{
      steamrot::GhostPayload::GhostAction::CLEAR, std::monostate{}};

  steamrot::logic::action::ghost::ProcessSubscribers({subscriber}, mr_ghost);

  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_selection));
}
