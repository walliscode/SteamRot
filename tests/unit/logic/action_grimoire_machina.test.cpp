/////////////////////////////////////////////////
/// @file
/// @brief unit tests for the GrimoireMachina action processing functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_grimoire_machina.h"
#include "MachinaFormScaffold.h"
#include "Vector2fEqualsMatcher.h"
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
  REQUIRE(result.error().message ==
          "PlaceGhostOnScaffold: no ghost item selected");
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
  REQUIRE(result.error().message ==
          "PlaceGhostOnScaffold: fragment key not found");
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
  REQUIRE(result.error().message ==
          "PlaceGhostOnScaffold: joint key not found");
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
      grimoire_machina, mr_ghost, {100.f, 80.f}, /*is_first_piece=*/true);

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
      grimoire_machina, mr_ghost, {100.f, 80.f}, true);

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
      grimoire_machina, mr_ghost, world_pos, true);
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
      grimoire_machina, mr_ghost, {50.f, 50.f}, true);

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
      grimoire_machina, mr_ghost, {50.f, 50.f}, true);

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
      grimoire_machina, mr_ghost, world_pos, true);
  if (!result.has_value()) {
    FAIL("PlaceGhostOnScaffold failed with error: " + result.error().message);
  }

  const sf::Transform &t =
      grimoire_machina.m_scaffold_form->joints[0].transform;
  const sf::Vector2f mapped = t.transformPoint({0.f, 0.f});

  REQUIRE_THAT(mapped, steamrot::tests::EqualsVector2f(world_pos));
}

/////////////////////////////////////////////////
/// PlaceGhostOnScaffold subsequent-piece placement tests
/////////////////////////////////////////////////

TEST_CASE(
    "PlaceGhostOnScaffold subsequent piece: fragment uses ghost anchor with "
    "empty bounds",
    "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  // With empty bounds, ghost anchor formula:
  // translate(world_pos - bounds.position - bounds.size - {5,5})
  // = translate(world_pos - {0,0} - {0,0} - {5,5}) = translate(world_pos -
  // {5,5}). So transformPoint({0,0}) == world_pos - {5,5}.
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};

  const sf::Vector2f world_pos{60.f, 40.f};
  // is_first_piece defaults to false
  auto result = steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost, world_pos);

  if (!result.has_value()) {
    FAIL("PlaceGhostOnScaffold failed with error: " + result.error().message);
  }
  const sf::Transform &t =
      grimoire_machina.m_scaffold_form->fragments[0].transform;
  const sf::Vector2f mapped = t.transformPoint({0.f, 0.f});

  REQUIRE_THAT(mapped, steamrot::tests::EqualsVector2f(
                           {world_pos.x - 5.f, world_pos.y - 5.f}));
}

TEST_CASE("PlaceGhostOnScaffold subsequent piece: joint uses ghost anchor with "
          "empty bounds",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_selection = steamrot::JointTag{"joint"};

  const sf::Vector2f world_pos{80.f, 60.f};
  auto result = steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
      grimoire_machina, mr_ghost, world_pos);

  if (!result.has_value()) {
    FAIL("PlaceGhostOnScaffold failed with error: " + result.error().message);
  }
  const sf::Transform &t =
      grimoire_machina.m_scaffold_form->joints[0].transform;
  const sf::Vector2f mapped = t.transformPoint({0.f, 0.f});

  REQUIRE_THAT(mapped, steamrot::tests::EqualsVector2f(
                           {world_pos.x - 5.f, world_pos.y - 5.f}));
}

/////////////////////////////////////////////////
/// PlaceGhostOnScaffold ID assignment tests
/////////////////////////////////////////////////

TEST_CASE(
    "PlaceGhostOnScaffold assigns incrementing IDs across mixed placements",
    "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};

  steamrot::MrGhost mr_ghost;

  // First placement: fragment as first piece → id 0
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};
  auto frag_result =
      steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
          grimoire_machina, mr_ghost, {10.f, 10.f}, true);
  if (!frag_result.has_value()) {
    FAIL("PlaceGhostOnScaffold failed with error: " +
         frag_result.error().message);
  }

  // Second placement: joint as subsequent piece → id 1
  mr_ghost.m_selection = steamrot::JointTag{"joint"};
  auto joint_result =
      steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
          grimoire_machina, mr_ghost, {20.f, 20.f});
  if (!joint_result.has_value()) {
    FAIL("PlaceGhostOnScaffold failed with error: " +
         joint_result.error().message);
  }

  // Third placement: fragment as subsequent piece → id 2
  mr_ghost.m_selection = steamrot::FragmentTag{"frag"};
  auto third_result =
      steamrot::logic::action::grimoire_machina::PlaceGhostOnScaffold(
          grimoire_machina, mr_ghost, {30.f, 30.f});

  if (!third_result.has_value()) {
    FAIL("PlaceGhostOnScaffold failed with error: " +
         third_result.error().message);
  }

  REQUIRE(grimoire_machina.m_scaffold_form->fragments[0].id == 0u);
  REQUIRE(grimoire_machina.m_scaffold_form->joints[0].id == 1u);
  REQUIRE(grimoire_machina.m_scaffold_form->fragments[1].id == 2u);
  REQUIRE(grimoire_machina.m_scaffold_form->next_id == 3u);
}
