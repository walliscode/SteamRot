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
#include "positioning_grimoire_machina.h"
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <memory>

TEST_CASE("InitialiseActiveMachinaForm adds a new MachinaForm to the "
          "GrimoireMachina active form",

          "[GrimoireMachina]") {
  steamrot::GrimoireMachina grimoire_machina;

  steamrot::logic::action::grimoire_machina::
      InitialiseActiveMachinaFormScaffold(grimoire_machina);
  REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
}

TEST_CASE("ClearActiveMachinaForm clears the active MachinaForm in the "
          "GrimoireMachina",
          "[GrimoireMachina]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
  steamrot::logic::action::grimoire_machina::ClearActiveMachinaFormScaffold(
      grimoire_machina);
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

TEST_CASE("JointInstance constructor populates socket_local_positions for each "
          "socket",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 3;
  joint.socket_config.radius = 10.f;
  joint.socket_config.arc_min = 0.f;
  joint.socket_config.arc_max = 360.f;
  joint.socket_config.has_fixed_socket = false;

  steamrot::JointInstance instance{joint};

  REQUIRE(instance.socket_local_positions.size() == 3u);
}

TEST_CASE("JointInstance constructor socket_local_positions size matches "
          "socket_states size",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 4;
  joint.socket_config.radius = 20.f;
  joint.socket_config.arc_min = 0.f;
  joint.socket_config.arc_max = 360.f;
  joint.socket_config.has_fixed_socket = false;

  steamrot::JointInstance instance{joint};

  REQUIRE(instance.socket_local_positions.size() ==
          instance.socket_states.size());
}

TEST_CASE("JointInstance constructor with zero sockets has empty "
          "socket_local_positions",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 0;

  steamrot::JointInstance instance{joint};

  REQUIRE(instance.socket_local_positions.empty());
}

TEST_CASE("JointInstance constructor socket_local_positions are zero-initialised",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 3;
  joint.socket_config.radius = 10.f;
  joint.socket_config.arc_min = 0.f;
  joint.socket_config.arc_max = 360.f;
  joint.socket_config.has_fixed_socket = false;

  steamrot::JointInstance instance{joint};

  for (size_t i = 0; i < 3u; ++i) {
    REQUIRE(instance.socket_local_positions[i].x == 0.f);
    REQUIRE(instance.socket_local_positions[i].y == 0.f);
  }
}

TEST_CASE("compute_socket_local_pos returns expected positions at zero ring "
          "rotation",
          "[unit][positioning_grimoire_machina]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 3;
  joint.socket_config.radius = 10.f;
  joint.socket_config.arc_min = 0.f;
  joint.socket_config.arc_max = 360.f;
  joint.socket_config.has_fixed_socket = false;

  for (size_t i = 0; i < 3u; ++i) {
    // Each socket should be at radius distance from origin
    const sf::Vector2f pos =
        steamrot::logic::positioning::grimoire_machina::compute_socket_local_pos(
            joint.socket_config, i, 0.f);
    const float dist = std::sqrt(pos.x * pos.x + pos.y * pos.y);
    REQUIRE(std::abs(dist - joint.socket_config.radius) < 1e-4f);
  }
}

TEST_CASE("initialize_joint_socket_positions populates socket_local_positions "
          "from SocketConfig at zero rotation",
          "[unit][positioning_grimoire_machina]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 3;
  joint.socket_config.radius = 10.f;
  joint.socket_config.arc_min = 0.f;
  joint.socket_config.arc_max = 360.f;
  joint.socket_config.has_fixed_socket = false;

  steamrot::JointInstance instance{joint};
  steamrot::logic::positioning::grimoire_machina::initialize_joint_socket_positions(
      instance);

  for (size_t i = 0; i < 3u; ++i) {
    const sf::Vector2f expected =
        steamrot::logic::positioning::grimoire_machina::compute_socket_local_pos(
            joint.socket_config, i, 0.f);
    REQUIRE(instance.socket_local_positions[i].x == expected.x);
    REQUIRE(instance.socket_local_positions[i].y == expected.y);
  }
}

/////////////////////////////////////////////////
/// PlaceGhostOnScaffold guard tests
/////////////////////////////////////////////////

TEST_CASE("PlaceGhostOnScaffold does nothing when no scaffold is active",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  REQUIRE_NOTHROW(
      steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
          grimoire_machina, mr_ghost));

  // no scaffold was ever created — it must remain null
  REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
}

TEST_CASE("PlaceGhostOnScaffold does nothing when ghost selection is monostate",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost mr_ghost; // default selection = monostate

  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());
}

TEST_CASE("PlaceGhostOnScaffold does nothing when fragment key not found",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"nonexistent_fragment"};

  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());
}

TEST_CASE("PlaceGhostOnScaffold does nothing when joint key not found",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"nonexistent_joint"};

  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());
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

  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
  REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(
      grimoire_machina.m_scaffold_form->parts.at(0)));
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

  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);

  REQUIRE(std::get<steamrot::FragmentInstance>(
              grimoire_machina.m_scaffold_form->parts.at(0))
              .id == 0u);
  REQUIRE(grimoire_machina.m_scaffold_form->next_id == 1u);
}

TEST_CASE("PlaceGhostOnScaffold first piece: fragment instance has identity "
          "transform",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);

  REQUIRE(std::get<steamrot::FragmentInstance>(
              grimoire_machina.m_scaffold_form->parts.at(0))
              .transform == sf::Transform::Identity);
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

  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
  REQUIRE(std::holds_alternative<steamrot::JointInstance>(
      grimoire_machina.m_scaffold_form->parts.at(0)));
}

TEST_CASE("PlaceGhostOnScaffold first piece: joint id and next_id are assigned",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"joint"};

  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);

  REQUIRE(std::get<steamrot::JointInstance>(
              grimoire_machina.m_scaffold_form->parts.at(0))
              .id == 0u);
  REQUIRE(grimoire_machina.m_scaffold_form->next_id == 1u);
}

TEST_CASE(
    "PlaceGhostOnScaffold first piece: joint instance has identity transform",
    "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"joint"};

  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);

  REQUIRE(std::get<steamrot::JointInstance>(
              grimoire_machina.m_scaffold_form->parts.at(0))
              .transform == sf::Transform::Identity);
}

/////////////////////////////////////////////////
/// PlaceGhostOnScaffold subsequent-piece tests (blocked until collision logic)
/////////////////////////////////////////////////

TEST_CASE(
    "PlaceGhostOnScaffold does not add pieces when scaffold already has a "
    "fragment",
    "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  // Place the first piece successfully.
  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);
  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);

  // A second placement must not add any more pieces (no collision logic yet).
  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
  REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(
      grimoire_machina.m_scaffold_form->parts.at(0)));
}

TEST_CASE(
    "PlaceGhostOnScaffold does not add pieces when scaffold already has a "
    "joint",
    "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"joint"};

  // Place the first piece successfully.
  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);
  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);

  // A second placement must not add any more pieces (no collision logic yet).
  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
  REQUIRE(std::holds_alternative<steamrot::JointInstance>(
      grimoire_machina.m_scaffold_form->parts.at(0)));
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

  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);
  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);
  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
  REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(
      grimoire_machina.m_scaffold_form->parts.at(0)));
}

TEST_CASE("PlaceGhostOnScaffold only places one joint per game instance",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"joint"};

  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);
  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);
  steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
  REQUIRE(std::holds_alternative<steamrot::JointInstance>(
      grimoire_machina.m_scaffold_form->parts.at(0)));
}

/////////////////////////////////////////////////
/// PlaceFirstPiece guard tests
/////////////////////////////////////////////////

TEST_CASE("PlaceFirstPiece does nothing when no scaffold is active",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  REQUIRE_NOTHROW(steamrot::logic::action::grimoire_machina::PlaceFirstPiece(
      grimoire_machina, mr_ghost));

  // no scaffold was ever created — it must remain null
  REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
}

TEST_CASE("PlaceFirstPiece does nothing when scaffold already has pieces",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  // Place the first piece.
  steamrot::logic::action::grimoire_machina::PlaceFirstPiece(grimoire_machina,
                                                             mr_ghost);
  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);

  // A second call must be ignored.
  steamrot::logic::action::grimoire_machina::PlaceFirstPiece(grimoire_machina,
                                                             mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
}

TEST_CASE("PlaceFirstPiece does nothing when ghost selection is monostate",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost mr_ghost; // default selection = monostate

  steamrot::logic::action::grimoire_machina::PlaceFirstPiece(grimoire_machina,
                                                             mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());
}

TEST_CASE("PlaceFirstPiece does nothing when fragment key not found",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"nonexistent"};

  steamrot::logic::action::grimoire_machina::PlaceFirstPiece(grimoire_machina,
                                                             mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());
}

TEST_CASE("PlaceFirstPiece does nothing when joint key not found",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"nonexistent"};

  steamrot::logic::action::grimoire_machina::PlaceFirstPiece(grimoire_machina,
                                                             mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());
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

  steamrot::logic::action::grimoire_machina::PlaceFirstPiece(grimoire_machina,
                                                             mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
  REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(
      grimoire_machina.m_scaffold_form->parts.at(0)));
}

TEST_CASE("PlaceFirstPiece appends a joint to an empty scaffold",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"joint"};

  steamrot::logic::action::grimoire_machina::PlaceFirstPiece(grimoire_machina,
                                                             mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
  REQUIRE(std::holds_alternative<steamrot::JointInstance>(
      grimoire_machina.m_scaffold_form->parts.at(0)));
}

TEST_CASE("PlaceFirstPiece assigns fragment id 0 and increments next_id",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  steamrot::logic::action::grimoire_machina::PlaceFirstPiece(grimoire_machina,
                                                             mr_ghost);

  REQUIRE(std::get<steamrot::FragmentInstance>(
              grimoire_machina.m_scaffold_form->parts.at(0))
              .id == 0u);
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

  steamrot::logic::action::grimoire_machina::PlaceFirstPiece(grimoire_machina,
                                                             mr_ghost);

  REQUIRE(std::get<steamrot::JointInstance>(
              grimoire_machina.m_scaffold_form->parts.at(0))
              .id == 0u);
  REQUIRE(grimoire_machina.m_scaffold_form->next_id == 1u);
}

TEST_CASE("PlaceFirstPiece: placed fragment has identity transform",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  steamrot::logic::action::grimoire_machina::PlaceFirstPiece(grimoire_machina,
                                                             mr_ghost);

  REQUIRE(std::get<steamrot::FragmentInstance>(
              grimoire_machina.m_scaffold_form->parts.at(0))
              .transform == sf::Transform::Identity);
}

TEST_CASE("PlaceFirstPiece: placed joint has identity transform",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"joint"};

  steamrot::logic::action::grimoire_machina::PlaceFirstPiece(grimoire_machina,
                                                             mr_ghost);

  REQUIRE(std::get<steamrot::JointInstance>(
              grimoire_machina.m_scaffold_form->parts.at(0))
              .transform == sf::Transform::Identity);
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

  steamrot::logic::action::grimoire_machina::PlaceFirstPiece(grimoire_machina,
                                                             mr_ghost);
  steamrot::logic::action::grimoire_machina::PlaceFirstPiece(grimoire_machina,
                                                             mr_ghost);
  steamrot::logic::action::grimoire_machina::PlaceFirstPiece(grimoire_machina,
                                                             mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
  REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(
      grimoire_machina.m_scaffold_form->parts.at(0)));
}

/////////////////////////////////////////////////
/// ProcessLogicEvents tests
/////////////////////////////////////////////////

TEST_CASE("ProcessLogicEvents: active INITIATE subscriber initialises "
          "scaffold",
          "[unit][actions][grimoire_machina][ProcessLogicEvents]") {
  steamrot::GrimoireMachina grimoire_machina;

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->event_type = steamrot::EventType::LOGIC;
  subscriber->captured_payload = steamrot::LogicPayload{
      steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD};

  steamrot::logic::action::grimoire_machina::ProcessLogicEvents(
      *subscriber, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
}

TEST_CASE("ProcessLogicEvents: active CLEAR subscriber clears existing "
          "scaffold",
          "[unit][actions][grimoire_machina][ProcessLogicEvents]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->event_type = steamrot::EventType::LOGIC;
  subscriber->captured_payload = steamrot::LogicPayload{
      steamrot::LogicPayload::LogicToggle::CLEAR_MACHINA_FORM_SCAFFOLD};

  steamrot::logic::action::grimoire_machina::ProcessLogicEvents(
      *subscriber, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
}

/////////////////////////////////////////////////
/// ToggleSocketVisibility tests
/////////////////////////////////////////////////

TEST_CASE(
    "ToggleSocketVisibility toggles are_sockets_visible from false to true",
    "[unit][actions][grimoire_machina][ToggleSocketVisibility]") {
  steamrot::MachinaFormScaffold scaffold;
  REQUIRE(scaffold.are_sockets_visible == false);

  steamrot::logic::action::grimoire_machina::ToggleSocketVisibility(scaffold);

  REQUIRE(scaffold.are_sockets_visible == true);
}

TEST_CASE("ToggleSocketVisibility toggles are_sockets_visible from true to "
          "false",
          "[unit][actions][grimoire_machina][ToggleSocketVisibility]") {
  steamrot::MachinaFormScaffold scaffold;
  scaffold.are_sockets_visible = true;

  steamrot::logic::action::grimoire_machina::ToggleSocketVisibility(scaffold);

  REQUIRE(scaffold.are_sockets_visible == false);
}

TEST_CASE("ToggleSocketVisibility: toggling twice returns to original state",
          "[unit][actions][grimoire_machina][ToggleSocketVisibility]") {
  steamrot::MachinaFormScaffold scaffold;

  steamrot::logic::action::grimoire_machina::ToggleSocketVisibility(scaffold);
  steamrot::logic::action::grimoire_machina::ToggleSocketVisibility(scaffold);

  REQUIRE(scaffold.are_sockets_visible == false);
}

/////////////////////////////////////////////////
/// ProcessUserInputEvents tests
/////////////////////////////////////////////////

TEST_CASE("ProcessUserInputEvents: missing captured_payload is ignored without "
          "crash",
          "[unit][actions][grimoire_machina][ProcessUserInputEvents]") {
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::Subscriber subscriber;
  subscriber.m_active = true;
  subscriber.event_type = steamrot::EventType::USER_INPUT;
  // no captured_payload

  REQUIRE_NOTHROW(
      steamrot::logic::action::grimoire_machina::ProcessUserInputEvents(
          subscriber, scene_context, grimoire_machina));

  REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());
}

TEST_CASE("ProcessUserInputEvents: TOGGLE_SOCKET_VISIBILITY toggles socket "
          "visibility",
          "[unit][actions][grimoire_machina][ProcessUserInputEvents]") {
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  REQUIRE(grimoire_machina.m_scaffold_form->are_sockets_visible == false);

  steamrot::Subscriber subscriber;
  subscriber.m_active = true;
  subscriber.event_type = steamrot::EventType::USER_INPUT;
  subscriber.captured_payload = steamrot::InputPayload{
      steamrot::InputPayload::InputAction::TOGGLE_SOCKET_VISIBILITY};

  steamrot::logic::action::grimoire_machina::ProcessUserInputEvents(
      subscriber, scene_context, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form->are_sockets_visible == true);
}

TEST_CASE("ProcessUserInputEvents: TOGGLE_SOCKET_VISIBILITY with no scaffold "
          "does not crash",
          "[unit][actions][grimoire_machina][ProcessUserInputEvents]") {
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::GrimoireMachina grimoire_machina;
  // no scaffold_form set

  steamrot::Subscriber subscriber;
  subscriber.m_active = true;
  subscriber.event_type = steamrot::EventType::USER_INPUT;
  subscriber.captured_payload = steamrot::InputPayload{
      steamrot::InputPayload::InputAction::TOGGLE_SOCKET_VISIBILITY};

  REQUIRE_NOTHROW(
      steamrot::logic::action::grimoire_machina::ProcessUserInputEvents(
          subscriber, scene_context, grimoire_machina));
}

TEST_CASE(
    "ProcessUserInputEvents: SELECT with valid conditions places fragment",
    "[unit][actions][grimoire_machina][ProcessUserInputEvents]") {
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};
  fixture.GetMrGhost().m_selection = steamrot::FragmentTag{"frag"};
  // scene_state.is_mouse_over_ui_layer defaults to false

  steamrot::Subscriber subscriber;
  subscriber.m_active = true;
  subscriber.event_type = steamrot::EventType::USER_INPUT;
  subscriber.captured_payload =
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};

  steamrot::logic::action::grimoire_machina::ProcessUserInputEvents(
      subscriber, scene_context, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
  REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(
      grimoire_machina.m_scaffold_form->parts.at(0)));
}

TEST_CASE("ProcessUserInputEvents: SELECT with valid conditions places joint",
          "[unit][actions][grimoire_machina][ProcessUserInputEvents]") {
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};
  fixture.GetMrGhost().m_selection = steamrot::JointTag{"joint"};

  steamrot::Subscriber subscriber;
  subscriber.m_active = true;
  subscriber.event_type = steamrot::EventType::USER_INPUT;
  subscriber.captured_payload =
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};

  steamrot::logic::action::grimoire_machina::ProcessUserInputEvents(
      subscriber, scene_context, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
  REQUIRE(std::holds_alternative<steamrot::JointInstance>(
      grimoire_machina.m_scaffold_form->parts.at(0)));
}

TEST_CASE("ProcessUserInputEvents: SELECT with monostate ghost does not place",
          "[unit][actions][grimoire_machina][ProcessUserInputEvents]") {
  steamrot::tests::TestFixture fixture;
  steamrot::SceneContext &scene_context = fixture.GetSceneContext();

  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  // mr_ghost has default monostate selection

  steamrot::Subscriber subscriber;
  subscriber.m_active = true;
  subscriber.event_type = steamrot::EventType::USER_INPUT;
  subscriber.captured_payload =
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};

  steamrot::logic::action::grimoire_machina::ProcessUserInputEvents(
      subscriber, scene_context, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());
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
