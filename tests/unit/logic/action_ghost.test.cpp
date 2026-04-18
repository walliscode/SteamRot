/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the action_ghost free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_ghost.h"
#include "EventPayload.h"
#include "GrimoireMachina.h"
#include "MrGhost.h"
#include "Subscriber.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

/////////////////////////////////////////////////
// SelectGhostItem
/////////////////////////////////////////////////

TEST_CASE("SelectGhostItem sets a FragmentInstance on MrGhost from a "
          "FragmentTag",
          "[unit][action_ghost]") {
  steamrot::tests::TestFixture fixture;
  steamrot::AssetManager &asset_manager =
      fixture.GetSceneContext().asset_manager;
  auto set_up = asset_manager.SetUpEmptyGrimoireMachina();
  REQUIRE(set_up.has_value());
  auto *grimoire = asset_manager.GetGrimoireMachina().value();
  grimoire->m_all_fragments.insert({"rock", steamrot::Fragment{}});

  steamrot::MrGhost mr_ghost;
  steamrot::GhostSelection selection = steamrot::FragmentTag{"rock"};
  steamrot::logic::action::ghost::SelectGhostItem(mr_ghost, selection,
                                                  asset_manager);

  REQUIRE(
      std::holds_alternative<steamrot::FragmentInstance>(mr_ghost.m_instance));
  REQUIRE(std::get<steamrot::FragmentInstance>(mr_ghost.m_instance).fragment ==
          &grimoire->m_all_fragments["rock"]);
}

TEST_CASE("SelectGhostItem sets a JointInstance on MrGhost from a JointTag",
          "[unit][action_ghost]") {
  steamrot::tests::TestFixture fixture;
  steamrot::AssetManager &asset_manager =
      fixture.GetSceneContext().asset_manager;
  auto set_up = asset_manager.SetUpEmptyGrimoireMachina();
  REQUIRE(set_up.has_value());
  auto *grimoire = asset_manager.GetGrimoireMachina().value();
  grimoire->m_all_joints.insert({"pivot", steamrot::Joint{}});

  steamrot::MrGhost mr_ghost;
  steamrot::GhostSelection selection = steamrot::JointTag{"pivot"};
  steamrot::logic::action::ghost::SelectGhostItem(mr_ghost, selection,
                                                  asset_manager);

  REQUIRE(
      std::holds_alternative<steamrot::JointInstance>(mr_ghost.m_instance));
  REQUIRE(std::get<steamrot::JointInstance>(mr_ghost.m_instance).joint ==
          &grimoire->m_all_joints["pivot"]);
}

TEST_CASE("SelectGhostItem overwrites an existing instance on MrGhost",
          "[unit][action_ghost]") {
  steamrot::tests::TestFixture fixture;
  steamrot::AssetManager &asset_manager =
      fixture.GetSceneContext().asset_manager;
  auto set_up = asset_manager.SetUpEmptyGrimoireMachina();
  REQUIRE(set_up.has_value());
  auto *grimoire = asset_manager.GetGrimoireMachina().value();
  grimoire->m_all_fragments.insert({"arm", steamrot::Fragment{}});
  grimoire->m_all_fragments.insert({"leg", steamrot::Fragment{}});

  steamrot::MrGhost mr_ghost;
  // First selection
  steamrot::logic::action::ghost::SelectGhostItem(
      mr_ghost, steamrot::FragmentTag{"arm"}, asset_manager);
  REQUIRE(std::get<steamrot::FragmentInstance>(mr_ghost.m_instance).fragment ==
          &grimoire->m_all_fragments["arm"]);

  // Overwrite with second selection
  steamrot::logic::action::ghost::SelectGhostItem(
      mr_ghost, steamrot::FragmentTag{"leg"}, asset_manager);
  REQUIRE(std::get<steamrot::FragmentInstance>(mr_ghost.m_instance).fragment ==
          &grimoire->m_all_fragments["leg"]);
}

TEST_CASE("SelectGhostItem leaves MrGhost unchanged when key is not found",
          "[unit][action_ghost]") {
  steamrot::tests::TestFixture fixture;
  steamrot::AssetManager &asset_manager =
      fixture.GetSceneContext().asset_manager;
  auto set_up = asset_manager.SetUpEmptyGrimoireMachina();
  REQUIRE(set_up.has_value());

  steamrot::MrGhost mr_ghost;
  steamrot::GhostSelection selection = steamrot::FragmentTag{"missing"};
  steamrot::logic::action::ghost::SelectGhostItem(mr_ghost, selection,
                                                  asset_manager);

  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_instance));
}

/////////////////////////////////////////////////
// ClearGhostSelection
/////////////////////////////////////////////////

TEST_CASE("ClearGhostSelection resets an active FragmentInstance",
          "[unit][action_ghost]") {
  steamrot::tests::TestFixture fixture;
  steamrot::AssetManager &asset_manager =
      fixture.GetSceneContext().asset_manager;
  auto set_up = asset_manager.SetUpEmptyGrimoireMachina();
  REQUIRE(set_up.has_value());
  auto *grimoire = asset_manager.GetGrimoireMachina().value();
  grimoire->m_all_fragments.insert({"rock", steamrot::Fragment{}});

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::FragmentInstance{&grimoire->m_all_fragments["rock"]};

  steamrot::logic::action::ghost::ClearGhostSelection(mr_ghost);

  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_instance));
}

TEST_CASE("ClearGhostSelection resets an active JointInstance",
          "[unit][action_ghost]") {
  steamrot::tests::TestFixture fixture;
  steamrot::AssetManager &asset_manager =
      fixture.GetSceneContext().asset_manager;
  auto set_up = asset_manager.SetUpEmptyGrimoireMachina();
  REQUIRE(set_up.has_value());
  auto *grimoire = asset_manager.GetGrimoireMachina().value();
  grimoire->m_all_joints.insert({"pivot", steamrot::Joint{}});

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::JointInstance{&grimoire->m_all_joints["pivot"]};

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

TEST_CASE("ProcessSubscriber – SELECT with FragmentTag resolves instance in "
          "MrGhost",
          "[unit][action_ghost]") {
  steamrot::tests::TestFixture fixture;
  steamrot::AssetManager &asset_manager =
      fixture.GetSceneContext().asset_manager;
  auto set_up = asset_manager.SetUpEmptyGrimoireMachina();
  REQUIRE(set_up.has_value());
  auto *grimoire = asset_manager.GetGrimoireMachina().value();
  grimoire->m_all_fragments.insert({"iron", steamrot::Fragment{}});

  steamrot::MrGhost mr_ghost;
  steamrot::Subscriber subscriber;
  subscriber.captured_payload = steamrot::GhostPayload{
      steamrot::GhostPayload::GhostAction::SELECT,
      steamrot::FragmentTag{"iron"}};

  steamrot::logic::action::ghost::ProcessSubscriber(subscriber, mr_ghost,
                                                    asset_manager);

  REQUIRE(
      std::holds_alternative<steamrot::FragmentInstance>(mr_ghost.m_instance));
  REQUIRE(std::get<steamrot::FragmentInstance>(mr_ghost.m_instance).fragment ==
          &grimoire->m_all_fragments["iron"]);
}

TEST_CASE("ProcessSubscriber – SELECT with JointTag resolves instance in "
          "MrGhost",
          "[unit][action_ghost]") {
  steamrot::tests::TestFixture fixture;
  steamrot::AssetManager &asset_manager =
      fixture.GetSceneContext().asset_manager;
  auto set_up = asset_manager.SetUpEmptyGrimoireMachina();
  REQUIRE(set_up.has_value());
  auto *grimoire = asset_manager.GetGrimoireMachina().value();
  grimoire->m_all_joints.insert({"hinge", steamrot::Joint{}});

  steamrot::MrGhost mr_ghost;
  steamrot::Subscriber subscriber;
  subscriber.captured_payload =
      steamrot::GhostPayload{steamrot::GhostPayload::GhostAction::SELECT,
                             steamrot::JointTag{"hinge"}};

  steamrot::logic::action::ghost::ProcessSubscriber(subscriber, mr_ghost,
                                                    asset_manager);

  REQUIRE(
      std::holds_alternative<steamrot::JointInstance>(mr_ghost.m_instance));
  REQUIRE(std::get<steamrot::JointInstance>(mr_ghost.m_instance).joint ==
          &grimoire->m_all_joints["hinge"]);
}

TEST_CASE("ProcessSubscriber – CLEAR resets MrGhost instance",
          "[unit][action_ghost]") {
  steamrot::tests::TestFixture fixture;
  steamrot::AssetManager &asset_manager =
      fixture.GetSceneContext().asset_manager;
  auto set_up = asset_manager.SetUpEmptyGrimoireMachina();
  REQUIRE(set_up.has_value());
  auto *grimoire = asset_manager.GetGrimoireMachina().value();
  grimoire->m_all_fragments.insert({"rock", steamrot::Fragment{}});

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::FragmentInstance{&grimoire->m_all_fragments["rock"]};

  steamrot::Subscriber subscriber;
  subscriber.captured_payload = steamrot::GhostPayload{
      steamrot::GhostPayload::GhostAction::CLEAR, std::monostate{}};

  steamrot::logic::action::ghost::ProcessSubscriber(subscriber, mr_ghost,
                                                    asset_manager);

  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_instance));
}

TEST_CASE("ProcessSubscriber – no captured payload leaves MrGhost unchanged",
          "[unit][action_ghost]") {
  steamrot::tests::TestFixture fixture;
  steamrot::AssetManager &asset_manager =
      fixture.GetSceneContext().asset_manager;
  auto set_up = asset_manager.SetUpEmptyGrimoireMachina();
  REQUIRE(set_up.has_value());
  auto *grimoire = asset_manager.GetGrimoireMachina().value();
  grimoire->m_all_fragments.insert({"rock", steamrot::Fragment{}});

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::FragmentInstance{&grimoire->m_all_fragments["rock"]};

  steamrot::Subscriber subscriber;
  // captured_payload left as std::nullopt

  steamrot::logic::action::ghost::ProcessSubscriber(subscriber, mr_ghost,
                                                    asset_manager);

  REQUIRE(
      std::holds_alternative<steamrot::FragmentInstance>(mr_ghost.m_instance));
  REQUIRE(std::get<steamrot::FragmentInstance>(mr_ghost.m_instance).fragment ==
          &grimoire->m_all_fragments["rock"]);
}

TEST_CASE(
    "ProcessSubscriber – non-GhostPayload captured payload leaves MrGhost "
    "unchanged",
    "[unit][action_ghost]") {
  steamrot::tests::TestFixture fixture;
  steamrot::AssetManager &asset_manager =
      fixture.GetSceneContext().asset_manager;
  auto set_up = asset_manager.SetUpEmptyGrimoireMachina();
  REQUIRE(set_up.has_value());
  auto *grimoire = asset_manager.GetGrimoireMachina().value();
  grimoire->m_all_fragments.insert({"rock", steamrot::Fragment{}});

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::FragmentInstance{&grimoire->m_all_fragments["rock"]};

  steamrot::Subscriber subscriber;
  subscriber.captured_payload = std::monostate{};

  steamrot::logic::action::ghost::ProcessSubscriber(subscriber, mr_ghost,
                                                    asset_manager);

  REQUIRE(
      std::holds_alternative<steamrot::FragmentInstance>(mr_ghost.m_instance));
}

/////////////////////////////////////////////////
// ProcessSubscribers
/////////////////////////////////////////////////

TEST_CASE("ProcessSubscribers – inactive subscriber is skipped",
          "[unit][action_ghost]") {
  steamrot::tests::TestFixture fixture;
  steamrot::AssetManager &asset_manager =
      fixture.GetSceneContext().asset_manager;
  auto set_up = asset_manager.SetUpEmptyGrimoireMachina();
  REQUIRE(set_up.has_value());
  auto *grimoire = asset_manager.GetGrimoireMachina().value();
  grimoire->m_all_fragments.insert({"rock", steamrot::Fragment{}});

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::FragmentInstance{&grimoire->m_all_fragments["rock"]};

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = false;
  subscriber->captured_payload = steamrot::GhostPayload{
      steamrot::GhostPayload::GhostAction::CLEAR, std::monostate{}};

  steamrot::logic::action::ghost::ProcessSubscribers({subscriber}, mr_ghost,
                                                     asset_manager);

  REQUIRE(
      std::holds_alternative<steamrot::FragmentInstance>(mr_ghost.m_instance));
}

TEST_CASE("ProcessSubscribers – active SELECT subscriber resolves instance",
          "[unit][action_ghost]") {
  steamrot::tests::TestFixture fixture;
  steamrot::AssetManager &asset_manager =
      fixture.GetSceneContext().asset_manager;
  auto set_up = asset_manager.SetUpEmptyGrimoireMachina();
  REQUIRE(set_up.has_value());
  auto *grimoire = asset_manager.GetGrimoireMachina().value();
  grimoire->m_all_fragments.insert({"copper", steamrot::Fragment{}});

  steamrot::MrGhost mr_ghost;

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->captured_payload = steamrot::GhostPayload{
      steamrot::GhostPayload::GhostAction::SELECT,
      steamrot::FragmentTag{"copper"}};

  steamrot::logic::action::ghost::ProcessSubscribers({subscriber}, mr_ghost,
                                                     asset_manager);

  REQUIRE(
      std::holds_alternative<steamrot::FragmentInstance>(mr_ghost.m_instance));
  REQUIRE(std::get<steamrot::FragmentInstance>(mr_ghost.m_instance).fragment ==
          &grimoire->m_all_fragments["copper"]);
}

TEST_CASE("ProcessSubscribers – active CLEAR subscriber clears MrGhost",
          "[unit][action_ghost]") {
  steamrot::tests::TestFixture fixture;
  steamrot::AssetManager &asset_manager =
      fixture.GetSceneContext().asset_manager;
  auto set_up = asset_manager.SetUpEmptyGrimoireMachina();
  REQUIRE(set_up.has_value());
  auto *grimoire = asset_manager.GetGrimoireMachina().value();
  grimoire->m_all_fragments.insert({"rock", steamrot::Fragment{}});

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::FragmentInstance{&grimoire->m_all_fragments["rock"]};

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->captured_payload = steamrot::GhostPayload{
      steamrot::GhostPayload::GhostAction::CLEAR, std::monostate{}};

  steamrot::logic::action::ghost::ProcessSubscribers({subscriber}, mr_ghost,
                                                     asset_manager);

  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_instance));
}
