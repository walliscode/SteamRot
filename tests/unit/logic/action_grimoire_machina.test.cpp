/////////////////////////////////////////////////
/// @file
/// @brief unit tests for the GrimoireMachina action processing functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_grimoire_machina.h"
#include "MachinaFormScaffold.h"
#include <catch2/catch_test_macros.hpp>

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

TEST_CASE("FragmentInstance constructor creates one socket state per Fragment socket",
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
  joint.sockets = {{5.f, 10.f}, {15.f, 25.f}};

  steamrot::JointInstance instance{joint};

  REQUIRE(instance.socket_states.size() == 2);
}

TEST_CASE("JointInstance constructor stores the Joint reference",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.name = "test_joint";
  joint.sockets = {{0.f, 0.f}};

  steamrot::JointInstance instance{joint};

  REQUIRE(&instance.joint == &joint);
}

TEST_CASE("JointInstance constructor stores the initial transform",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.sockets = {{0.f, 0.f}};

  sf::Transform transform;
  transform.translate({50.f, 75.f});

  steamrot::JointInstance instance{joint, transform};

  REQUIRE(instance.transform == transform);
}

TEST_CASE("JointInstance constructor with no sockets creates empty "
          "socket_states vector",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.sockets = {};

  steamrot::JointInstance instance{joint};

  REQUIRE(instance.socket_states.empty());
}

TEST_CASE("JointInstance constructor initialises socket states to Available",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.sockets = {{5.f, 10.f}, {15.f, 25.f}};

  steamrot::JointInstance instance{joint};

  REQUIRE(instance.socket_states[0].state ==
          steamrot::SocketState::State::Available);
  REQUIRE(instance.socket_states[1].state ==
          steamrot::SocketState::State::Available);
}

TEST_CASE("JointInstance id defaults to zero",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.sockets = {{0.f, 0.f}};

  steamrot::JointInstance instance{joint};

  REQUIRE(instance.id == 0u);
}
