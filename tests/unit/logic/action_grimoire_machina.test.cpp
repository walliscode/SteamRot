/////////////////////////////////////////////////
/// @file
/// @brief unit tests for the GrimoireMachina action processing functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_grimoire_machina.h"
#include "EventPayload.h"
#include "EventType.h"
#include "MachinaFormScaffold.h"
#include "Subscriber.h"
#include "TestFixture.h"
#include "Vector2fEqualsMatcher.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

TEST_CASE("InitialiseActiveMachinaForm adds a new MachinaForm to the "
          "GrimoireMachina active form",

          "[GrimoireMachina]") {
  steamrot::GrimoireMachina grimoire_machina;

  auto result = steamrot::logic::action::grimoire_machina::
      InitialiseActiveMachinaFormScaffold(grimoire_machina);
  REQUIRE(result.has_value());
  REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
}

TEST_CASE("ClearActiveMachinaForm clears the active MachinaForm in the "
          "GrimoireMachina",
          "[GrimoireMachina]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
  auto result =
      steamrot::logic::action::grimoire_machina::ClearActiveMachinaFormScaffold(
          grimoire_machina);
  REQUIRE(result.has_value());
  REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
}

TEST_CASE("GetAllFragmentNames returns the string names of all fragments in "
          "the GrimoireMachina",
          "[unit][actions][grimoire_machina]") {
  steamrot::GrimoireMachina grimoire_machina;
  SECTION("No fragments in GrimoireMachina") {
    auto fragment_names =
        steamrot::logic::action::grimoire_machina::GetAllFragmentNames(
            grimoire_machina);
    REQUIRE(fragment_names.empty());
  }

  SECTION("Multiple fragments in GrimoireMachina") {
    grimoire_machina.m_all_fragments = {{"Fragment1", steamrot::Fragment{}},
                                        {"Fragment2", steamrot::Fragment{}},
                                        {"Fragment3", steamrot::Fragment{}}};
    auto fragment_names =
        steamrot::logic::action::grimoire_machina::GetAllFragmentNames(
            grimoire_machina);
    REQUIRE(fragment_names.size() == 3);
    REQUIRE(fragment_names[0] == "Fragment1");
    REQUIRE(fragment_names[1] == "Fragment2");
    REQUIRE(fragment_names[2] == "Fragment3");
  }
}

TEST_CASE("GetAllJointNames returns the string names of all joints in "
          "the GrimoireMachina",
          "[unit][actions][grimoire_machina]") {
  steamrot::GrimoireMachina grimoire_machina;
  SECTION("No joints in GrimoireMachina") {
    auto joint_names =
        steamrot::logic::action::grimoire_machina::GetAllJointNames(
            grimoire_machina);
    REQUIRE(joint_names.empty());
  }

  SECTION("Multiple joints in GrimoireMachina") {
    grimoire_machina.m_all_joints = {{"Joint1", steamrot::Joint{}},
                                     {"Joint2", steamrot::Joint{}},
                                     {"Joint3", steamrot::Joint{}}};
    auto joint_names =
        steamrot::logic::action::grimoire_machina::GetAllJointNames(
            grimoire_machina);
    REQUIRE(joint_names.size() == 3);
    REQUIRE(joint_names[0] == "Joint1");
    REQUIRE(joint_names[1] == "Joint2");
    REQUIRE(joint_names[2] == "Joint3");
  }
}

/////////////////////////////////////////////////
/// SocketState default-state tests
/////////////////////////////////////////////////

TEST_CASE("SocketState has Available state by default",
          "[unit][SocketState][MachinaFormScaffold]") {
  steamrot::SocketState socket_state;
  REQUIRE(socket_state.state == steamrot::SocketState::State::Available);
}

TEST_CASE("SocketState has is_mouse_over false by default",
          "[unit][SocketState][MachinaFormScaffold]") {
  steamrot::SocketState socket_state;
  REQUIRE(socket_state.is_mouse_over == false);
}

/////////////////////////////////////////////////
/// FragmentInstance constructor tests
/////////////////////////////////////////////////

TEST_CASE(
    "FragmentInstance constructor creates one socket state per Fragment socket",
    "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;
  fragment.sockets = {{10.f, 20.f}, {30.f, 40.f}, {50.f, 60.f}};

  steamrot::FragmentInstance instance{fragment};

  REQUIRE(instance.socket_states.size() == 3);
}

TEST_CASE("FragmentInstance constructor stores the Fragment reference",
          "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;
  fragment.name = "test_fragment";
  fragment.sockets = {{0.f, 0.f}};

  steamrot::FragmentInstance instance{fragment};

  REQUIRE(&instance.fragment == &fragment);
}

TEST_CASE("FragmentInstance constructor stores the initial transform",
          "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;
  fragment.sockets = {{0.f, 0.f}};

  sf::Transform transform;
  transform.translate({50.f, 75.f});

  steamrot::FragmentInstance instance{fragment, transform};

  REQUIRE(instance.transform == transform);
}

TEST_CASE("FragmentInstance constructor with no sockets creates empty "
          "socket_states vector",
          "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;
  fragment.sockets = {};

  steamrot::FragmentInstance instance{fragment};

  REQUIRE(instance.socket_states.empty());
}

TEST_CASE("FragmentInstance constructor initialises socket states to Available",
          "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;
  fragment.sockets = {{10.f, 20.f}, {30.f, 40.f}};

  steamrot::FragmentInstance instance{fragment};

  REQUIRE(instance.socket_states[0].state ==
          steamrot::SocketState::State::Available);
  REQUIRE(instance.socket_states[1].state ==
          steamrot::SocketState::State::Available);
}

TEST_CASE("FragmentInstance id defaults to zero",
          "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;
  fragment.sockets = {{0.f, 0.f}};

  steamrot::FragmentInstance instance{fragment};

  REQUIRE(instance.id == 0u);
}

/////////////////////////////////////////////////
/// JointInstance constructor tests
/////////////////////////////////////////////////

TEST_CASE("JointInstance constructor creates one socket state per Joint socket",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 2;

  steamrot::JointInstance instance{joint};

  REQUIRE(instance.socket_states.size() == 2);
}

TEST_CASE("JointInstance constructor stores the Joint reference",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.name = "test_joint";
  joint.socket_config.socket_count = 1;

  steamrot::JointInstance instance{joint};

  REQUIRE(&instance.joint == &joint);
}

TEST_CASE("JointInstance constructor stores the initial transform",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 1;

  sf::Transform transform;
  transform.translate({50.f, 75.f});

  steamrot::JointInstance instance{joint, transform};

  REQUIRE(instance.transform == transform);
}

TEST_CASE("JointInstance constructor with socket_count zero creates empty "
          "socket_states vector",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 0;

  steamrot::JointInstance instance{joint};

  REQUIRE(instance.socket_states.empty());
}

TEST_CASE("JointInstance constructor initialises socket states to Available",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 2;

  steamrot::JointInstance instance{joint};

  REQUIRE(instance.socket_states[0].state ==
          steamrot::SocketState::State::Available);
  REQUIRE(instance.socket_states[1].state ==
          steamrot::SocketState::State::Available);
}

TEST_CASE("JointInstance id defaults to zero",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 1;

  steamrot::JointInstance instance{joint};

  REQUIRE(instance.id == 0u);
}

TEST_CASE("JointInstance constructor defaults current_rotation to zero",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 1;

  steamrot::JointInstance instance{joint};

  REQUIRE(instance.current_rotation == 0.f);
}

TEST_CASE("JointInstance constructor defaults rotation_min to -180",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 1;

  steamrot::JointInstance instance{joint};

  REQUIRE(instance.rotation_min == -180.f);
}

TEST_CASE("JointInstance constructor defaults rotation_max to 180",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 1;

  steamrot::JointInstance instance{joint};

  REQUIRE(instance.rotation_max == 180.f);
}

/////////////////////////////////////////////////
/// PlaceGhostOnScaffold error-path tests
/////////////////////////////////////////////////

TEST_CASE("PlaceGhostOnScaffold returns error when no scaffold is active",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  auto result = steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost, {50.f, 50.f});

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
  REQUIRE(result.error().message == "PlaceGhostOnScaffold: no active scaffold");
}

TEST_CASE(
    "PlaceGhostOnScaffold returns error when ghost selection is monostate",
    "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost mr_ghost; // default selection = monostate

  auto result = steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost, {50.f, 50.f});

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::InvalidInput);
  REQUIRE(result.error().message == "PlaceFirstPiece: no ghost item selected");
}

TEST_CASE("PlaceGhostOnScaffold returns error when fragment key not found",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"nonexistent_fragment"};

  auto result = steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost, {50.f, 50.f});

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingData);
  REQUIRE(result.error().message == "PlaceFirstPiece: fragment key not found");
}

TEST_CASE("PlaceGhostOnScaffold returns error when joint key not found",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"nonexistent_joint"};

  auto result = steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost, {50.f, 50.f});

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingData);
  REQUIRE(result.error().message == "PlaceFirstPiece: joint key not found");
}

/////////////////////////////////////////////////
/// PlaceGhostOnScaffold first-piece fragment tests
/////////////////////////////////////////////////

TEST_CASE("PlaceGhostOnScaffold first piece: appends fragment to scaffold",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  auto result = steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost, {100.f, 80.f});

  REQUIRE(result.has_value());
  REQUIRE(grimoire_machina.m_scaffold_form->fragments.size() == 1);
  REQUIRE(grimoire_machina.m_scaffold_form->joints.empty());
}

TEST_CASE(
    "PlaceGhostOnScaffold first piece: fragment id and next_id are assigned",
    "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  auto result = steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost, {100.f, 80.f});

  if (!result.has_value()) {
    FAIL("PlaceGhostOnScaffold failed with error: " + result.error().message);
  }
  // use unsigned integer literals to avoid signed/unsigned comparison warnings
  REQUIRE(grimoire_machina.m_scaffold_form->fragments[0].id == 0u);
  REQUIRE(grimoire_machina.m_scaffold_form->next_id == 1u);
}

TEST_CASE(
    "PlaceGhostOnScaffold first piece: fragment transform centers on world_pos "
    "with empty bounds",
    "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  // With a default Fragment (empty VertexArray), bounds.position = {0,0} and
  // bounds.size = {0,0}.
  // First-piece anchor: translate(world_pos - bounds.position - bounds.size/2)
  //                   = translate(world_pos - {0,0} - {0,0}) =
  //                   translate(world_pos).
  // Therefore transformPoint({0,0}) == world_pos.
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  const sf::Vector2f world_pos{60.f, 40.f};
  auto result = steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost, world_pos);
  if (!result.has_value()) {
    FAIL("PlaceGhostOnScaffold failed with error: " + result.error().message);
  }
  const sf::Transform &t =
      grimoire_machina.m_scaffold_form->fragments[0].transform;
  const sf::Vector2f mapped = t.transformPoint({0.f, 0.f});

  REQUIRE_THAT(mapped, steamrot::tests::EqualsVector2f(world_pos));
}

/////////////////////////////////////////////////
/// PlaceGhostOnScaffold first-piece joint tests
/////////////////////////////////////////////////

TEST_CASE("PlaceGhostOnScaffold first piece: appends joint to scaffold",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"joint"};

  auto result = steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost, {50.f, 50.f});

  REQUIRE(result.has_value());
  REQUIRE(grimoire_machina.m_scaffold_form->joints.size() == 1);
  REQUIRE(grimoire_machina.m_scaffold_form->fragments.empty());
}

TEST_CASE("PlaceGhostOnScaffold first piece: joint id and next_id are assigned",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"joint"};

  auto result = steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost, {50.f, 50.f});

  if (!result.has_value()) {
    FAIL("PlaceGhostOnScaffold failed with error: " + result.error().message);
  }
  REQUIRE(grimoire_machina.m_scaffold_form->joints[0].id == 0u);
  REQUIRE(grimoire_machina.m_scaffold_form->next_id == 1u);
}

TEST_CASE(
    "PlaceGhostOnScaffold first piece: joint transform centers on world_pos "
    "with empty bounds",
    "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"joint"};

  const sf::Vector2f world_pos{70.f, 30.f};
  auto result = steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost, world_pos);
  if (!result.has_value()) {
    FAIL("PlaceGhostOnScaffold failed with error: " + result.error().message);
  }

  const sf::Transform &t =
      grimoire_machina.m_scaffold_form->joints[0].transform;
  const sf::Vector2f mapped = t.transformPoint({0.f, 0.f});

  REQUIRE_THAT(mapped, steamrot::tests::EqualsVector2f(world_pos));
}

/////////////////////////////////////////////////
/// PlaceGhostOnScaffold subsequent-piece tests (blocked until collision logic)
/////////////////////////////////////////////////

TEST_CASE(
    "PlaceGhostOnScaffold returns error when scaffold already has a fragment",
    "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  // Place the first piece successfully.
  auto first_result =
      steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
          grimoire_machina, mr_ghost, {50.f, 50.f});
  REQUIRE(first_result.has_value());

  // Attempting a second placement must fail (no collision logic yet).
  auto second_result =
      steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
          grimoire_machina, mr_ghost, {60.f, 60.f});

  REQUIRE_FALSE(second_result.has_value());
  REQUIRE(second_result.error().mode == steamrot::FailMode::InvalidInput);
  REQUIRE(second_result.error().message ==
          "PlaceGhostOnScaffold: no valid socket connection");
}

TEST_CASE(
    "PlaceGhostOnScaffold returns error when scaffold already has a joint",
    "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"joint"};

  // Place the first piece successfully.
  auto first_result =
      steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
          grimoire_machina, mr_ghost, {50.f, 50.f});
  REQUIRE(first_result.has_value());

  // Attempting a second placement must fail (no collision logic yet).
  auto second_result =
      steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
          grimoire_machina, mr_ghost, {60.f, 60.f});

  REQUIRE_FALSE(second_result.has_value());
  REQUIRE(second_result.error().mode == steamrot::FailMode::InvalidInput);
  REQUIRE(second_result.error().message ==
          "PlaceGhostOnScaffold: no valid socket connection");
}

/////////////////////////////////////////////////
/// PlaceGhostOnScaffold single-piece-per-instance tests
/////////////////////////////////////////////////

TEST_CASE("PlaceGhostOnScaffold only places one fragment per game instance",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  auto first = steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost, {50.f, 50.f});
  REQUIRE(first.has_value());

  auto second = steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost, {60.f, 60.f});
  REQUIRE_FALSE(second.has_value());

  auto third = steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost, {70.f, 70.f});
  REQUIRE_FALSE(third.has_value());

  REQUIRE(grimoire_machina.m_scaffold_form->fragments.size() == 1);
  REQUIRE(grimoire_machina.m_scaffold_form->joints.empty());
}

TEST_CASE("PlaceGhostOnScaffold only places one joint per game instance",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"joint"};

  auto first = steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost, {50.f, 50.f});
  REQUIRE(first.has_value());

  auto second = steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost, {60.f, 60.f});
  REQUIRE_FALSE(second.has_value());

  auto third = steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost, {70.f, 70.f});
  REQUIRE_FALSE(third.has_value());

  REQUIRE(grimoire_machina.m_scaffold_form->joints.size() == 1);
  REQUIRE(grimoire_machina.m_scaffold_form->fragments.empty());
}

/////////////////////////////////////////////////
/// PlaceFirstPiece error-path tests
/////////////////////////////////////////////////

TEST_CASE("PlaceFirstPiece returns error when no scaffold is active",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  auto result = steamrot::logic::action::grimoire_machina::PlaceFirstPiece(
      grimoire_machina, mr_ghost, {50.f, 50.f});

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
  REQUIRE(result.error().message == "PlaceFirstPiece: no active scaffold");
}

TEST_CASE("PlaceFirstPiece returns error when scaffold already has pieces",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  // Place the first piece via PlaceFirstPiece.
  auto first_result =
      steamrot::logic::action::grimoire_machina::PlaceFirstPiece(
          grimoire_machina, mr_ghost, {50.f, 50.f});
  REQUIRE(first_result.has_value());

  // A second call must be rejected.
  auto second_result =
      steamrot::logic::action::grimoire_machina::PlaceFirstPiece(
          grimoire_machina, mr_ghost, {60.f, 60.f});

  REQUIRE_FALSE(second_result.has_value());
  REQUIRE(second_result.error().mode == steamrot::FailMode::InvalidInput);
  REQUIRE(second_result.error().message ==
          "PlaceFirstPiece: scaffold is not empty");
}

TEST_CASE("PlaceFirstPiece returns error when ghost selection is monostate",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost mr_ghost; // default selection = monostate

  auto result = steamrot::logic::action::grimoire_machina::PlaceFirstPiece(
      grimoire_machina, mr_ghost, {50.f, 50.f});

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::InvalidInput);
  REQUIRE(result.error().message == "PlaceFirstPiece: no ghost item selected");
}

TEST_CASE("PlaceFirstPiece returns error when fragment key not found",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"nonexistent"};

  auto result = steamrot::logic::action::grimoire_machina::PlaceFirstPiece(
      grimoire_machina, mr_ghost, {50.f, 50.f});

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingData);
  REQUIRE(result.error().message == "PlaceFirstPiece: fragment key not found");
}

TEST_CASE("PlaceFirstPiece returns error when joint key not found",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"nonexistent"};

  auto result = steamrot::logic::action::grimoire_machina::PlaceFirstPiece(
      grimoire_machina, mr_ghost, {50.f, 50.f});

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingData);
  REQUIRE(result.error().message == "PlaceFirstPiece: joint key not found");
}

/////////////////////////////////////////////////
/// PlaceFirstPiece success tests
/////////////////////////////////////////////////

TEST_CASE("PlaceFirstPiece appends a fragment to an empty scaffold",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  auto result = steamrot::logic::action::grimoire_machina::PlaceFirstPiece(
      grimoire_machina, mr_ghost, {100.f, 80.f});

  REQUIRE(result.has_value());
  REQUIRE(grimoire_machina.m_scaffold_form->fragments.size() == 1);
  REQUIRE(grimoire_machina.m_scaffold_form->joints.empty());
}

TEST_CASE("PlaceFirstPiece appends a joint to an empty scaffold",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"joint"};

  auto result = steamrot::logic::action::grimoire_machina::PlaceFirstPiece(
      grimoire_machina, mr_ghost, {50.f, 50.f});

  REQUIRE(result.has_value());
  REQUIRE(grimoire_machina.m_scaffold_form->joints.size() == 1);
  REQUIRE(grimoire_machina.m_scaffold_form->fragments.empty());
}

TEST_CASE("PlaceFirstPiece assigns fragment id 0 and increments next_id",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  auto result = steamrot::logic::action::grimoire_machina::PlaceFirstPiece(
      grimoire_machina, mr_ghost, {50.f, 50.f});

  if (!result.has_value()) {
    FAIL("PlaceFirstPiece failed with error: " + result.error().message);
  }
  REQUIRE(grimoire_machina.m_scaffold_form->fragments[0].id == 0u);
  REQUIRE(grimoire_machina.m_scaffold_form->next_id == 1u);
}

TEST_CASE("PlaceFirstPiece assigns joint id 0 and increments next_id",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"joint"};

  auto result = steamrot::logic::action::grimoire_machina::PlaceFirstPiece(
      grimoire_machina, mr_ghost, {50.f, 50.f});

  if (!result.has_value()) {
    FAIL("PlaceFirstPiece failed with error: " + result.error().message);
  }
  REQUIRE(grimoire_machina.m_scaffold_form->joints[0].id == 0u);
  REQUIRE(grimoire_machina.m_scaffold_form->next_id == 1u);
}

TEST_CASE("PlaceFirstPiece: fragment transform centers on world_pos with empty "
          "bounds",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  const sf::Vector2f world_pos{60.f, 40.f};
  auto result = steamrot::logic::action::grimoire_machina::PlaceFirstPiece(
      grimoire_machina, mr_ghost, world_pos);
  if (!result.has_value()) {
    FAIL("PlaceFirstPiece failed with error: " + result.error().message);
  }

  const sf::Transform &t =
      grimoire_machina.m_scaffold_form->fragments[0].transform;
  REQUIRE_THAT(t.transformPoint({0.f, 0.f}),
               steamrot::tests::EqualsVector2f(world_pos));
}

TEST_CASE(
    "PlaceFirstPiece: joint transform centers on world_pos with empty bounds",
    "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"joint"};

  const sf::Vector2f world_pos{70.f, 30.f};
  auto result = steamrot::logic::action::grimoire_machina::PlaceFirstPiece(
      grimoire_machina, mr_ghost, world_pos);
  if (!result.has_value()) {
    FAIL("PlaceFirstPiece failed with error: " + result.error().message);
  }

  const sf::Transform &t =
      grimoire_machina.m_scaffold_form->joints[0].transform;
  REQUIRE_THAT(t.transformPoint({0.f, 0.f}),
               steamrot::tests::EqualsVector2f(world_pos));
}

/////////////////////////////////////////////////
/// PlaceFirstPiece single-piece-only tests
/////////////////////////////////////////////////

TEST_CASE("PlaceFirstPiece only places one piece per game instance",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  auto first = steamrot::logic::action::grimoire_machina::PlaceFirstPiece(
      grimoire_machina, mr_ghost, {50.f, 50.f});
  REQUIRE(first.has_value());

  auto second = steamrot::logic::action::grimoire_machina::PlaceFirstPiece(
      grimoire_machina, mr_ghost, {60.f, 60.f});
  REQUIRE_FALSE(second.has_value());

  auto third = steamrot::logic::action::grimoire_machina::PlaceFirstPiece(
      grimoire_machina, mr_ghost, {70.f, 70.f});
  REQUIRE_FALSE(third.has_value());

  REQUIRE(grimoire_machina.m_scaffold_form->fragments.size() == 1);
  REQUIRE(grimoire_machina.m_scaffold_form->joints.empty());
}

/////////////////////////////////////////////////
/// ProcessScaffoldSubscribers tests
/////////////////////////////////////////////////

TEST_CASE("ProcessScaffoldSubscribers: inactive subscriber is skipped",
          "[unit][actions][grimoire_machina][ProcessScaffoldSubscribers]") {
  steamrot::GrimoireMachina grimoire_machina;

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = false;
  subscriber->event_type = steamrot::EventType::LOGIC;
  subscriber->captured_payload = steamrot::LogicPayload{
      steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD};

  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers{subscriber};

  steamrot::logic::action::grimoire_machina::ProcessScaffoldSubscribers(
      subscribers, grimoire_machina);

  // scaffold must not have been initialised
  REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
}

TEST_CASE("ProcessScaffoldSubscribers: active INITIATE subscriber initialises "
          "scaffold",
          "[unit][actions][grimoire_machina][ProcessScaffoldSubscribers]") {
  steamrot::GrimoireMachina grimoire_machina;

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->event_type = steamrot::EventType::LOGIC;
  subscriber->captured_payload = steamrot::LogicPayload{
      steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD};

  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers{subscriber};

  steamrot::logic::action::grimoire_machina::ProcessScaffoldSubscribers(
      subscribers, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
}

TEST_CASE("ProcessScaffoldSubscribers: active CLEAR subscriber clears existing "
          "scaffold",
          "[unit][actions][grimoire_machina][ProcessScaffoldSubscribers]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->event_type = steamrot::EventType::LOGIC;
  subscriber->captured_payload = steamrot::LogicPayload{
      steamrot::LogicPayload::LogicToggle::CLEAR_MACHINA_FORM_SCAFFOLD};

  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers{subscriber};

  steamrot::logic::action::grimoire_machina::ProcessScaffoldSubscribers(
      subscribers, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
}

/////////////////////////////////////////////////
/// ProcessPlacementSubscribers tests
/////////////////////////////////////////////////

TEST_CASE("ProcessPlacementSubscribers: inactive subscriber is skipped",
          "[unit][actions][grimoire_machina][ProcessPlacementSubscribers]") {
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};
  fixture.GetMrGhost().m_selection = steamrot::FragmentTag{"frag"};

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = false;
  subscriber->event_type = steamrot::EventType::USER_INPUT;

  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers{subscriber};

  steamrot::logic::action::grimoire_machina::ProcessPlacementSubscribers(
      subscribers, scene_context, grimoire_machina);

  // No placement should have occurred
  REQUIRE(grimoire_machina.m_scaffold_form->fragments.empty());
}

TEST_CASE("ProcessPlacementSubscribers: non-USER_INPUT subscriber is skipped",
          "[unit][actions][grimoire_machina][ProcessPlacementSubscribers]") {
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};
  fixture.GetMrGhost().m_selection = steamrot::FragmentTag{"frag"};

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->event_type = steamrot::EventType::LOGIC;

  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers{subscriber};

  steamrot::logic::action::grimoire_machina::ProcessPlacementSubscribers(
      subscribers, scene_context, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form->fragments.empty());
}

TEST_CASE(
    "ProcessPlacementSubscribers: monostate ghost selection blocks placement",
    "[unit][actions][grimoire_machina][ProcessPlacementSubscribers]") {
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  // mr_ghost has default monostate selection

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->event_type = steamrot::EventType::USER_INPUT;

  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers{subscriber};

  steamrot::logic::action::grimoire_machina::ProcessPlacementSubscribers(
      subscribers, scene_context, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form->fragments.empty());
  REQUIRE(grimoire_machina.m_scaffold_form->joints.empty());
}

/////////////////////////////////////////////////
/// ProcessSocketVisibilitySubscribers tests
/////////////////////////////////////////////////

TEST_CASE(
    "ProcessSocketVisibilitySubscribers: active TOGGLE_SOCKET_VISIBILITY "
    "subscriber toggles are_sockets_visible from false to true",
    "[unit][actions][grimoire_machina][ProcessSocketVisibilitySubscribers]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  REQUIRE(grimoire_machina.m_scaffold_form->are_sockets_visible == false);

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->event_type = steamrot::EventType::USER_INPUT;
  subscriber->captured_payload = steamrot::InputPayload{
      steamrot::InputPayload::InputAction::TOGGLE_SOCKET_VISIBILITY};

  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers{subscriber};

  steamrot::logic::action::grimoire_machina::ProcessSocketVisibilitySubscribers(
      subscribers, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form->are_sockets_visible == true);
}

TEST_CASE(
    "ProcessSocketVisibilitySubscribers: toggling twice returns to original "
    "state",
    "[unit][actions][grimoire_machina][ProcessSocketVisibilitySubscribers]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->event_type = steamrot::EventType::USER_INPUT;
  subscriber->captured_payload = steamrot::InputPayload{
      steamrot::InputPayload::InputAction::TOGGLE_SOCKET_VISIBILITY};

  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers{subscriber};

  steamrot::logic::action::grimoire_machina::ProcessSocketVisibilitySubscribers(
      subscribers, grimoire_machina);
  steamrot::logic::action::grimoire_machina::ProcessSocketVisibilitySubscribers(
      subscribers, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form->are_sockets_visible == false);
}

TEST_CASE(
    "ProcessSocketVisibilitySubscribers: inactive subscriber is skipped",
    "[unit][actions][grimoire_machina][ProcessSocketVisibilitySubscribers]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = false;
  subscriber->event_type = steamrot::EventType::USER_INPUT;
  subscriber->captured_payload = steamrot::InputPayload{
      steamrot::InputPayload::InputAction::TOGGLE_SOCKET_VISIBILITY};

  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers{subscriber};

  steamrot::logic::action::grimoire_machina::ProcessSocketVisibilitySubscribers(
      subscribers, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form->are_sockets_visible == false);
}

TEST_CASE(
    "ProcessSocketVisibilitySubscribers: no scaffold means subscriber is "
    "skipped without crash",
    "[unit][actions][grimoire_machina][ProcessSocketVisibilitySubscribers]") {
  steamrot::GrimoireMachina grimoire_machina;
  // no scaffold_form set

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->event_type = steamrot::EventType::USER_INPUT;
  subscriber->captured_payload = steamrot::InputPayload{
      steamrot::InputPayload::InputAction::TOGGLE_SOCKET_VISIBILITY};

  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers{subscriber};

  REQUIRE_NOTHROW(
      steamrot::logic::action::grimoire_machina::
          ProcessSocketVisibilitySubscribers(subscribers, grimoire_machina));
}

TEST_CASE(
    "ProcessSocketVisibilitySubscribers: SELECT subscriber does not toggle "
    "socket visibility",
    "[unit][actions][grimoire_machina][ProcessSocketVisibilitySubscribers]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->event_type = steamrot::EventType::USER_INPUT;
  subscriber->captured_payload =
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};

  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers{subscriber};

  steamrot::logic::action::grimoire_machina::ProcessSocketVisibilitySubscribers(
      subscribers, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form->are_sockets_visible == false);
}

/////////////////////////////////////////////////
/// ProcessSubscribers tests
/////////////////////////////////////////////////

TEST_CASE("ProcessSubscribers: LOGIC INITIATE subscriber initialises scaffold "
          "in a single pass",
          "[unit][actions][grimoire_machina][ProcessSubscribers]") {
  steamrot::GrimoireMachina grimoire_machina;

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->event_type = steamrot::EventType::LOGIC;
  subscriber->captured_payload = steamrot::LogicPayload{
      steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD};

  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers{subscriber};
  steamrot::logic::action::grimoire_machina::ProcessSubscribers(
      subscribers, scene_context, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
}

TEST_CASE("ProcessSubscribers: LOGIC CLEAR subscriber clears scaffold in a "
          "single pass",
          "[unit][actions][grimoire_machina][ProcessSubscribers]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->event_type = steamrot::EventType::LOGIC;
  subscriber->captured_payload = steamrot::LogicPayload{
      steamrot::LogicPayload::LogicToggle::CLEAR_MACHINA_FORM_SCAFFOLD};

  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers{subscriber};
  steamrot::logic::action::grimoire_machina::ProcessSubscribers(
      subscribers, scene_context, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
}

TEST_CASE("ProcessSubscribers: TOGGLE_SOCKET_VISIBILITY subscriber toggles "
          "socket visibility in a single pass",
          "[unit][actions][grimoire_machina][ProcessSubscribers]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  REQUIRE(grimoire_machina.m_scaffold_form->are_sockets_visible == false);

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->event_type = steamrot::EventType::USER_INPUT;
  subscriber->captured_payload = steamrot::InputPayload{
      steamrot::InputPayload::InputAction::TOGGLE_SOCKET_VISIBILITY};

  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers{subscriber};
  steamrot::logic::action::grimoire_machina::ProcessSubscribers(
      subscribers, scene_context, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form->are_sockets_visible == true);
}

TEST_CASE("ProcessSubscribers: inactive subscriber is skipped",
          "[unit][actions][grimoire_machina][ProcessSubscribers]") {
  steamrot::GrimoireMachina grimoire_machina;

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = false;
  subscriber->event_type = steamrot::EventType::LOGIC;
  subscriber->captured_payload = steamrot::LogicPayload{
      steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD};

  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers{subscriber};
  steamrot::logic::action::grimoire_machina::ProcessSubscribers(
      subscribers, scene_context, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
}

TEST_CASE(
    "ProcessSubscribers: multiple mixed subscribers processed in a single pass",
    "[unit][actions][grimoire_machina][ProcessSubscribers]") {
  steamrot::GrimoireMachina grimoire_machina;

  // First: initialise scaffold
  auto init_subscriber = std::make_shared<steamrot::Subscriber>();
  init_subscriber->m_active = true;
  init_subscriber->event_type = steamrot::EventType::LOGIC;
  init_subscriber->captured_payload = steamrot::LogicPayload{
      steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD};

  // Second: toggle socket visibility
  auto toggle_subscriber = std::make_shared<steamrot::Subscriber>();
  toggle_subscriber->m_active = true;
  toggle_subscriber->event_type = steamrot::EventType::USER_INPUT;
  toggle_subscriber->captured_payload = steamrot::InputPayload{
      steamrot::InputPayload::InputAction::TOGGLE_SOCKET_VISIBILITY};

  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  std::vector<std::shared_ptr<steamrot::Subscriber>> subscribers{
      init_subscriber, toggle_subscriber};
  steamrot::logic::action::grimoire_machina::ProcessSubscribers(
      subscribers, scene_context, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
  REQUIRE(grimoire_machina.m_scaffold_form->are_sockets_visible == true);
}
