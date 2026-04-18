/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the action_ghost free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Fragment.h"
#include "Joint.h"
#include "MachinaFormScaffold.h"
#include "action_ghost.h"
#include "EventPayload.h"
#include "MrGhost.h"
#include "Subscriber.h"
#include <catch2/catch_test_macros.hpp>

/////////////////////////////////////////////////
// SelectGhostItem
/////////////////////////////////////////////////

TEST_CASE("SelectGhostItem sets a FragmentInstance on MrGhost",
          "[unit][action_ghost]") {
  steamrot::MrGhost mr_ghost;
  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_instance));

  steamrot::Fragment fragment;
  steamrot::FragmentInstance instance{&fragment};
  steamrot::logic::action::ghost::SelectGhostItem(mr_ghost, instance);

  REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(mr_ghost.m_instance));
  REQUIRE(std::get<steamrot::FragmentInstance>(mr_ghost.m_instance).fragment ==
          &fragment);
}

TEST_CASE("SelectGhostItem sets a JointInstance on MrGhost",
          "[unit][action_ghost]") {
  steamrot::MrGhost mr_ghost;

  steamrot::Joint joint;
  steamrot::JointInstance instance{&joint};
  steamrot::logic::action::ghost::SelectGhostItem(mr_ghost, instance);

  REQUIRE(std::holds_alternative<steamrot::JointInstance>(mr_ghost.m_instance));
  REQUIRE(std::get<steamrot::JointInstance>(mr_ghost.m_instance).joint == &joint);
}

TEST_CASE("SelectGhostItem overwrites an existing instance on MrGhost",
          "[unit][action_ghost]") {
  steamrot::Fragment old_fragment;
  steamrot::Fragment new_fragment;
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::FragmentInstance{&old_fragment};

  steamrot::FragmentInstance new_instance{&new_fragment};
  steamrot::logic::action::ghost::SelectGhostItem(mr_ghost, new_instance);

  REQUIRE(std::get<steamrot::FragmentInstance>(mr_ghost.m_instance).fragment ==
          &new_fragment);
}

/////////////////////////////////////////////////
// ClearGhostSelection
/////////////////////////////////////////////////

TEST_CASE("ClearGhostSelection resets an active FragmentInstance",
          "[unit][action_ghost]") {
  steamrot::Fragment fragment;
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::FragmentInstance{&fragment};

  steamrot::logic::action::ghost::ClearGhostSelection(mr_ghost);

  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_instance));
}

TEST_CASE("ClearGhostSelection resets an active JointInstance",
          "[unit][action_ghost]") {
  steamrot::Joint joint;
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::JointInstance{&joint};

  steamrot::logic::action::ghost::ClearGhostSelection(mr_ghost);

  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_instance));
}

TEST_CASE("ClearGhostSelection is idempotent on an already-clear MrGhost",
          "[unit][action_ghost]") {
  steamrot::MrGhost mr_ghost;
  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_instance));

  steamrot::logic::action::ghost::ClearGhostSelection(mr_ghost);

  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_instance));
}

/////////////////////////////////////////////////
// ProcessSubscriber
/////////////////////////////////////////////////

TEST_CASE("ProcessSubscriber – SELECT with FragmentInstance updates MrGhost",
          "[unit][action_ghost]") {
  steamrot::Fragment fragment;
  steamrot::MrGhost mr_ghost;

  steamrot::Subscriber subscriber;
  subscriber.captured_payload =
      steamrot::GhostPayload{steamrot::GhostPayload::GhostAction::SELECT,
                             steamrot::FragmentInstance{&fragment}};

  steamrot::logic::action::ghost::ProcessSubscriber(subscriber, mr_ghost);

  REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(mr_ghost.m_instance));
  REQUIRE(std::get<steamrot::FragmentInstance>(mr_ghost.m_instance).fragment ==
          &fragment);
}

TEST_CASE("ProcessSubscriber – SELECT with JointInstance updates MrGhost",
          "[unit][action_ghost]") {
  steamrot::Joint joint;
  steamrot::MrGhost mr_ghost;

  steamrot::Subscriber subscriber;
  subscriber.captured_payload = steamrot::GhostPayload{
      steamrot::GhostPayload::GhostAction::SELECT,
      steamrot::JointInstance{&joint}};

  steamrot::logic::action::ghost::ProcessSubscriber(subscriber, mr_ghost);

  REQUIRE(std::holds_alternative<steamrot::JointInstance>(mr_ghost.m_instance));
  REQUIRE(std::get<steamrot::JointInstance>(mr_ghost.m_instance).joint == &joint);
}

TEST_CASE("ProcessSubscriber – CLEAR resets MrGhost instance",
          "[unit][action_ghost]") {
  steamrot::Fragment fragment;
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::FragmentInstance{&fragment};

  steamrot::Subscriber subscriber;
  subscriber.captured_payload = steamrot::GhostPayload{
      steamrot::GhostPayload::GhostAction::CLEAR, std::monostate{}};

  steamrot::logic::action::ghost::ProcessSubscriber(subscriber, mr_ghost);

  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_instance));
}

TEST_CASE("ProcessSubscriber – no captured payload leaves MrGhost unchanged",
          "[unit][action_ghost]") {
  steamrot::Fragment fragment;
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::FragmentInstance{&fragment};

  steamrot::Subscriber subscriber;
  // captured_payload left as std::nullopt

  steamrot::logic::action::ghost::ProcessSubscriber(subscriber, mr_ghost);

  REQUIRE(
      std::holds_alternative<steamrot::FragmentInstance>(mr_ghost.m_instance));
  REQUIRE(std::get<steamrot::FragmentInstance>(mr_ghost.m_instance).fragment ==
          &fragment);
}

TEST_CASE(
    "ProcessSubscriber – non-GhostPayload captured payload leaves MrGhost "
    "unchanged",
    "[unit][action_ghost]") {
  steamrot::Fragment fragment;
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::FragmentInstance{&fragment};

  steamrot::Subscriber subscriber;
  subscriber.captured_payload = std::monostate{};

  steamrot::logic::action::ghost::ProcessSubscriber(subscriber, mr_ghost);

  REQUIRE(
      std::holds_alternative<steamrot::FragmentInstance>(mr_ghost.m_instance));
}

/////////////////////////////////////////////////
// ProcessSubscribers
/////////////////////////////////////////////////

TEST_CASE("ProcessSubscribers – inactive subscriber is skipped",
          "[unit][action_ghost]") {
  steamrot::Fragment fragment;
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::FragmentInstance{&fragment};

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = false;
  subscriber->captured_payload = steamrot::GhostPayload{
      steamrot::GhostPayload::GhostAction::CLEAR, std::monostate{}};

  steamrot::logic::action::ghost::ProcessSubscribers({subscriber}, mr_ghost);

  REQUIRE(
      std::holds_alternative<steamrot::FragmentInstance>(mr_ghost.m_instance));
}

TEST_CASE("ProcessSubscribers – active SELECT subscriber updates MrGhost",
          "[unit][action_ghost]") {
  steamrot::Fragment fragment;
  steamrot::MrGhost mr_ghost;

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->captured_payload =
      steamrot::GhostPayload{steamrot::GhostPayload::GhostAction::SELECT,
                             steamrot::FragmentInstance{&fragment}};

  steamrot::logic::action::ghost::ProcessSubscribers({subscriber}, mr_ghost);

  REQUIRE(
      std::holds_alternative<steamrot::FragmentInstance>(mr_ghost.m_instance));
  REQUIRE(std::get<steamrot::FragmentInstance>(mr_ghost.m_instance).fragment ==
          &fragment);
}

TEST_CASE("ProcessSubscribers – active CLEAR subscriber clears MrGhost",
          "[unit][action_ghost]") {
  steamrot::Fragment fragment;
  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::FragmentInstance{&fragment};

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->captured_payload = steamrot::GhostPayload{
      steamrot::GhostPayload::GhostAction::CLEAR, std::monostate{}};

  steamrot::logic::action::ghost::ProcessSubscribers({subscriber}, mr_ghost);

  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_instance));
}
