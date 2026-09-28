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

namespace steamrot::tests {
using namespace steamrot::logic::action::ghost;

TEST_CASE("select_ghost tests") {

  TestFixture fixture;
  AssetManager &asset_manager = fixture.GetSceneContext().asset_manager;
  auto set_up = asset_manager.SetUpEmptyGrimoireMachina();
  REQUIRE(set_up.has_value());
  auto *grimoire = asset_manager.GetGrimoireMachina().value();

  SECTION("select_ghost sets a FragmentInstance on MrGhost from a "
          "FragmentTag",
          "[unit][action_ghost]") {
    FragmentTag fragment_tag{"rock"};
    Fragment fragment;
    fragment.name = fragment_tag.key;
    grimoire->m_all_fragments.insert({fragment_tag.key, fragment});
    REQUIRE(grimoire->m_all_fragments.find("rock") !=
            grimoire->m_all_fragments.end());

    MrGhost mr_ghost;
    GhostSelection selection = FragmentTag{"rock"};
    select_ghost_item(mr_ghost, selection, asset_manager);

    REQUIRE(std::holds_alternative<FragmentInstance>(mr_ghost.m_instance));
    REQUIRE(std::get<FragmentInstance>(mr_ghost.m_instance).GetPart().name ==
            "rock");
  }

  SECTION("select_ghost sets a JointInstance on MrGhost from a JointTag",
          "[unit][action_ghost]") {
    JointTag joint_tag{"pivot"};
    Joint joint;
    joint.name = joint_tag.key;
    grimoire->m_all_joints.insert({joint_tag.key, joint});

    MrGhost mr_ghost;
    GhostSelection selection = JointTag{"pivot"};
    select_ghost_item(mr_ghost, selection, asset_manager);

    REQUIRE(std::holds_alternative<JointInstance>(mr_ghost.m_instance));
    REQUIRE(std::get<JointInstance>(mr_ghost.m_instance).GetPart().name ==
            "pivot");
  }

  SECTION("select_ghost overwrites an existing instance on MrGhost",
          "[unit][action_ghost]") {
    FragmentTag fragment_tag1{"arm"};
    FragmentTag fragment_tag2{"leg"};
    Fragment fragment1;
    fragment1.name = fragment_tag1.key;
    Fragment fragment2;
    fragment2.name = fragment_tag2.key;
    grimoire->m_all_fragments.insert({fragment_tag1.key, fragment1});
    grimoire->m_all_fragments.insert({fragment_tag2.key, fragment2});

    MrGhost mr_ghost;
    // First selection
    select_ghost_item(mr_ghost, FragmentTag{"arm"}, asset_manager);
    REQUIRE(std::get<FragmentInstance>(mr_ghost.m_instance).GetPart().name ==
            "arm");

    // Overwrite with second selection
    select_ghost_item(mr_ghost, FragmentTag{"leg"}, asset_manager);
    REQUIRE(std::get<FragmentInstance>(mr_ghost.m_instance).GetPart().name ==
            "leg");
  }

  SECTION("select_ghost leaves MrGhost unchanged when key is not found",
          "[unit][action_ghost]") {

    MrGhost mr_ghost;
    GhostSelection selection = FragmentTag{"missing"};
    select_ghost_item(mr_ghost, selection, asset_manager);

    REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_instance));
  }
}

TEST_CASE("clear_ghost_selection tests") {

  TestFixture fixture;
  steamrot::AssetManager &asset_manager =
      fixture.GetSceneContext().asset_manager;
  auto set_up = asset_manager.SetUpEmptyGrimoireMachina();
  REQUIRE(set_up.has_value());
  auto *grimoire = asset_manager.GetGrimoireMachina().value();

  SECTION("clear_ghost_selection resets an active FragmentInstance",
          "[unit][action_ghost]") {
    grimoire->m_all_fragments.insert({"rock", steamrot::Fragment{}});

    steamrot::MrGhost mr_ghost;
    mr_ghost.m_instance.emplace<steamrot::FragmentInstance>(
        0, grimoire->m_all_fragments["rock"]);

    steamrot::logic::action::ghost::clear_ghost_selection(mr_ghost);

    REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_instance));
  }

  SECTION("clear_ghost_selection resets an active JointInstance",
          "[unit][action_ghost]") {
    grimoire->m_all_joints.insert({"pivot", steamrot::Joint{}});

    steamrot::MrGhost mr_ghost;
    mr_ghost.m_instance.emplace<steamrot::JointInstance>(
        0, grimoire->m_all_joints["pivot"]);

    clear_ghost_selection(mr_ghost);

    REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_instance));
  }
}

TEST_CASE("action::ghost::ProcessSubscriber tests ") {

  // Arrange
  TestFixture fixture;
  AssetManager &asset_manager = fixture.GetSceneContext().asset_manager;
  auto set_up = asset_manager.SetUpEmptyGrimoireMachina();
  REQUIRE(set_up.has_value());
  auto *grimoire = asset_manager.GetGrimoireMachina().value();
  MrGhost mr_ghost;

  SECTION("ProcessSubscriber – SELECT with FragmentTag resolves instance in "
          "MrGhost",
          "[unit][action_ghost]") {
    // Arrange
    // add a fragment to the GrimoireMachina
    FragmentTag fragment_tag{"iron"};
    Fragment fragment;
    fragment.name = fragment_tag.key;
    grimoire->m_all_fragments.insert({fragment_tag.key, fragment});

    // create a subscriber with a captured payload of SELECT action and
    // FragmentTag
    Subscriber subscriber;
    subscriber.captured_payload =
        GhostPayload{GhostPayload::GhostAction::SELECT, FragmentTag{"iron"}};

    // Act
    ProcessSubscriber(subscriber, mr_ghost, asset_manager);

    // Assert
    REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(
        mr_ghost.m_instance));
    REQUIRE(std::get<steamrot::FragmentInstance>(mr_ghost.m_instance)
                .GetPart()
                .name == "iron");
  }

  SECTION("ProcessSubscriber – SELECT with JointTag resolves instance in "
          "MrGhost",
          "[unit][action_ghost]") {
    JointTag joint_tag{"hinge"};
    Joint joint;
    joint.name = joint_tag.key;
    grimoire->m_all_joints.insert({joint_tag.key, joint});

    Subscriber subscriber;
    subscriber.captured_payload = steamrot::GhostPayload{
        GhostPayload::GhostAction::SELECT, JointTag{"hinge"}};

    ProcessSubscriber(subscriber, mr_ghost, asset_manager);

    REQUIRE(std::holds_alternative<JointInstance>(mr_ghost.m_instance));
    REQUIRE(std::get<JointInstance>(mr_ghost.m_instance).GetPart().name ==
            "hinge");
  }

  SECTION("ProcessSubscriber – CLEAR resets MrGhost instance",
          "[unit][action_ghost]") {
    FragmentTag fragment_tag{"rock"};
    Fragment fragment;
    fragment.name = fragment_tag.key;
    grimoire->m_all_fragments.insert({fragment_tag.key, fragment});

    MrGhost mr_ghost;
    mr_ghost.m_instance.emplace<FragmentInstance>(
        0, grimoire->m_all_fragments["rock"]);

    Subscriber subscriber;
    subscriber.captured_payload =
        GhostPayload{GhostPayload::GhostAction::CLEAR, std::monostate{}};

    ProcessSubscriber(subscriber, mr_ghost, asset_manager);

    REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_instance));
  }

  SECTION("ProcessSubscriber – no captured payload leaves MrGhost unchanged",
          "[unit][action_ghost]") {
    FragmentTag fragment_tag{"rock"};
    Fragment fragment;
    fragment.name = fragment_tag.key;
    grimoire->m_all_fragments.insert({fragment_tag.key, fragment});

    MrGhost mr_ghost;
    mr_ghost.m_instance.emplace<FragmentInstance>(
        0, grimoire->m_all_fragments["rock"]);

    Subscriber subscriber;
    // captured_payload left as std::nullopt

    ProcessSubscriber(subscriber, mr_ghost, asset_manager);

    REQUIRE(std::holds_alternative<FragmentInstance>(mr_ghost.m_instance));
    REQUIRE(std::get<FragmentInstance>(mr_ghost.m_instance).GetPart().name ==
            "rock");
  }

  SECTION(
      "ProcessSubscriber – non-GhostPayload captured payload leaves MrGhost "
      "unchanged",
      "[unit][action_ghost]") {
    FragmentTag fragment_tag{"rock"};
    Fragment fragment;
    fragment.name = fragment_tag.key;
    grimoire->m_all_fragments.insert({"rock", fragment});

    MrGhost mr_ghost;
    mr_ghost.m_instance.emplace<FragmentInstance>(
        0, grimoire->m_all_fragments["rock"]);

    Subscriber subscriber;
    subscriber.captured_payload = std::monostate{};

    ProcessSubscriber(subscriber, mr_ghost, asset_manager);

    REQUIRE(std::holds_alternative<FragmentInstance>(mr_ghost.m_instance));
  }
}
TEST_CASE("action::ghost::ProcessSubscribers tests") {

  // Arrange
  TestFixture fixture;
  AssetManager &asset_manager = fixture.GetSceneContext().asset_manager;
  auto set_up = asset_manager.SetUpEmptyGrimoireMachina();
  REQUIRE(set_up.has_value());
  auto *grimoire = asset_manager.GetGrimoireMachina().value();
  MrGhost mr_ghost;
  SECTION("ProcessSubscribers – inactive subscriber is skipped",
          "[unit][action_ghost]") {
    FragmentTag fragment_tag{"rock"};
    Fragment fragment;
    fragment.name = fragment_tag.key;
    grimoire->m_all_fragments.insert({fragment_tag.key, fragment});

    MrGhost mr_ghost;
    mr_ghost.m_instance.emplace<FragmentInstance>(
        0, grimoire->m_all_fragments["rock"]);

    auto subscriber = std::make_shared<Subscriber>();
    subscriber->m_active = false;
    subscriber->captured_payload =
        GhostPayload{GhostPayload::GhostAction::CLEAR, std::monostate{}};

    ProcessSubscribers({subscriber}, mr_ghost, asset_manager);

    REQUIRE(std::holds_alternative<FragmentInstance>(mr_ghost.m_instance));
  }

  SECTION("ProcessSubscribers – active SELECT subscriber resolves instance",
          "[unit][action_ghost]") {
    FragmentTag fragment_tag{"copper"};
    Fragment fragment;
    fragment.name = fragment_tag.key;
    grimoire->m_all_fragments.insert({fragment_tag.key, fragment});

    MrGhost mr_ghost;

    auto subscriber = std::make_shared<Subscriber>();
    subscriber->m_active = true;
    subscriber->captured_payload =
        GhostPayload{GhostPayload::GhostAction::SELECT, FragmentTag{"copper"}};

    ProcessSubscribers({subscriber}, mr_ghost, asset_manager);

    REQUIRE(std::holds_alternative<FragmentInstance>(mr_ghost.m_instance));
    REQUIRE(std::get<FragmentInstance>(mr_ghost.m_instance).GetPart().name ==
            "copper");
  }

  SECTION("ProcessSubscribers – active CLEAR subscriber clears MrGhost",
          "[unit][action_ghost]") {
    FragmentTag fragment_tag{"rock"};
    Fragment fragment;
    fragment.name = fragment_tag.key;
    grimoire->m_all_fragments.insert({fragment_tag.key, fragment});

    MrGhost mr_ghost;
    mr_ghost.m_instance.emplace<FragmentInstance>(
        0, grimoire->m_all_fragments["rock"]);

    auto subscriber = std::make_shared<Subscriber>();
    subscriber->m_active = true;
    subscriber->captured_payload =
        GhostPayload{GhostPayload::GhostAction::CLEAR, std::monostate{}};

    ProcessSubscribers({subscriber}, mr_ghost, asset_manager);

    REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_instance));
  }
}

} // namespace steamrot::tests
