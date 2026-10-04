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
#include "FragmentInstance.h"
#include "JointInstance.h"
#include "MachinaFormScaffold.h"
#include "SocketState.h"
#include "Subscriber.h"
#include "TestFixture.h"
#include "fragment_library.h"
#include "joint_library.h"
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>

namespace steamrot::tests {

using namespace steamrot::logic::action::grimoire_machina;

namespace {

/////////////////////////////////////////////////
/// @brief Require that no socket in any part of @p scaffold has
/// @c connected_to set. Used by place_next_piece guard tests to verify that
/// a rejected placement left the scaffold unmodified.
/////////////////////////////////////////////////
void require_no_connections(const MachinaFormScaffold &scaffold) {
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

TEST_CASE("initialise_active_machina_form_scaffold tests",
          "[unit][actions][grimoire_machina]") {
  GrimoireMachina grimoire_machina;
  REQUIRE(grimoire_machina.m_scaffold_form == nullptr);

  SECTION(
      "initialise_active_machina_form_scaffold adds a new MachinaForm to the "
      "GrimoireMachina active form") {
    initialise_active_machina_form_scaffold(grimoire_machina);
    REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
  }
}

TEST_CASE("clear_active_machina_form_scaffold tests",
          "[unit][actions][grimoire_machina]") {
  GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form = std::make_unique<MachinaFormScaffold>();
  REQUIRE(grimoire_machina.m_scaffold_form != nullptr);

  SECTION("clear_active_machina_form_scaffold clears the active MachinaForm "
          "in the GrimoireMachina") {
    clear_active_machina_form_scaffold(grimoire_machina);
    REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
  }
}

TEST_CASE("get_all_fragment_names tests", "[unit][actions][grimoire_machina]") {
  GrimoireMachina grimoire_machina;

  SECTION("get_all_fragment_names returns an empty vector when "
          "GrimoireMachina has no fragments") {
    auto fragment_names = get_all_fragment_names(grimoire_machina);
    REQUIRE(fragment_names.empty());
  }

  SECTION("Multiple fragments in GrimoireMachina") {
    grimoire_machina.m_all_fragments = {{"Fragment1", Fragment{}},
                                        {"Fragment2", Fragment{}},
                                        {"Fragment3", Fragment{}}};
    auto fragment_names = get_all_fragment_names(grimoire_machina);
    REQUIRE(fragment_names.size() == 3);
    REQUIRE(fragment_names[0] == "Fragment1");
    REQUIRE(fragment_names[1] == "Fragment2");
    REQUIRE(fragment_names[2] == "Fragment3");
  }
}

TEST_CASE("get_all_joint_names tests", "[unit][actions][grimoire_machina]") {
  GrimoireMachina grimoire_machina;

  SECTION("get_all_joint_names returns an empty vector when GrimoireMachina "
          "has no joints") {
    auto joint_names = get_all_joint_names(grimoire_machina);
    REQUIRE(joint_names.empty());
  }

  SECTION("Multiple joints in GrimoireMachina") {
    grimoire_machina.m_all_joints = {
        {"Joint1", Joint{}}, {"Joint2", Joint{}}, {"Joint3", Joint{}}};
    auto joint_names = get_all_joint_names(grimoire_machina);
    REQUIRE(joint_names.size() == 3);
    REQUIRE(joint_names[0] == "Joint1");
    REQUIRE(joint_names[1] == "Joint2");
    REQUIRE(joint_names[2] == "Joint3");
  }
}

TEST_CASE("toggle_socket_visibility tests",
          "[unit][actions][grimoire_machina]") {
  MachinaFormScaffold scaffold;
  REQUIRE(scaffold.are_sockets_visible == false);

  SECTION("toggle_socket_visibility toggles are_sockets_visible from false "
          "to true") {
    toggle_socket_visibility(scaffold);
    REQUIRE(scaffold.are_sockets_visible == true);
  }

  SECTION("toggle_socket_visibility toggles are_sockets_visible from true "
          "to false") {
    scaffold.are_sockets_visible = true;
    toggle_socket_visibility(scaffold);
    REQUIRE(scaffold.are_sockets_visible == false);
  }
}

TEST_CASE("place_first_piece tests",
          "[unit][actions][grimoire_machina][place_first_piece]") {
  // Common setup shared across all sections: an active scaffold with a
  // registered fragment and a ghost with no selection yet.
  GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form = std::make_unique<MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = Fragment{};
  MrGhost mr_ghost;

  SECTION("place_first_piece does nothing when no scaffold is active") {
    grimoire_machina.m_scaffold_form = nullptr;
    mr_ghost.m_instance.emplace<FragmentInstance>(
        0, grimoire_machina.m_all_fragments["frag"]);

    place_first_piece(grimoire_machina.m_scaffold_form.get(), mr_ghost);

    // need to make sure that no scaffold is formed (thus we can't access the
    // PartGraph)
    REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
  }

  SECTION("place_first_piece does nothing when scaffold already has "
          "pieces") {
    mr_ghost.m_instance.emplace<FragmentInstance>(
        0, grimoire_machina.m_all_fragments["frag"]);
    REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());

    // Place the first piece.
    place_first_piece(grimoire_machina.m_scaffold_form.get(), mr_ghost);
    REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);

    // A second call must be ignored.
    place_first_piece(grimoire_machina.m_scaffold_form.get(), mr_ghost);

    REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
  }

  SECTION("place_first_piece does nothing when ghost selection is "
          "monostate") {
    mr_ghost.m_instance = std::monostate{};
    REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());

    place_first_piece(grimoire_machina.m_scaffold_form.get(), mr_ghost);

    REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());
  }

  SECTION("place_first_piece appends a fragment to an empty scaffold") {
    mr_ghost.m_instance.emplace<FragmentInstance>(
        0, grimoire_machina.m_all_fragments["frag"]);
    REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());

    place_first_piece(grimoire_machina.m_scaffold_form.get(), mr_ghost);

    REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
    REQUIRE(std::holds_alternative<FragmentInstance>(
        grimoire_machina.m_scaffold_form->parts.at(0)));
  }

  SECTION("place_first_piece appends a joint to an empty scaffold") {
    grimoire_machina.m_all_joints["joint"] = Joint{};
    mr_ghost.m_instance.emplace<JointInstance>(
        0, grimoire_machina.m_all_joints["joint"]);
    REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());

    place_first_piece(grimoire_machina.m_scaffold_form.get(), mr_ghost);

    REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
    REQUIRE(std::holds_alternative<JointInstance>(
        grimoire_machina.m_scaffold_form->parts.at(0)));
  }

  SECTION("place_first_piece assigns fragment id 0 and increments "
          "next_id") {
    mr_ghost.m_instance.emplace<FragmentInstance>(
        0, grimoire_machina.m_all_fragments["frag"]);

    place_first_piece(grimoire_machina.m_scaffold_form.get(), mr_ghost);

    REQUIRE(std::get<FragmentInstance>(
                grimoire_machina.m_scaffold_form->parts.at(0))
                .GetId() == 0u);
    REQUIRE(grimoire_machina.m_scaffold_form->next_id == 1u);
  }

  SECTION("place_first_piece placed fragment has position 0") {
    mr_ghost.m_instance.emplace<FragmentInstance>(
        0, grimoire_machina.m_all_fragments["frag"]);

    place_first_piece(grimoire_machina.m_scaffold_form.get(), mr_ghost);

    REQUIRE(std::get<FragmentInstance>(
                grimoire_machina.m_scaffold_form->parts.at(0))
                .getPosition() == sf::Vector2f(0.f, 0.f));
  }
}

TEST_CASE("check__part_graph_for_connection_readiness tests",
          "[unit][actions][grimoire_machina]"
          "[check_part_graph_for_connection_readiness]") {

  // Arrange
  PartGraph part_graph;
  part_graph.emplace(
      0, FragmentInstance{0, parts::FragmentRectangleWithOneSocket});
  FragmentInstance &frag0 = std::get<FragmentInstance>(part_graph.at(0));
  part_graph.emplace(1, JointInstance{1, parts::JointSquareWithOneSocket});
  JointInstance &joint1 = std::get<JointInstance>(part_graph.at(1));
  joint1.PositionSockets(JointSocketPositioningStrategy::MaximizeDistance);
  part_graph.emplace(
      2, FragmentInstance{2, parts::FragmentRectangleWithTwoSockets});
  FragmentInstance &frag2 = std::get<FragmentInstance>(part_graph.at(2));

  SECTION(
      "check_part_graph_for_connection_readiness returns nullopt if emmpty") {

    // Arrange
    part_graph.clear();

    // Act
    auto result = check_part_graph_for_connection_readiness(part_graph);

    // Assert
    REQUIRE_FALSE(result);
  }

  SECTION(
      "check_part_graph_for_connection_readiness returns nullopt if no sockets "
      "are available") {
    // Arrange
    part_graph.clear();
    FragmentInstance frag_instance{0, parts::FragmentRectangleWithNoSockets};
    part_graph.emplace(frag_instance.GetId(), frag_instance);

    // Act
    auto result = check_part_graph_for_connection_readiness(part_graph);

    // Assert
    REQUIRE_FALSE(result);
  }

  SECTION("check_part_graph_for_connection_readiness returns nullopt if no"
          "sockets are connection ready") {

    // Act
    auto result = check_part_graph_for_connection_readiness(part_graph);

    // Assert
    REQUIRE_FALSE(result);
  }

  SECTION("check_part_graph_for_connection_readiness returns the first ready "
          "socket pair when a connection is possible") {

    SECTION("fragment0") {
      // Arrange
      REQUIRE(frag0.SetSocketConnectionDistance(
          0, k_connection_distance_threshold - 1.f));
      // Act
      auto result = check_part_graph_for_connection_readiness(part_graph);
      // Assert
      REQUIRE(result.has_value());
      REQUIRE(result->first == 0u);
      REQUIRE(result->second == 0u);
    }

    SECTION("joint1") {
      // Arrange
      REQUIRE(joint1.SetSocketConnectionDistance(
          0, k_connection_distance_threshold - 1.f));
      // Act
      auto result = check_part_graph_for_connection_readiness(part_graph);
      // Assert
      REQUIRE(result.has_value());
      REQUIRE(result->first == 1u);
      REQUIRE(result->second == 0u);
    }
    SECTION("fragment2") {
      // Arrange
      REQUIRE(frag2.SetSocketConnectionDistance(
          1, k_connection_distance_threshold - 1.f));
      // Act
      auto result = check_part_graph_for_connection_readiness(part_graph);
      // Assert
      REQUIRE(result.has_value());
      REQUIRE(result->first == 2u);
      REQUIRE(result->second == 1u);
    }

    SECTION("multiple ready sockets: returns the first one found") {
      // Arrange
      REQUIRE(frag0.SetSocketConnectionDistance(
          0, k_connection_distance_threshold - 1.f));
      REQUIRE(joint1.SetSocketConnectionDistance(
          0, k_connection_distance_threshold - 1.f));
      REQUIRE(frag2.SetSocketConnectionDistance(
          1, k_connection_distance_threshold - 1.f));
      // Act
      auto result = check_part_graph_for_connection_readiness(part_graph);
      // Assert
      REQUIRE(result.has_value());
      REQUIRE(result->first == 0u);
      REQUIRE(result->second == 0u);
    }
  }
}

TEST_CASE("place_next_piece tests",
          "[unit][actions][grimoire_machina][place_next_piece]") {

  // Common setup shared across all sections: an active scaffold with a
  // registered fragment and a ghost with no selection yet.
  GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form = std::make_unique<MachinaFormScaffold>();
  // grimoire_machina.m_all_fragments["frag"] = Fragment{};
  MrGhost mr_ghost;

  SECTION("place_next_piece does nothing when no scaffold is active") {
    // Arrange
    grimoire_machina.m_scaffold_form = nullptr;
    mr_ghost.m_instance.emplace<FragmentInstance>(
        0, grimoire_machina.m_all_fragments["frag"]);

    // Act
    place_next_piece(grimoire_machina.m_scaffold_form.get(), mr_ghost);

    // Assert
    // need to make sure that no scaffold is formed (thus we can't access the
    // PartGraph)
    REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
  }

  SECTION("place_next_piece does nothing when PartGraph is empty") {
    // Arrange
    mr_ghost.m_instance.emplace<FragmentInstance>(
        0, grimoire_machina.m_all_fragments["frag"]);
    REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());
    // Act
    place_next_piece(grimoire_machina.m_scaffold_form.get(), mr_ghost);
    // Assert
    REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());
  }
}

TEST_CASE("place_ghost_on_scaffold tests",
          "[unit][actions][grimoire_machina][place_ghost_on_scaffold]") {
  // Common setup shared across all sections: an active scaffold with a
  // registered fragment and joint, and a ghost with no selection yet.
  GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form = std::make_unique<MachinaFormScaffold>();
  grimoire_machina.m_all_fragments["frag"] = Fragment{};
  grimoire_machina.m_all_joints["joint"] = Joint{};
  MrGhost mr_ghost;

  SECTION("place_ghost_on_scaffold does nothing when no scaffold is "
          "active") {
    grimoire_machina.m_scaffold_form = nullptr;
    mr_ghost.m_instance.emplace<FragmentInstance>(
        0, grimoire_machina.m_all_fragments["frag"]);

    REQUIRE_NOTHROW(place_ghost_on_scaffold(grimoire_machina, mr_ghost));

    // no scaffold was ever created — it must remain null
    REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
  }

  SECTION("place_ghost_on_scaffold does nothing when ghost selection is "
          "monostate") {
    // mr_ghost has default selection = monostate
    place_ghost_on_scaffold(grimoire_machina, mr_ghost);

    REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());
  }

  SECTION("place_ghost_on_scaffold first piece: appends fragment to "
          "scaffold") {
    mr_ghost.m_instance.emplace<FragmentInstance>(
        0, grimoire_machina.m_all_fragments["frag"]);

    place_ghost_on_scaffold(grimoire_machina, mr_ghost);

    REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
    REQUIRE(std::holds_alternative<FragmentInstance>(
        grimoire_machina.m_scaffold_form->parts.at(0)));
  }

  SECTION("place_ghost_on_scaffold first piece: fragment id and next_id "
          "are assigned") {
    mr_ghost.m_instance.emplace<FragmentInstance>(
        0, grimoire_machina.m_all_fragments["frag"]);

    place_ghost_on_scaffold(grimoire_machina, mr_ghost);

    REQUIRE(std::get<FragmentInstance>(
                grimoire_machina.m_scaffold_form->parts.at(0))
                .GetId() == 0u);
    REQUIRE(grimoire_machina.m_scaffold_form->next_id == 1u);
  }

  SECTION("place_ghost_on_scaffold first piece: fragment instance has "
          "identity transform") {
    mr_ghost.m_instance.emplace<FragmentInstance>(
        0, grimoire_machina.m_all_fragments["frag"]);

    place_ghost_on_scaffold(grimoire_machina, mr_ghost);

    REQUIRE(std::get<FragmentInstance>(
                grimoire_machina.m_scaffold_form->parts.at(0))
                .getTransform() == sf::Transform::Identity);
  }

  SECTION("place_ghost_on_scaffold first piece: appends joint to "
          "scaffold") {
    mr_ghost.m_instance.emplace<JointInstance>(
        0, grimoire_machina.m_all_joints["joint"]);

    place_ghost_on_scaffold(grimoire_machina, mr_ghost);

    REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
    REQUIRE(std::holds_alternative<JointInstance>(
        grimoire_machina.m_scaffold_form->parts.at(0)));
  }

  SECTION("place_ghost_on_scaffold first piece: joint id and next_id are "
          "assigned") {
    mr_ghost.m_instance.emplace<JointInstance>(
        0, grimoire_machina.m_all_joints["joint"]);

    place_ghost_on_scaffold(grimoire_machina, mr_ghost);

    REQUIRE(
        std::get<JointInstance>(grimoire_machina.m_scaffold_form->parts.at(0))
            .GetId() == 0u);
    REQUIRE(grimoire_machina.m_scaffold_form->next_id == 1u);
  }

  SECTION("place_ghost_on_scaffold first piece: joint instance has "
          "identity transform") {
    mr_ghost.m_instance.emplace<JointInstance>(
        0, grimoire_machina.m_all_joints["joint"]);

    place_ghost_on_scaffold(grimoire_machina, mr_ghost);

    REQUIRE(
        std::get<JointInstance>(grimoire_machina.m_scaffold_form->parts.at(0))
            .getTransform() == sf::Transform::Identity);
  }

  SECTION("place_ghost_on_scaffold does not add pieces when scaffold "
          "already has a fragment") {
    mr_ghost.m_instance.emplace<FragmentInstance>(
        0, grimoire_machina.m_all_fragments["frag"]);

    // Place the first piece successfully.
    place_ghost_on_scaffold(grimoire_machina, mr_ghost);
    REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);

    // A second placement must not add any more pieces (no collision logic
    // yet).
    place_ghost_on_scaffold(grimoire_machina, mr_ghost);

    REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
    REQUIRE(std::holds_alternative<FragmentInstance>(
        grimoire_machina.m_scaffold_form->parts.at(0)));
  }

  SECTION("place_ghost_on_scaffold does not add pieces when scaffold "
          "already has a joint") {
    mr_ghost.m_instance.emplace<JointInstance>(
        0, grimoire_machina.m_all_joints["joint"]);

    // Place the first piece successfully.
    place_ghost_on_scaffold(grimoire_machina, mr_ghost);
    REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);

    // A second placement must not add any more pieces (no collision logic
    // yet).
    place_ghost_on_scaffold(grimoire_machina, mr_ghost);

    REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
    REQUIRE(std::holds_alternative<JointInstance>(
        grimoire_machina.m_scaffold_form->parts.at(0)));
  }

  SECTION("place_ghost_on_scaffold only places one fragment per game "
          "instance") {
    mr_ghost.m_instance.emplace<FragmentInstance>(
        0, grimoire_machina.m_all_fragments["frag"]);

    place_ghost_on_scaffold(grimoire_machina, mr_ghost);
    place_ghost_on_scaffold(grimoire_machina, mr_ghost);
    place_ghost_on_scaffold(grimoire_machina, mr_ghost);

    REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
    REQUIRE(std::holds_alternative<FragmentInstance>(
        grimoire_machina.m_scaffold_form->parts.at(0)));
  }

  SECTION("place_ghost_on_scaffold only places one joint per game "
          "instance") {
    mr_ghost.m_instance.emplace<JointInstance>(
        0, grimoire_machina.m_all_joints["joint"]);

    place_ghost_on_scaffold(grimoire_machina, mr_ghost);
    place_ghost_on_scaffold(grimoire_machina, mr_ghost);
    place_ghost_on_scaffold(grimoire_machina, mr_ghost);

    REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
    REQUIRE(std::holds_alternative<JointInstance>(
        grimoire_machina.m_scaffold_form->parts.at(0)));
  }
}

TEST_CASE("process_logic_events tests",
          "[unit][actions][grimoire_machina][process_logic_events]") {
  // Common setup shared across all sections: an active subscriber for the
  // LOGIC event type.
  GrimoireMachina grimoire_machina;
  auto subscriber = std::make_shared<Subscriber>();
  subscriber->m_active = true;
  subscriber->event_type = EventType::LOGIC;

  SECTION("process_logic_events: active INITIATE subscriber initialises "
          "scaffold") {
    subscriber->captured_payload =
        LogicPayload{LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD};

    process_logic_events(*subscriber, grimoire_machina);

    REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
  }

  SECTION("process_logic_events: active CLEAR subscriber clears existing "
          "scaffold") {
    grimoire_machina.m_scaffold_form = std::make_unique<MachinaFormScaffold>();
    subscriber->captured_payload =
        LogicPayload{LogicPayload::LogicToggle::CLEAR_MACHINA_FORM_SCAFFOLD};

    process_logic_events(*subscriber, grimoire_machina);

    REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
  }
}

TEST_CASE("process_user_input_events tests",
          "[unit][actions][grimoire_machina][process_user_input_events]") {
  // Common setup shared across all sections: a fixture-backed scene context,
  // an active scaffold and an active USER_INPUT subscriber.
  TestFixture fixture;
  SceneContext &scene_context = fixture.GetSceneContext();

  GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form = std::make_unique<MachinaFormScaffold>();

  Subscriber subscriber;
  subscriber.m_active = true;
  subscriber.event_type = EventType::USER_INPUT;

  SECTION("process_user_input_events: missing captured_payload is ignored "
          "without crash") {
    // no captured_payload

    REQUIRE_NOTHROW(
        process_user_input_events(subscriber, scene_context, grimoire_machina));

    REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());
  }

  SECTION("process_user_input_events: TOGGLE_SOCKET_VISIBILITY toggles "
          "socket visibility") {
    REQUIRE(grimoire_machina.m_scaffold_form->are_sockets_visible == false);
    subscriber.captured_payload =
        InputPayload{InputPayload::InputAction::TOGGLE_SOCKET_VISIBILITY};

    process_user_input_events(subscriber, scene_context, grimoire_machina);

    REQUIRE(grimoire_machina.m_scaffold_form->are_sockets_visible == true);
  }

  SECTION("process_user_input_events: TOGGLE_SOCKET_VISIBILITY with no "
          "scaffold does not crash") {
    grimoire_machina.m_scaffold_form = nullptr;
    subscriber.captured_payload =
        InputPayload{InputPayload::InputAction::TOGGLE_SOCKET_VISIBILITY};

    REQUIRE_NOTHROW(
        process_user_input_events(subscriber, scene_context, grimoire_machina));
  }

  SECTION("process_user_input_events: SELECT with valid conditions places "
          "fragment") {
    grimoire_machina.m_all_fragments["frag"] = Fragment{};
    fixture.GetMrGhost().m_instance.emplace<FragmentInstance>(
        0, grimoire_machina.m_all_fragments["frag"]);

    // scene_state.is_mouse_over_ui_layer defaults to false
    subscriber.captured_payload =
        InputPayload{InputPayload::InputAction::SELECT};

    process_user_input_events(subscriber, scene_context, grimoire_machina);

    REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
    REQUIRE(std::holds_alternative<FragmentInstance>(
        grimoire_machina.m_scaffold_form->parts.at(0)));
  }

  SECTION("process_user_input_events: SELECT with valid conditions places "
          "joint") {
    grimoire_machina.m_all_joints["joint"] = Joint{};
    fixture.GetMrGhost().m_instance.emplace<JointInstance>(
        0, grimoire_machina.m_all_joints["joint"]);

    subscriber.captured_payload =
        InputPayload{InputPayload::InputAction::SELECT};

    process_user_input_events(subscriber, scene_context, grimoire_machina);

    REQUIRE(grimoire_machina.m_scaffold_form->parts.size() == 1);
    REQUIRE(std::holds_alternative<JointInstance>(
        grimoire_machina.m_scaffold_form->parts.at(0)));
  }

  SECTION("process_user_input_events: SELECT with monostate ghost does not "
          "place") {
    // mr_ghost has default monostate selection
    subscriber.captured_payload =
        InputPayload{InputPayload::InputAction::SELECT};

    process_user_input_events(subscriber, scene_context, grimoire_machina);

    REQUIRE(grimoire_machina.m_scaffold_form->parts.empty());
  }
}

TEST_CASE("process_subscribers tests",
          "[unit][actions][grimoire_machina][process_subscribers]") {
  // Common setup shared across all sections: a fixture-backed scene context
  // and a GrimoireMachina with no active scaffold.
  TestFixture fixture;
  fixture.Initialize();
  SceneContext &scene_context = fixture.GetSceneContext();

  GrimoireMachina grimoire_machina;

  SECTION("process_subscribers: LOGIC INITIATE subscriber initialises "
          "scaffold in a single pass") {
    auto subscriber = std::make_shared<Subscriber>();
    subscriber->m_active = true;
    subscriber->event_type = EventType::LOGIC;
    subscriber->captured_payload =
        LogicPayload{LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD};

    std::vector<std::shared_ptr<Subscriber>> subscribers{subscriber};
    process_subscribers(subscribers, scene_context, grimoire_machina);

    REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
  }

  SECTION("process_subscribers: LOGIC CLEAR subscriber clears scaffold in "
          "a single pass") {
    grimoire_machina.m_scaffold_form = std::make_unique<MachinaFormScaffold>();

    auto subscriber = std::make_shared<Subscriber>();
    subscriber->m_active = true;
    subscriber->event_type = EventType::LOGIC;
    subscriber->captured_payload =
        LogicPayload{LogicPayload::LogicToggle::CLEAR_MACHINA_FORM_SCAFFOLD};

    std::vector<std::shared_ptr<Subscriber>> subscribers{subscriber};
    process_subscribers(subscribers, scene_context, grimoire_machina);

    REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
  }

  SECTION("process_subscribers: TOGGLE_SOCKET_VISIBILITY subscriber "
          "toggles socket visibility in a single pass") {
    grimoire_machina.m_scaffold_form = std::make_unique<MachinaFormScaffold>();
    REQUIRE(grimoire_machina.m_scaffold_form->are_sockets_visible == false);

    auto subscriber = std::make_shared<Subscriber>();
    subscriber->m_active = true;
    subscriber->event_type = EventType::USER_INPUT;
    subscriber->captured_payload =
        InputPayload{InputPayload::InputAction::TOGGLE_SOCKET_VISIBILITY};

    std::vector<std::shared_ptr<Subscriber>> subscribers{subscriber};
    process_subscribers(subscribers, scene_context, grimoire_machina);

    REQUIRE(grimoire_machina.m_scaffold_form->are_sockets_visible == true);
  }

  SECTION("process_subscribers: inactive subscriber is skipped") {
    auto subscriber = std::make_shared<Subscriber>();
    subscriber->m_active = false;
    subscriber->event_type = EventType::LOGIC;
    subscriber->captured_payload =
        LogicPayload{LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD};

    std::vector<std::shared_ptr<Subscriber>> subscribers{subscriber};
    process_subscribers(subscribers, scene_context, grimoire_machina);

    REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
  }

  SECTION("process_subscribers: multiple mixed subscribers processed in a "
          "single pass") {
    // First: initialise scaffold
    auto init_subscriber = std::make_shared<Subscriber>();
    init_subscriber->m_active = true;
    init_subscriber->event_type = EventType::LOGIC;
    init_subscriber->captured_payload =
        LogicPayload{LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD};

    // Second: toggle socket visibility
    auto toggle_subscriber = std::make_shared<Subscriber>();
    toggle_subscriber->m_active = true;
    toggle_subscriber->event_type = EventType::USER_INPUT;
    toggle_subscriber->captured_payload =
        InputPayload{InputPayload::InputAction::TOGGLE_SOCKET_VISIBILITY};

    std::vector<std::shared_ptr<Subscriber>> subscribers{init_subscriber,
                                                         toggle_subscriber};
    process_subscribers(subscribers, scene_context, grimoire_machina);

    REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
    REQUIRE(grimoire_machina.m_scaffold_form->are_sockets_visible == true);
  }
}

} // namespace steamrot::tests
