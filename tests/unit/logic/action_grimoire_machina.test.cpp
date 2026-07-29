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
#include "PartGraphBuilder.h"
#include "SocketState.h"
#include "Subscriber.h"
#include "TestFixture.h"
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
          for (const auto &[sid, socket] : instance.GetSockets())
            REQUIRE_FALSE(socket.GetConnectionState() ==
                          SocketConnectionState::Connected);
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
/// PlaceGhostOnScaffold guard tests
/////////////////////////////////////////////////

TEST_CASE("PlaceGhostOnScaffold does nothing when no scaffold is active",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance.emplace<FragmentInstance>(
      0, grimoire_machina.m_all_fragments["frag"]);

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

TEST_CASE("PlaceGhostOnScaffold first piece: appends fragment to scaffold",
          "[unit][actions][grimoire_machina][PlaceGhostOnScaffold]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance.emplace<FragmentInstance>(
      0, grimoire_machina.m_all_fragments["frag"]);

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
  mr_ghost.m_instance.emplace<FragmentInstance>(
      0, grimoire_machina.m_all_fragments["frag"]);

  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
      grimoire_machina, mr_ghost);

  REQUIRE(std::get<steamrot::FragmentInstance>(
              grimoire_machina.m_scaffold_form->parts.at(0))
              .GetId() == 0u);
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
  mr_ghost.m_instance.emplace<FragmentInstance>(
      0, grimoire_machina.m_all_fragments["frag"]);

  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
      grimoire_machina, mr_ghost);

  REQUIRE(std::get<steamrot::FragmentInstance>(
              grimoire_machina.m_scaffold_form->parts.at(0))
              .GetTransform() == sf::Transform::Identity);
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
  mr_ghost.m_instance.emplace<JointInstance>(
      0, grimoire_machina.m_all_joints["joint"]);

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
  mr_ghost.m_instance.emplace<JointInstance>(
      0, grimoire_machina.m_all_joints["joint"]);

  place_ghost_on_scaffold(grimoire_machina, mr_ghost);

  REQUIRE(std::get<steamrot::JointInstance>(
              grimoire_machina.m_scaffold_form->parts.at(0))
              .GetId() == 0u);
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
  mr_ghost.m_instance.emplace<JointInstance>(
      0, grimoire_machina.m_all_joints["joint"]);

  steamrot::logic::action::grimoire_machina::place_ghost_on_scaffold(
      grimoire_machina, mr_ghost);

  REQUIRE(std::get<steamrot::JointInstance>(
              grimoire_machina.m_scaffold_form->parts.at(0))
              .GetTransform() == sf::Transform::Identity);
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
  mr_ghost.m_instance.emplace<FragmentInstance>(
      0, grimoire_machina.m_all_fragments["frag"]);

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
  mr_ghost.m_instance.emplace<JointInstance>(
      0, grimoire_machina.m_all_joints["joint"]);

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
  mr_ghost.m_instance.emplace<FragmentInstance>(
      0, grimoire_machina.m_all_fragments["frag"]);

  place_ghost_on_scaffold(grimoire_machina, mr_ghost);
  place_ghost_on_scaffold(grimoire_machina, mr_ghost);
  place_ghost_on_scaffold(grimoire_machina, mr_ghost);

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
  mr_ghost.m_instance.emplace<JointInstance>(
      0, grimoire_machina.m_all_joints["joint"]);

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
  mr_ghost.m_instance.emplace<FragmentInstance>(
      0, grimoire_machina.m_all_fragments["frag"]);

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
  mr_ghost.m_instance.emplace<FragmentInstance>(
      0, grimoire_machina.m_all_fragments["frag"]);

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

TEST_CASE("PlaceFirstPiece appends a fragment to an empty scaffold",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance.emplace<FragmentInstance>(
      0, grimoire_machina.m_all_fragments["frag"]);

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
  mr_ghost.m_instance.emplace<JointInstance>(
      0, grimoire_machina.m_all_joints["joint"]);

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
  mr_ghost.m_instance.emplace<FragmentInstance>(
      0, grimoire_machina.m_all_fragments["frag"]);

  steamrot::logic::action::grimoire_machina::place_first_piece(grimoire_machina,
                                                               mr_ghost);

  REQUIRE(std::get<steamrot::FragmentInstance>(
              grimoire_machina.m_scaffold_form->parts.at(0))
              .GetId() == 0u);
  REQUIRE(grimoire_machina.m_scaffold_form->next_id == 1u);
}

TEST_CASE("PlaceFirstPiece assigns joint id 0 and increments next_id",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance.emplace<JointInstance>(
      0, grimoire_machina.m_all_joints["joint"]);

  place_first_piece(grimoire_machina, mr_ghost);

  REQUIRE(std::get<steamrot::JointInstance>(
              grimoire_machina.m_scaffold_form->parts.at(0))
              .GetId() == 0u);
  REQUIRE(grimoire_machina.m_scaffold_form->next_id == 1u);
}

TEST_CASE("PlaceFirstPiece: placed fragment has identity transform",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = steamrot::Fragment{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance.emplace<FragmentInstance>(
      0, grimoire_machina.m_all_fragments["frag"]);

  place_first_piece(grimoire_machina, mr_ghost);

  REQUIRE(std::get<steamrot::FragmentInstance>(
              grimoire_machina.m_scaffold_form->parts.at(0))
              .GetTransform() == sf::Transform::Identity);
}

TEST_CASE("PlaceFirstPiece: placed joint has identity transform",
          "[unit][actions][grimoire_machina][PlaceFirstPiece]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  grimoire_machina.m_all_joints["joint"] = steamrot::Joint{};

  steamrot::MrGhost mr_ghost;
  mr_ghost.m_instance.emplace<JointInstance>(
      0, grimoire_machina.m_all_joints["joint"]);

  steamrot::logic::action::grimoire_machina::place_first_piece(grimoire_machina,
                                                               mr_ghost);

  REQUIRE(std::get<steamrot::JointInstance>(
              grimoire_machina.m_scaffold_form->parts.at(0))
              .GetTransform() == sf::Transform::Identity);
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
  mr_ghost.m_instance.emplace<FragmentInstance>(
      0, grimoire_machina.m_all_fragments["frag"]);

  place_first_piece(grimoire_machina, mr_ghost);
  place_first_piece(grimoire_machina, mr_ghost);
  place_first_piece(grimoire_machina, mr_ghost);

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
  fixture.GetMrGhost().m_instance.emplace<FragmentInstance>(
      0, grimoire_machina.m_all_fragments["frag"]);

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
  fixture.GetMrGhost().m_instance.emplace<JointInstance>(
      0, grimoire_machina.m_all_joints["joint"]);

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
    mr_ghost.m_instance.emplace<FragmentInstance>(frag_instance);
    auto result = steamrot::logic::action::grimoire_machina::
        check_MrGhost_for_connection_readiness(mr_ghost);
    REQUIRE_FALSE(result);
  }

  SECTION("check_MrGhost_for_connection_readiness returns false for joint with "
          "no sockets") {
    steamrot::JointInstance joint_instance =
        builder.MakeJointInstance(steamrot::tests::JointNames::NoSocket);
    steamrot::MrGhost mr_ghost;
    mr_ghost.m_instance.emplace<JointInstance>(joint_instance);
    auto result = steamrot::logic::action::grimoire_machina::
        check_MrGhost_for_connection_readiness(mr_ghost);
    REQUIRE_FALSE(result);
  }
}

TEST_CASE("check_PartGraph_for_connection_readiness tests",
          "[unit][actions][grimoire_machina][check_PartGraph_for_connection_"
          "readiness]") {}

} // namespace steamrot::tests
