//
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
#include "PartGraphBuilder.h"
#include "SocketState.h"
#include "Subscriber.h"
#include "TestFixture.h"
#include "joint_library.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

namespace steamrot::tests {

using namespace logic::action::grimoire_machina;
namespace {

/////////////////////////////////////////////////
/// @brief Require that no socket in any part of @p scaffold has
/// @c connected_to set. Used by place_next_piece guard tests to verify that
/// a rejected placement left the scaffold unmodified.
/////////////////////////////////////////////////
void require_no_connections(const steamrot::MachinaFormScaffold &scaffold) {
  for (const auto &[part_id, variant] : scaffold.parts) {
    std::visit(
        [](const auto &instance) {
          for (const auto &[sid, socket] : instance.sockets)
            REQUIRE_FALSE(socket.connected_to.has_value());
        },
        variant);
  }
}

} // namespace

TEST_CASE("InitialiseActiveMachinaForm adds a new MachinaForm to the "
          "GrimoireMachina active form",

          "[GrimoireMachina]") {
  steamrot::GrimoireMachina grimoire_machina;

  steamrot::logic::action::grimoire_machina::
      initialise_active_machina_form_scaffold(grimoire_machina);
  REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
}

TEST_CASE("ClearActiveMachinaForm clears the active MachinaForm in the "
          "GrimoireMachina",
          "[GrimoireMachina]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
  steamrot::logic::action::grimoire_machina::clear_active_machina_form_scaffold(
      grimoire_machina);
  REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
}

TEST_CASE("GetAllFragmentNames returns the string names of all fragments in "
          "the GrimoireMachina",
          "[unit][actions][grimoire_machina]") {
  steamrot::GrimoireMachina grimoire_machina;
  SECTION("No fragments in GrimoireMachina") {
    auto fragment_names =
        steamrot::logic::action::grimoire_machina::get_all_fragment_names(
            grimoire_machina);
    REQUIRE(fragment_names.empty());
  }

  SECTION("Multiple fragments in GrimoireMachina") {
    grimoire_machina.m_all_fragments = {{"Fragment1", steamrot::Fragment{}},
                                        {"Fragment2", steamrot::Fragment{}},
                                        {"Fragment3", steamrot::Fragment{}}};
    auto fragment_names =
        steamrot::logic::action::grimoire_machina::get_all_fragment_names(
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
        steamrot::logic::action::grimoire_machina::get_all_joint_names(
            grimoire_machina);
    REQUIRE(joint_names.empty());
  }

  SECTION("Multiple joints in GrimoireMachina") {
    grimoire_machina.m_all_joints = {{"Joint1", steamrot::Joint{}},
                                     {"Joint2", steamrot::Joint{}},
                                     {"Joint3", steamrot::Joint{}}};
    auto joint_names =
        steamrot::logic::action::grimoire_machina::get_all_joint_names(
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
  steamrot::SocketState socket_data;
  REQUIRE(socket_data.connection_state ==
          steamrot::SocketConnectionState::Available);
}

TEST_CASE("SocketState has is_mouse_over false by default",
          "[unit][SocketState][MachinaFormScaffold]") {

  steamrot::SocketState socket_data;
  REQUIRE(socket_data.is_mouse_over == false);
}

/////////////////////////////////////////////////
/// FragmentInstance constructor tests
/////////////////////////////////////////////////

TEST_CASE("FragmentInstance constructor creates one socket per Fragment socket",
          "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;
  SocketData socket1{{10.f, 20.f}, {1.f, 0.f}};
  SocketData socket2{{30.f, 40.f}, {0.f, 1.f}};
  SocketData socket3{{50.f, 60.f}, {-1.f, 0.f}};

  fragment.sockets.push_back(socket1);
  fragment.sockets.push_back(socket2);
  fragment.sockets.push_back(socket3);

  steamrot::FragmentInstance instance{&fragment};

  REQUIRE(instance.sockets.size() == 3);
}

TEST_CASE("FragmentInstance constructor stores the Fragment reference",
          "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;
  fragment.name = "test_fragment";
  SocketData socket{{0.f, 0.f}, {1.f, 0.f}};
  fragment.sockets.push_back(socket);

  steamrot::FragmentInstance instance{&fragment};

  REQUIRE(instance.fragment == &fragment);
}

TEST_CASE("FragmentInstance constructor stores the initial transform",
          "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;
  SocketData socket{{0.f, 0.f}, {1.f, 0.f}};
  fragment.sockets.push_back(socket);

  sf::Transform transform;
  transform.translate({50.f, 75.f});

  steamrot::FragmentInstance instance{&fragment, transform};

  REQUIRE(instance.transform == transform);
}

TEST_CASE("FragmentInstance constructor with no sockets creates empty "
          "sockets vector",
          "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;

  steamrot::FragmentInstance instance{&fragment};

  REQUIRE(instance.sockets.empty());
}

TEST_CASE("FragmentInstance constructor initialises socket states to Available",
          "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;
  SocketData socket1{{10.f, 20.f}, {1.f, 0.f}};
  SocketData socket2{{30.f, 40.f}, {0.f, 1.f}};
  fragment.sockets.push_back(socket1);
  fragment.sockets.push_back(socket2);

  steamrot::FragmentInstance instance{&fragment};

  REQUIRE(instance.sockets.at(0).connection_state ==
          steamrot::SocketConnectionState::Available);
  REQUIRE(instance.sockets.at(1).connection_state ==
          steamrot::SocketConnectionState::Available);
}

TEST_CASE("FragmentInstance id defaults to zero",
          "[unit][FragmentInstance][MachinaFormScaffold]") {
  steamrot::Fragment fragment;
  SocketData socket{{0.f, 0.f}, {1.f, 0.f}};
  fragment.sockets.push_back(socket);

  steamrot::FragmentInstance instance{&fragment};

  REQUIRE(instance.id == 0u);
}

/////////////////////////////////////////////////
/// JointInstance constructor tests
/////////////////////////////////////////////////

TEST_CASE("JointInstance constructor creates one socket per Joint socket",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 2;

  steamrot::JointInstance instance{&joint};

  REQUIRE(instance.sockets.size() == 2);
}

TEST_CASE("JointInstance constructor stores the Joint reference",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.name = "test_joint";
  joint.socket_config.socket_count = 1;

  steamrot::JointInstance instance{&joint};

  REQUIRE(instance.joint == &joint);
}

TEST_CASE("JointInstance constructor stores the initial transform",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 1;

  sf::Transform transform;
  transform.translate({50.f, 75.f});

  steamrot::JointInstance instance{&joint, transform};

  REQUIRE(instance.transform == transform);
}

TEST_CASE("JointInstance constructor with socket_count zero creates empty "
          "sockets vector",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 0;

  steamrot::JointInstance instance{&joint};

  REQUIRE(instance.sockets.empty());
}

TEST_CASE("JointInstance constructor initialises socket states to Available",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 2;

  steamrot::JointInstance instance{&joint};

  REQUIRE(instance.sockets.at(0).connection_state ==
          steamrot::SocketConnectionState::Available);
  REQUIRE(instance.sockets.at(1).connection_state ==
          steamrot::SocketConnectionState::Available);
}

TEST_CASE("JointInstance id defaults to zero",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 1;

  steamrot::JointInstance instance{&joint};

  REQUIRE(instance.id == 0u);
}

TEST_CASE("JointInstance constructor populates sockets for each socket",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 3;
  joint.socket_config.radius = 10.f;
  joint.socket_config.rotation_arc_min = 0.f;
  joint.socket_config.rotation_arc_max = 360.f;
  joint.socket_config.has_fixed_socket = false;

  steamrot::JointInstance instance{&joint};

  REQUIRE(instance.sockets.size() == 3u);
}

TEST_CASE("JointInstance constructor sockets size matches socket_count",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 4;
  joint.socket_config.radius = 20.f;
  joint.socket_config.rotation_arc_min = 0.f;
  joint.socket_config.rotation_arc_max = 360.f;
  joint.socket_config.has_fixed_socket = false;

  steamrot::JointInstance instance{&joint};

  REQUIRE(instance.sockets.size() ==
          static_cast<size_t>(joint.socket_config.socket_count));
}

TEST_CASE(
    "JointInstance constructor with zero sockets has empty sockets vector",
    "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 0;

  steamrot::JointInstance instance{&joint};

  REQUIRE(instance.sockets.empty());
}

TEST_CASE("JointInstance constructor zero-initialises socket positions",
          "[unit][JointInstance][MachinaFormScaffold]") {
  steamrot::Joint joint;
  joint.socket_config.socket_count = 3;
  joint.socket_config.radius = 10.f;
  joint.socket_config.rotation_arc_min = 0.f;
  joint.socket_config.rotation_arc_max = 360.f;
  joint.socket_config.has_fixed_socket = false;

  steamrot::JointInstance instance{&joint};

  // Positions are zero-initialised at construction; call
  // initialize_joint_socket_positions() to populate them.
  REQUIRE(instance.sockets.at(0).local_position.x == 0.f);
  REQUIRE(instance.sockets.at(0).local_position.y == 0.f);
  REQUIRE(instance.sockets.at(1).local_position.x == 0.f);
  REQUIRE(instance.sockets.at(1).local_position.y == 0.f);
  REQUIRE(instance.sockets.at(2).local_position.x == 0.f);
  REQUIRE(instance.sockets.at(2).local_position.y == 0.f);
}

/////////////////////////////////////////////////
/// PlaceGhostOnScaffold guard tests
/////////////////////////////////////////////////

TEST_CASE("PlaceGhostOnScaffold does nothing when no scaffold is active",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance =
      steamrot::FragmentInstance{&grimoire_machina.m_all_fragments["frag"]};

  REQUIRE_NOTHROW(
      steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
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

  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
      grimoire_machina, mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());
}

TEST_CASE("PlaceGhostOnScaffold does nothing when fragment pointer is null",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::FragmentInstance{nullptr};

  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
      grimoire_machina, mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());
}

TEST_CASE("PlaceGhostOnScaffold does nothing when joint pointer is null",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::JointInstance{nullptr};

  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
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
  mr_ghost.m_instance =
      steamrot::FragmentInstance{&grimoire_machina.m_all_fragments["frag"]};

  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
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
  mr_ghost.m_instance =
      steamrot::FragmentInstance{&grimoire_machina.m_all_fragments["frag"]};

  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
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
  mr_ghost.m_instance =
      steamrot::FragmentInstance{&grimoire_machina.m_all_fragments["frag"]};

  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
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
  mr_ghost.m_instance =
      steamrot::JointInstance{&grimoire_machina.m_all_joints["joint"]};

  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
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
  mr_ghost.m_instance =
      steamrot::JointInstance{&grimoire_machina.m_all_joints["joint"]};

  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
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
  mr_ghost.m_instance =
      steamrot::JointInstance{&grimoire_machina.m_all_joints["joint"]};

  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
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
  mr_ghost.m_instance =
      steamrot::FragmentInstance{&grimoire_machina.m_all_fragments["frag"]};

  // Place the first piece successfully.
  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
      grimoire_machina, mr_ghost);
  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);

  // A second placement must not add any more pieces (no collision logic yet).
  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
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
  mr_ghost.m_instance =
      steamrot::JointInstance{&grimoire_machina.m_all_joints["joint"]};

  // Place the first piece successfully.
  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
      grimoire_machina, mr_ghost);
  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);

  // A second placement must not add any more pieces (no collision logic yet).
  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
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
  mr_ghost.m_instance =
      steamrot::FragmentInstance{&grimoire_machina.m_all_fragments["frag"]};

  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
      grimoire_machina, mr_ghost);
  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
      grimoire_machina, mr_ghost);
  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
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
  mr_ghost.m_instance =
      steamrot::JointInstance{&grimoire_machina.m_all_joints["joint"]};

  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
      grimoire_machina, mr_ghost);
  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
      grimoire_machina, mr_ghost);
  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
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
  mr_ghost.m_instance =
      steamrot::FragmentInstance{&grimoire_machina.m_all_fragments["frag"]};

  REQUIRE_NOTHROW(steamrot::logic::action::grimoire_machina::place_first_piece(
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
  mr_ghost.m_instance =
      steamrot::FragmentInstance{&grimoire_machina.m_all_fragments["frag"]};

  // Place the first piece.
  steamrot::logic::action::grimoire_machina::place_first_piece(grimoire_machina,
                                                               mr_ghost);
  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);

  // A second call must be ignored.
  steamrot::logic::action::grimoire_machina::place_first_piece(grimoire_machina,
                                                               mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
}

TEST_CASE("PlaceFirstPiece does nothing when ghost selection is monostate",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost mr_ghost; // default selection = monostate

  steamrot::logic::action::grimoire_machina::place_first_piece(grimoire_machina,
                                                               mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());
}

TEST_CASE("PlaceFirstPiece does nothing when fragment pointer is null",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::FragmentInstance{nullptr};

  steamrot::logic::action::grimoire_machina::place_first_piece(grimoire_machina,
                                                               mr_ghost);

  REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());
}

TEST_CASE("PlaceFirstPiece does nothing when joint pointer is null",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance = steamrot::JointInstance{nullptr};

  steamrot::logic::action::grimoire_machina::place_first_piece(grimoire_machina,
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
  mr_ghost.m_instance =
      steamrot::FragmentInstance{&grimoire_machina.m_all_fragments["frag"]};

  steamrot::logic::action::grimoire_machina::place_first_piece(grimoire_machina,
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
  mr_ghost.m_instance =
      steamrot::JointInstance{&grimoire_machina.m_all_joints["joint"]};

  steamrot::logic::action::grimoire_machina::place_first_piece(grimoire_machina,
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
  mr_ghost.m_instance =
      steamrot::FragmentInstance{&grimoire_machina.m_all_fragments["frag"]};

  steamrot::logic::action::grimoire_machina::place_first_piece(grimoire_machina,
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
  mr_ghost.m_instance =
      steamrot::JointInstance{&grimoire_machina.m_all_joints["joint"]};

  steamrot::logic::action::grimoire_machina::place_first_piece(grimoire_machina,
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
  mr_ghost.m_instance =
      steamrot::FragmentInstance{&grimoire_machina.m_all_fragments["frag"]};

  steamrot::logic::action::grimoire_machina::place_first_piece(grimoire_machina,
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
  mr_ghost.m_instance =
      steamrot::JointInstance{&grimoire_machina.m_all_joints["joint"]};

  steamrot::logic::action::grimoire_machina::place_first_piece(grimoire_machina,
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
  mr_ghost.m_instance =
      steamrot::FragmentInstance{&grimoire_machina.m_all_fragments["frag"]};

  steamrot::logic::action::grimoire_machina::place_first_piece(grimoire_machina,
                                                               mr_ghost);
  steamrot::logic::action::grimoire_machina::place_first_piece(grimoire_machina,
                                                               mr_ghost);
  steamrot::logic::action::grimoire_machina::place_first_piece(grimoire_machina,
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

  steamrot::logic::action::grimoire_machina::process_logic_events(
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

  steamrot::logic::action::grimoire_machina::process_logic_events(
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

  steamrot::logic::action::grimoire_machina::toggle_socket_visibility(scaffold);

  REQUIRE(scaffold.are_sockets_visible == true);
}

TEST_CASE("ToggleSocketVisibility toggles are_sockets_visible from true to "
          "false",
          "[unit][actions][grimoire_machina][ToggleSocketVisibility]") {
  steamrot::MachinaFormScaffold scaffold;
  scaffold.are_sockets_visible = true;

  steamrot::logic::action::grimoire_machina::toggle_socket_visibility(scaffold);

  REQUIRE(scaffold.are_sockets_visible == false);
}

TEST_CASE("ToggleSocketVisibility: toggling twice returns to original state",
          "[unit][actions][grimoire_machina][ToggleSocketVisibility]") {
  steamrot::MachinaFormScaffold scaffold;

  steamrot::logic::action::grimoire_machina::toggle_socket_visibility(scaffold);
  steamrot::logic::action::grimoire_machina::toggle_socket_visibility(scaffold);

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
      steamrot::logic::action::grimoire_machina::proces_user_input_events(
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

  steamrot::logic::action::grimoire_machina::proces_user_input_events(
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
      steamrot::logic::action::grimoire_machina::proces_user_input_events(
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
  fixture.GetMrGhost().m_instance =
      steamrot::FragmentInstance{&grimoire_machina.m_all_fragments["frag"]};
  // scene_state.is_mouse_over_ui_layer defaults to false

  steamrot::Subscriber subscriber;
  subscriber.m_active = true;
  subscriber.event_type = steamrot::EventType::USER_INPUT;
  subscriber.captured_payload =
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};

  steamrot::logic::action::grimoire_machina::proces_user_input_events(
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
  fixture.GetMrGhost().m_instance =
      steamrot::JointInstance{&grimoire_machina.m_all_joints["joint"]};

  steamrot::Subscriber subscriber;
  subscriber.m_active = true;
  subscriber.event_type = steamrot::EventType::USER_INPUT;
  subscriber.captured_payload =
      steamrot::InputPayload{steamrot::InputPayload::InputAction::SELECT};

  steamrot::logic::action::grimoire_machina::proces_user_input_events(
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

  steamrot::logic::action::grimoire_machina::proces_user_input_events(
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
  steamrot::logic::action::grimoire_machina::process_subscribers(
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
  steamrot::logic::action::grimoire_machina::process_subscribers(
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
  steamrot::logic::action::grimoire_machina::process_subscribers(
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
  steamrot::logic::action::grimoire_machina::process_subscribers(
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
  steamrot::logic::action::grimoire_machina::process_subscribers(
      subscribers, scene_context, grimoire_machina);

  REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
  REQUIRE(grimoire_machina.m_scaffold_form->are_sockets_visible == true);
}

TEST_CASE("create_connection tests",
          "[unit][actions][grimoire_machina][create_connection]") {

  steamrot::tests::PartGraphBuilder builder;

  SECTION("create_connection returns error when both parts have no "
          "sockets") {
    steamrot::FragmentInstance frag_instance =
        builder.MakeFragmentInstance(steamrot::tests::FragmentNames::NoSocket);

    steamrot::JointInstance joint_instance =
        builder.MakeJointInstance(steamrot::tests::JointNames::NoSocket);

    auto connection_result =
        steamrot::logic::action::grimoire_machina::create_connection(
            frag_instance, 0, joint_instance, 3);

    REQUIRE_FALSE(connection_result.has_value());
    REQUIRE(connection_result.error() ==
            "Connection creation failed: both parts have no sockets.");
  }

  SECTION("create_connection returns error when socket indices are out of "
          "range") {
    steamrot::FragmentInstance frag_instance =
        builder.MakeFragmentInstance(steamrot::tests::FragmentNames::OneSocket);
    steamrot::JointInstance joint_instance =
        builder.MakeJointInstance(steamrot::tests::JointNames::TwoSockets);
    auto connection_result =
        steamrot::logic::action::grimoire_machina::create_connection(
            frag_instance, 5, joint_instance, 5);
    REQUIRE_FALSE(connection_result.has_value());
    REQUIRE(
        connection_result.error() ==
        "Connection creation failed: one or both socket IDs are not found.");
  }

  SECTION("create_connection returns a valid result when given valid "
          "inputs") {
    steamrot::FragmentInstance frag_instance =
        builder.MakeFragmentInstance(steamrot::tests::FragmentNames::OneSocket);
    steamrot::JointInstance joint_instance =
        builder.MakeJointInstance(steamrot::tests::JointNames::TwoSockets);
    auto connection_result =
        steamrot::logic::action::grimoire_machina::create_connection(
            frag_instance, 0, joint_instance, 1);
    REQUIRE(connection_result.has_value());

    REQUIRE(frag_instance.sockets.at(0).connected_to.has_value());
    REQUIRE(frag_instance.sockets.at(0).connected_to->peer_part_id ==
            joint_instance.id);
    REQUIRE(frag_instance.sockets.at(0).connected_to->peer_socket_id == 1u);

    REQUIRE(joint_instance.sockets.at(1).connected_to.has_value());
    REQUIRE(joint_instance.sockets.at(1).connected_to->peer_part_id ==
            frag_instance.id);
    REQUIRE(joint_instance.sockets.at(1).connected_to->peer_socket_id == 0u);
  }

  SECTION("create_connection changes SocketState on connected sockets to "
          "SocketStatae::Connected") {
    steamrot::FragmentInstance frag_instance =
        builder.MakeFragmentInstance(steamrot::tests::FragmentNames::OneSocket);
    steamrot::JointInstance joint_instance =
        builder.MakeJointInstance(steamrot::tests::JointNames::TwoSockets);
    REQUIRE(frag_instance.sockets.at(0).connection_state ==
            steamrot::SocketConnectionState::Available);
    REQUIRE(joint_instance.sockets.at(1).connection_state ==
            steamrot::SocketConnectionState::Available);
    auto connection_result =
        steamrot::logic::action::grimoire_machina::create_connection(
            frag_instance, 0, joint_instance, 1);
    REQUIRE(connection_result.has_value());
    REQUIRE(frag_instance.sockets.at(0).connection_state ==
            steamrot::SocketConnectionState::Connected);
    REQUIRE(joint_instance.sockets.at(1).connection_state ==
            steamrot::SocketConnectionState::Connected);
  }
}

TEST_CASE("check_socket_for_connection_readiness tests",
          "[unit][actions][grimoire_machina][check_socket_for_connection_"
          "readiness]") {

  steamrot::tests::PartGraphBuilder builder;

  SECTION("check_socket_for_connection_readiness returns false for state != "
          "Available and is_ready_to_connect is false") {
    steamrot::FragmentInstance frag_instance =
        builder.MakeFragmentInstance(steamrot::tests::FragmentNames::OneSocket);
    // manually set the only socket to occupied
    frag_instance.sockets.at(0).connection_state =
        steamrot::SocketConnectionState::Connected;
    frag_instance.sockets.at(0).is_ready_to_connect = false;

    auto result = steamrot::logic::action::grimoire_machina::
        check_socket_for_connection_readiness(frag_instance.sockets.at(0));
    REQUIRE_FALSE(result);
  }

  SECTION("check_socket_for_connection_readiness returns false for state != "
          "Available but is_ready_to_connect is true") {
    steamrot::FragmentInstance frag_instance =
        builder.MakeFragmentInstance(steamrot::tests::FragmentNames::OneSocket);
    // manually set the only socket to occupied but ready
    frag_instance.sockets.at(0).connection_state =
        steamrot::SocketConnectionState::Connected;
    frag_instance.sockets.at(0).is_ready_to_connect = true;
    auto result = steamrot::logic::action::grimoire_machina::
        check_socket_for_connection_readiness(frag_instance.sockets.at(0));
    REQUIRE_FALSE(result);
  }

  SECTION("check_socket_for_connection_readiness returns false for state == "
          "Available but is_ready_to_connect is false") {
    steamrot::FragmentInstance frag_instance =
        builder.MakeFragmentInstance(steamrot::tests::FragmentNames::OneSocket);
    // ensure the only socket is available but not ready
    frag_instance.sockets.at(0).connection_state =
        steamrot::SocketConnectionState::Available;
    frag_instance.sockets.at(0).is_ready_to_connect = false;
    auto result = steamrot::logic::action::grimoire_machina::
        check_socket_for_connection_readiness(frag_instance.sockets.at(0));
    REQUIRE_FALSE(result);
  }

  SECTION("check_socket_for_connection_readiness returns true for state == "
          "Available and is_ready_to_connect == true") {
    steamrot::FragmentInstance frag_instance =
        builder.MakeFragmentInstance(steamrot::tests::FragmentNames::OneSocket);
    // ensure the only socket is available and ready
    frag_instance.sockets.at(0).connection_state =
        steamrot::SocketConnectionState::Available;
    frag_instance.sockets.at(0).is_ready_to_connect = true;
    auto result = steamrot::logic::action::grimoire_machina::
        check_socket_for_connection_readiness(frag_instance.sockets.at(0));
    REQUIRE(result);
  }
}
TEST_CASE("check_MrGhost_for_connection_readiness tests",
          "[unit][actions][grimoire_machina][check_MrGhost_for_connection_"
          "readiness]") {
  steamrot::tests::PartGraphBuilder builder;

  steamrot::MrGhost mr_ghost; // default monostate selection
  REQUIRE(std::holds_alternative<std::monostate>(mr_ghost.m_instance));

  SECTION("check_MrGhost_for_connection_readiness returns false for monostate "
          "selection") {

    auto result = steamrot::logic::action::grimoire_machina::
        check_MrGhost_for_connection_readiness(mr_ghost);
    REQUIRE_FALSE(result);
  }

  SECTION("check_MrGhost_for_connection_readiness returns false for fragment "
          "with no sockets") {
    steamrot::FragmentInstance frag_instance =
        builder.MakeFragmentInstance(steamrot::tests::FragmentNames::NoSocket);
    steamrot::MrGhost mr_ghost;
    mr_ghost.m_instance = frag_instance;
    auto result = steamrot::logic::action::grimoire_machina::
        check_MrGhost_for_connection_readiness(mr_ghost);
    REQUIRE_FALSE(result);
  }

  SECTION("check_MrGhost_for_connection_readiness returns false for joint with "
          "no sockets") {
    steamrot::JointInstance joint_instance =
        builder.MakeJointInstance(steamrot::tests::JointNames::NoSocket);
    steamrot::MrGhost mr_ghost;
    mr_ghost.m_instance = joint_instance;
    auto result = steamrot::logic::action::grimoire_machina::
        check_MrGhost_for_connection_readiness(mr_ghost);
    REQUIRE_FALSE(result);
  }
  SECTION("check_MrGhost_for_connection_readiness returns false for fragment "
          "with state != Available") {
    steamrot::FragmentInstance frag_instance =
        builder.MakeFragmentInstance(steamrot::tests::FragmentNames::OneSocket);
    // manually set the only socket to occupied
    frag_instance.sockets.at(0).connection_state =
        steamrot::SocketConnectionState::Connected;

    steamrot::MrGhost mr_ghost;
    mr_ghost.m_instance = frag_instance;
    auto result = steamrot::logic::action::grimoire_machina::
        check_MrGhost_for_connection_readiness(mr_ghost);
    REQUIRE_FALSE(result);
  }

  SECTION("check_MrGhost_for_connection_readiness returns false for joint with "
          "state != Available") {
    steamrot::JointInstance joint_instance =
        builder.MakeJointInstance(steamrot::tests::JointNames::OneSocket);
    // manually set the only socket to occupied
    joint_instance.sockets.at(0).connection_state =
        steamrot::SocketConnectionState::Connected;
    steamrot::MrGhost mr_ghost;
    mr_ghost.m_instance = joint_instance;
    auto result = steamrot::logic::action::grimoire_machina::
        check_MrGhost_for_connection_readiness(mr_ghost);
    REQUIRE_FALSE(result);
  }
  SECTION(
      "check_MrGhost_for_connection_readiness returns true for fragment with"
      "available sockets") {
    steamrot::FragmentInstance frag_instance = builder.MakeFragmentInstance(
        steamrot::tests::FragmentNames::TwoSockets);
    frag_instance.sockets.at(1).connection_state =
        steamrot::SocketConnectionState::Available;
    frag_instance.sockets.at(1).is_ready_to_connect = true;
    steamrot::MrGhost mr_ghost;
    mr_ghost.m_instance = frag_instance;
    auto result = steamrot::logic::action::grimoire_machina::
        check_MrGhost_for_connection_readiness(mr_ghost);
    REQUIRE(result);
    REQUIRE(result.value() ==
            1); // the index of the first available and ready socket
  }

  SECTION("check_MrGhost_for_connection_readiness returns true for joint with"
          "available sockets") {
    steamrot::JointInstance joint_instance =
        builder.MakeJointInstance(steamrot::tests::JointNames::TwoSockets);
    joint_instance.sockets.at(0).connection_state =
        steamrot::SocketConnectionState::Available;
    joint_instance.sockets.at(0).is_ready_to_connect = true;
    steamrot::MrGhost mr_ghost;
    mr_ghost.m_instance = joint_instance;
    auto result = steamrot::logic::action::grimoire_machina::
        check_MrGhost_for_connection_readiness(mr_ghost);
    REQUIRE(result);
    REQUIRE(result.value() ==
            0); // the index of the first available and ready socket_data
  }
}

TEST_CASE("check_PartGraph_for_connection_readiness tests",
          "[unit][actions][grimoire_machina][check_PartGraph_for_connection_"
          "readiness]") {
  steamrot::tests::PartGraphBuilder builder;

  SECTION("check_PartGraph_for_connection_readiness returns std::nullopt for "
          "empty PartGraph") {
    steamrot::PartGraph part_graph;
    auto result = steamrot::logic::action::grimoire_machina::
        check_PartGraph_for_connection_readiness(part_graph);

    REQUIRE_FALSE(result);
  }

  SECTION(
      "check_PartGraph_for_connection_readiness returns std::nullopt when no "
      "parts have available sockets") {

    steamrot::PartGraph part_graph =
        steamrot::tests::PartGraphBuilder{}
            .AddFragmentInstance(steamrot::tests::FragmentNames::NoSocket, "f0")
            .AddJointInstance(steamrot::tests::JointNames::NoSocket, "j0")
            .Build()
            .part_graph;

    auto result = steamrot::logic::action::grimoire_machina::
        check_PartGraph_for_connection_readiness(part_graph);

    REQUIRE_FALSE(result);
  }

  SECTION("check_PartGraph_for_connection_readiness returns the id of a "
          "FragmentInstance with "
          "available sockets") {
    steamrot::PartGraph part_graph =
        steamrot::tests::PartGraphBuilder{}
            .AddFragmentInstance(steamrot::tests::FragmentNames::TwoSockets,
                                 "f0")
            .AddJointInstance(steamrot::tests::JointNames::NoSocket, "j0")
            .Build()
            .part_graph;

    // get the Fragment at id 0
    auto &part = part_graph.at(0);
    REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(part));
    auto &frag_instance = std::get<steamrot::FragmentInstance>(part);
    // set its only socket to available and ready
    frag_instance.sockets.at(1).connection_state =
        steamrot::SocketConnectionState::Available;
    frag_instance.sockets.at(1).is_ready_to_connect = true;

    auto result = steamrot::logic::action::grimoire_machina::
        check_PartGraph_for_connection_readiness(part_graph);
    REQUIRE(result);
    REQUIRE(result.value().first ==
            0); // the id of the part with available sockets
    REQUIRE(
        result.value().second ==
        1); // the index of the first available and ready socket on that part
  }

  SECTION("check_PartGraph_for_connection_readiness returns the id of a "
          "JointInstance with "
          "available sockets") {
    steamrot::PartGraph part_graph =
        steamrot::tests::PartGraphBuilder{}
            .AddFragmentInstance(steamrot::tests::FragmentNames::NoSocket, "f0")
            .AddJointInstance(parts::JointSquareWith2Sockets, "j1")
            .Build()
            .part_graph;

    // get the Joint at id 1
    auto &part = part_graph.at(1);
    REQUIRE(std::holds_alternative<steamrot::JointInstance>(part));
    auto &joint_instance = std::get<steamrot::JointInstance>(part);
    REQUIRE(joint_instance.sockets.size() == 2);

    // set its first socket to available and ready
    joint_instance.sockets.at(0).connection_state =
        steamrot::SocketConnectionState::Available;
    joint_instance.sockets.at(0).is_ready_to_connect = true;
    auto result = check_PartGraph_for_connection_readiness(part_graph);
    REQUIRE(result);
    REQUIRE(result.value().first ==
            1); // the id of the part with available sockets
    REQUIRE(
        result.value().second ==
        0); // the index of the first available and ready socket on that part
  }

  SECTION(
      "check_PartGraph_for_connection_readiness returns the id of a part with "
      "available sockets when multiple parts have available sockets") {
    steamrot::PartGraph part_graph =
        steamrot::tests::PartGraphBuilder{}
            .AddFragmentInstance(steamrot::tests::FragmentNames::TwoSockets,
                                 "f0")
            .AddFragmentInstance(steamrot::tests::FragmentNames::OneSocket,
                                 "f1")
            .AddJointInstance(steamrot::tests::JointNames::NoSocket, "j0")
            .Build()
            .part_graph;
    // set the first Fragment's socket 1 to available and ready
    auto &part0 = part_graph.at(0);
    REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(part0));
    auto &frag_instance0 = std::get<steamrot::FragmentInstance>(part0);
    frag_instance0.sockets.at(1).connection_state =
        steamrot::SocketConnectionState::Available;
    frag_instance0.sockets.at(1).is_ready_to_connect = true;
    // set the second Fragment's socket 0 to available and ready
    auto &part1 = part_graph.at(1);
    REQUIRE(std::holds_alternative<steamrot::FragmentInstance>(part1));
    auto &frag_instance1 = std::get<steamrot::FragmentInstance>(part1);
    frag_instance1.sockets.at(0).connection_state =
        steamrot::SocketConnectionState::Available;
    frag_instance1.sockets.at(0).is_ready_to_connect = true;
    auto result = steamrot::logic::action::grimoire_machina::
        check_PartGraph_for_connection_readiness(part_graph);
    REQUIRE(result);
    // the result should be the first part with available sockets, which is the
    // Fragment at id 0
    REQUIRE(result.value().first == 0);
    REQUIRE(result.value().second == 1); // socket index 1 on that Fragment
  }
}

/////////////////////////////////////////////////
/// place_next_piece guard tests
/////////////////////////////////////////////////

TEST_CASE("place_next_piece does nothing when scaffold parts map is empty",
          "[unit][actions][grimoire_machina][place_next_piece]") {
  steamrot::tests::PartGraphBuilder builder;

  steamrot::MachinaFormScaffold scaffold;
  REQUIRE(scaffold.parts.empty());

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance =
      builder.MakeFragmentInstance(steamrot::tests::FragmentNames::OneSocket);

  steamrot::logic::action::grimoire_machina::place_next_piece(scaffold,
                                                              mr_ghost);

  REQUIRE(scaffold.parts.empty());
  require_no_connections(scaffold);
}

TEST_CASE("place_next_piece does nothing when ghost instance is monostate",
          "[unit][actions][grimoire_machina][place_next_piece]") {
  steamrot::tests::PartGraphBuilder builder;

  steamrot::MachinaFormScaffold scaffold;
  auto joint =
      builder.MakeJointInstance(steamrot::tests::JointNames::OneSocket);
  joint.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  joint.sockets.at(0).is_ready_to_connect = true;
  scaffold.parts.emplace(scaffold.next_id++, joint);

  steamrot::MrGhost mr_ghost; // default monostate

  steamrot::logic::action::grimoire_machina::place_next_piece(scaffold,
                                                              mr_ghost);

  REQUIRE(scaffold.parts.size() == 1);
  require_no_connections(scaffold);
}

TEST_CASE("place_next_piece does nothing when ghost fragment pointer is null",
          "[unit][actions][grimoire_machina][place_next_piece]") {
  steamrot::tests::PartGraphBuilder builder;

  steamrot::MachinaFormScaffold scaffold;
  auto joint =
      builder.MakeJointInstance(steamrot::tests::JointNames::OneSocket);
  joint.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  joint.sockets.at(0).is_ready_to_connect = true;
  scaffold.parts.emplace(scaffold.next_id++, joint);

  steamrot::MrGhost mr_ghost;
  steamrot::FragmentInstance null_fi{nullptr};
  mr_ghost.m_instance = null_fi;

  steamrot::logic::action::grimoire_machina::place_next_piece(scaffold,
                                                              mr_ghost);

  REQUIRE(scaffold.parts.size() == 1);
  require_no_connections(scaffold);
}

TEST_CASE("place_next_piece does nothing when ghost joint pointer is null",
          "[unit][actions][grimoire_machina][place_next_piece]") {
  steamrot::tests::PartGraphBuilder builder;

  steamrot::MachinaFormScaffold scaffold;
  auto frag =
      builder.MakeFragmentInstance(steamrot::tests::FragmentNames::OneSocket);
  frag.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  frag.sockets.at(0).is_ready_to_connect = true;
  scaffold.parts.emplace(scaffold.next_id++, frag);

  steamrot::MrGhost mr_ghost;
  steamrot::JointInstance null_ji{nullptr};
  mr_ghost.m_instance = null_ji;

  steamrot::logic::action::grimoire_machina::place_next_piece(scaffold,
                                                              mr_ghost);

  REQUIRE(scaffold.parts.size() == 1);
  require_no_connections(scaffold);
}

TEST_CASE("place_next_piece does nothing when ghost has no ready sockets",
          "[unit][actions][grimoire_machina][place_next_piece]") {
  steamrot::tests::PartGraphBuilder builder;

  steamrot::MachinaFormScaffold scaffold;
  auto joint =
      builder.MakeJointInstance(steamrot::tests::JointNames::OneSocket);
  joint.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  joint.sockets.at(0).is_ready_to_connect = true;
  scaffold.parts.emplace(scaffold.next_id++, joint);

  steamrot::MrGhost mr_ghost;
  auto ghost_frag =
      builder.MakeFragmentInstance(steamrot::tests::FragmentNames::OneSocket);
  // socket is Available but NOT is_ready_to_connect
  ghost_frag.sockets.at(0).is_ready_to_connect = false;
  mr_ghost.m_instance = ghost_frag;

  steamrot::logic::action::grimoire_machina::place_next_piece(scaffold,
                                                              mr_ghost);

  REQUIRE(scaffold.parts.size() == 1);
  require_no_connections(scaffold);
}

TEST_CASE("place_next_piece does nothing when PartGraph has no ready sockets",
          "[unit][actions][grimoire_machina][place_next_piece]") {
  steamrot::tests::PartGraphBuilder builder;

  steamrot::MachinaFormScaffold scaffold;
  auto joint =
      builder.MakeJointInstance(steamrot::tests::JointNames::OneSocket);
  // socket is Available but NOT is_ready_to_connect
  joint.sockets.at(0).is_ready_to_connect = false;
  scaffold.parts.emplace(scaffold.next_id++, joint);

  steamrot::MrGhost mr_ghost;
  auto ghost_frag =
      builder.MakeFragmentInstance(steamrot::tests::FragmentNames::OneSocket);
  ghost_frag.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  ghost_frag.sockets.at(0).is_ready_to_connect = true;
  mr_ghost.m_instance = ghost_frag;

  steamrot::logic::action::grimoire_machina::place_next_piece(scaffold,
                                                              mr_ghost);

  REQUIRE(scaffold.parts.size() == 1);
  require_no_connections(scaffold);
}

TEST_CASE(
    "place_next_piece does nothing when ghost and PartGraph part are both "
    "FragmentInstances",
    "[unit][actions][grimoire_machina][place_next_piece]") {
  steamrot::tests::PartGraphBuilder builder;

  steamrot::MachinaFormScaffold scaffold;
  auto existing_frag =
      builder.MakeFragmentInstance(steamrot::tests::FragmentNames::TwoSockets);
  existing_frag.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  existing_frag.sockets.at(0).is_ready_to_connect = true;
  scaffold.parts.emplace(scaffold.next_id++, existing_frag);

  steamrot::MrGhost mr_ghost;
  auto ghost_frag =
      builder.MakeFragmentInstance(steamrot::tests::FragmentNames::OneSocket);
  ghost_frag.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  ghost_frag.sockets.at(0).is_ready_to_connect = true;
  mr_ghost.m_instance = ghost_frag;

  steamrot::logic::action::grimoire_machina::place_next_piece(scaffold,
                                                              mr_ghost);

  REQUIRE(scaffold.parts.size() == 1);
  require_no_connections(scaffold);
}

TEST_CASE(
    "place_next_piece does nothing when ghost and PartGraph part are both "
    "JointInstances",
    "[unit][actions][grimoire_machina][place_next_piece]") {
  steamrot::tests::PartGraphBuilder builder;

  steamrot::MachinaFormScaffold scaffold;
  auto existing_joint =
      builder.MakeJointInstance(steamrot::tests::JointNames::TwoSockets);
  existing_joint.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  existing_joint.sockets.at(0).is_ready_to_connect = true;
  scaffold.parts.emplace(scaffold.next_id++, existing_joint);

  steamrot::MrGhost mr_ghost;
  auto ghost_joint =
      builder.MakeJointInstance(steamrot::tests::JointNames::OneSocket);
  ghost_joint.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  ghost_joint.sockets.at(0).is_ready_to_connect = true;
  mr_ghost.m_instance = ghost_joint;

  steamrot::logic::action::grimoire_machina::place_next_piece(scaffold,
                                                              mr_ghost);

  REQUIRE(scaffold.parts.size() == 1);
  require_no_connections(scaffold);
}

/////////////////////////////////////////////////
/// place_next_piece success tests
/////////////////////////////////////////////////

TEST_CASE(
    "place_next_piece places a FragmentInstance when ghost is Fragment and "
    "PartGraph has a ready Joint",
    "[unit][actions][grimoire_machina][place_next_piece]") {
  steamrot::tests::PartGraphBuilder builder;

  steamrot::MachinaFormScaffold scaffold;
  auto existing_joint =
      builder.MakeJointInstance(steamrot::tests::JointNames::TwoSockets);
  existing_joint.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  existing_joint.sockets.at(0).is_ready_to_connect = true;
  scaffold.parts.emplace(scaffold.next_id++, existing_joint);
  REQUIRE(scaffold.parts.size() == 1);

  steamrot::MrGhost mr_ghost;
  auto ghost_frag =
      builder.MakeFragmentInstance(steamrot::tests::FragmentNames::OneSocket);
  ghost_frag.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  ghost_frag.sockets.at(0).is_ready_to_connect = true;
  mr_ghost.m_instance = ghost_frag;

  steamrot::logic::action::grimoire_machina::place_next_piece(scaffold,
                                                              mr_ghost);

  REQUIRE(scaffold.parts.size() == 2);
  REQUIRE(
      std::holds_alternative<steamrot::FragmentInstance>(scaffold.parts.at(1)));
}

TEST_CASE(
    "place_next_piece places a JointInstance when ghost is Joint and PartGraph "
    "has a ready Fragment",
    "[unit][actions][grimoire_machina][place_next_piece]") {
  steamrot::tests::PartGraphBuilder builder;

  steamrot::MachinaFormScaffold scaffold;
  auto existing_frag =
      builder.MakeFragmentInstance(steamrot::tests::FragmentNames::TwoSockets);
  existing_frag.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  existing_frag.sockets.at(0).is_ready_to_connect = true;
  scaffold.parts.emplace(scaffold.next_id++, existing_frag);
  REQUIRE(scaffold.parts.size() == 1);

  steamrot::MrGhost mr_ghost;
  auto ghost_joint =
      builder.MakeJointInstance(steamrot::tests::JointNames::OneSocket);
  ghost_joint.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  ghost_joint.sockets.at(0).is_ready_to_connect = true;
  mr_ghost.m_instance = ghost_joint;

  steamrot::logic::action::grimoire_machina::place_next_piece(scaffold,
                                                              mr_ghost);

  REQUIRE(scaffold.parts.size() == 2);
  REQUIRE(
      std::holds_alternative<steamrot::JointInstance>(scaffold.parts.at(1)));
}

TEST_CASE(
    "place_next_piece assigns a new id to the placed piece and increments "
    "next_id",
    "[unit][actions][grimoire_machina][place_next_piece]") {
  steamrot::tests::PartGraphBuilder builder;

  steamrot::MachinaFormScaffold scaffold;
  auto existing_joint =
      builder.MakeJointInstance(steamrot::tests::JointNames::TwoSockets);
  existing_joint.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  existing_joint.sockets.at(0).is_ready_to_connect = true;
  scaffold.parts.emplace(scaffold.next_id++, existing_joint);

  steamrot::MrGhost mr_ghost;
  auto ghost_frag =
      builder.MakeFragmentInstance(steamrot::tests::FragmentNames::OneSocket);
  ghost_frag.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  ghost_frag.sockets.at(0).is_ready_to_connect = true;
  mr_ghost.m_instance = ghost_frag;

  steamrot::logic::action::grimoire_machina::place_next_piece(scaffold,
                                                              mr_ghost);

  REQUIRE(std::get<steamrot::FragmentInstance>(scaffold.parts.at(1)).id == 1u);
  REQUIRE(scaffold.next_id == 2u);
}

TEST_CASE(
    "place_next_piece creates a connection between the placed piece and the "
    "existing part",
    "[unit][actions][grimoire_machina][place_next_piece]") {
  steamrot::tests::PartGraphBuilder builder;

  steamrot::MachinaFormScaffold scaffold;
  auto existing_joint =
      builder.MakeJointInstance(steamrot::tests::JointNames::TwoSockets);
  existing_joint.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  existing_joint.sockets.at(0).is_ready_to_connect = true;
  scaffold.parts.emplace(scaffold.next_id++, existing_joint);

  steamrot::MrGhost mr_ghost;
  auto ghost_frag =
      builder.MakeFragmentInstance(steamrot::tests::FragmentNames::OneSocket);
  ghost_frag.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  ghost_frag.sockets.at(0).is_ready_to_connect = true;
  mr_ghost.m_instance = ghost_frag;

  steamrot::logic::action::grimoire_machina::place_next_piece(scaffold,
                                                              mr_ghost);

  REQUIRE(scaffold.parts.size() == 2);
  // Placed FragmentInstance (id=1): socket[0] connects to existing Joint (id=0,
  // socket 0)
  const auto &placed_fi =
      std::get<steamrot::FragmentInstance>(scaffold.parts.at(1));
  REQUIRE(placed_fi.sockets.at(0).connected_to.has_value());
  REQUIRE(placed_fi.sockets.at(0).connected_to->peer_part_id == 0u);
  REQUIRE(placed_fi.sockets.at(0).connected_to->peer_socket_id == 0u);
  // Existing JointInstance (id=0): socket[0] connects back to placed Fragment
  // (id=1, socket 0)
  const auto &existing_ji =
      std::get<steamrot::JointInstance>(scaffold.parts.at(0));
  REQUIRE(existing_ji.sockets.at(0).connected_to.has_value());
  REQUIRE(existing_ji.sockets.at(0).connected_to->peer_part_id == 1u);
  REQUIRE(existing_ji.sockets.at(0).connected_to->peer_socket_id == 0u);
}

TEST_CASE("place_next_piece: connection endpoints are correct when Joint ghost "
          "connects to existing Fragment",
          "[unit][actions][grimoire_machina][place_next_piece]") {
  steamrot::tests::PartGraphBuilder builder;

  steamrot::MachinaFormScaffold scaffold;
  auto existing_frag =
      builder.MakeFragmentInstance(steamrot::tests::FragmentNames::TwoSockets);
  existing_frag.sockets.at(1).connection_state =
      steamrot::SocketConnectionState::Available;
  existing_frag.sockets.at(1).is_ready_to_connect = true;
  scaffold.parts.emplace(scaffold.next_id++, existing_frag);

  steamrot::MrGhost mr_ghost;
  auto ghost_joint =
      builder.MakeJointInstance(steamrot::tests::JointNames::TwoSockets);
  ghost_joint.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  ghost_joint.sockets.at(0).is_ready_to_connect = true;
  mr_ghost.m_instance = ghost_joint;

  steamrot::logic::action::grimoire_machina::place_next_piece(scaffold,
                                                              mr_ghost);

  REQUIRE(scaffold.parts.size() == 2);
  // Existing FragmentInstance (id=0): socket[1] connects to placed Joint (id=1,
  // socket 0)
  const auto &existing_fi =
      std::get<steamrot::FragmentInstance>(scaffold.parts.at(0));
  REQUIRE(existing_fi.sockets.at(1).connected_to.has_value());
  REQUIRE(existing_fi.sockets.at(1).connected_to->peer_part_id == 1u);
  REQUIRE(existing_fi.sockets.at(1).connected_to->peer_socket_id == 0u);
  // Placed JointInstance (id=1): socket[0] connects back to existing Fragment
  // (id=0, socket 1)
  const auto &placed_ji =
      std::get<steamrot::JointInstance>(scaffold.parts.at(1));
  REQUIRE(placed_ji.sockets.at(0).connected_to.has_value());
  REQUIRE(placed_ji.sockets.at(0).connected_to->peer_part_id == 0u);
  REQUIRE(placed_ji.sockets.at(0).connected_to->peer_socket_id == 1u);
}

TEST_CASE("place_next_piece marks connected sockets as SocketState::Connected",
          "[unit][actions][grimoire_machina][place_next_piece]") {
  steamrot::tests::PartGraphBuilder builder;

  steamrot::MachinaFormScaffold scaffold;
  auto existing_joint =
      builder.MakeJointInstance(steamrot::tests::JointNames::TwoSockets);
  existing_joint.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  existing_joint.sockets.at(0).is_ready_to_connect = true;
  scaffold.parts.emplace(scaffold.next_id++, existing_joint);

  steamrot::MrGhost mr_ghost;
  auto ghost_frag =
      builder.MakeFragmentInstance(steamrot::tests::FragmentNames::OneSocket);
  ghost_frag.sockets.at(0).connection_state =
      steamrot::SocketConnectionState::Available;
  ghost_frag.sockets.at(0).is_ready_to_connect = true;
  mr_ghost.m_instance = ghost_frag;

  steamrot::logic::action::grimoire_machina::place_next_piece(scaffold,
                                                              mr_ghost);

  const steamrot::FragmentInstance &placed_fi =
      std::get<steamrot::FragmentInstance>(scaffold.parts.at(1));
  const steamrot::JointInstance &existing_ji =
      std::get<steamrot::JointInstance>(scaffold.parts.at(0));

  REQUIRE(placed_fi.sockets.at(0).connection_state ==
          steamrot::SocketConnectionState::Connected);
  REQUIRE(existing_ji.sockets.at(0).connection_state ==
          steamrot::SocketConnectionState::Connected);
}
} // namespace steamrot::tests
