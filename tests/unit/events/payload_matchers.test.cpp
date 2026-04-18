/////////////////////////////////////////////////
/// @file
/// @brief unit tests for payload matchers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "payload_matchers.h"
#include "EventPayload.h"
#include "Fragment.h"
#include "Joint.h"
#include "MachinaFormScaffold.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("MatchPayload deals with various configurations of InputPayload",
          "[unit][MatchPayload][InputPayload]") {

  using namespace steamrot::events;
  steamrot::InputPayload filter_payload;
  steamrot::InputPayload event_payload;

  SECTION("Empty payloads match") {
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Mismatched actions do not match") {
    filter_payload.action = steamrot::InputPayload::InputAction::NONE;
    event_payload.action = steamrot::InputPayload::InputAction::SELECT;
    REQUIRE_FALSE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Matching payloads match") {
    filter_payload.action = steamrot::InputPayload::InputAction::SELECT;
    event_payload.action = steamrot::InputPayload::InputAction::SELECT;
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }
}

TEST_CASE("MatchPayload deals with various configurations of UIPayload",
          "[unit][MatchPayload][UIPayload]") {

  using namespace steamrot::events;
  steamrot::UIPayload filter_payload;
  steamrot::UIPayload event_payload;

  SECTION("Empty payloads with same action match") {
    filter_payload.action = steamrot::UIPayload::UIAction::TOGGLE;
    event_payload.action = steamrot::UIPayload::UIAction::TOGGLE;
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Mismatched actions do not match") {
    filter_payload.action = steamrot::UIPayload::UIAction::TOGGLE;
    filter_payload.c_ui_state_name = "menu";
    event_payload.action = steamrot::UIPayload::UIAction::TOGGLE;
    event_payload.c_ui_state_name = "dialog";
    REQUIRE_FALSE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Matching payloads with same ui_state_name match") {
    filter_payload.action = steamrot::UIPayload::UIAction::TOGGLE;
    filter_payload.c_ui_state_name = "menu";
    event_payload.action = steamrot::UIPayload::UIAction::TOGGLE;
    event_payload.c_ui_state_name = "menu";
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Mismatched ui_state_name do not match") {
    filter_payload.c_ui_state_name = "menu";
    event_payload.c_ui_state_name = "dialog";
    REQUIRE_FALSE(MatchPayload(filter_payload, event_payload));
  }
}

TEST_CASE("MatchPayload deals with various configurations of LogicPayload",
          "[unit][MatchPayload][LogicPayload]") {

  using namespace steamrot::events;
  steamrot::LogicPayload filter_payload;
  steamrot::LogicPayload event_payload;

  SECTION("Empty payloads match") {
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Mismatched toggle_name do not match") {
    filter_payload.toggle_name = steamrot::LogicPayload::LogicToggle::NONE;
    event_payload.toggle_name =
        steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD;
    REQUIRE_FALSE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Matching toggle_name match") {
    filter_payload.toggle_name =
        steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD;
    event_payload.toggle_name =
        steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD;
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }
}

TEST_CASE("MatchPayload deals with various configurations of ScenePayload",
          "[unit][MatchPayload][ScenePayload]") {

  using namespace steamrot::events;
  steamrot::ScenePayload filter_payload;
  steamrot::ScenePayload event_payload;

  SECTION("Empty payloads match") {
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Mismatched actions do not match") {
    filter_payload.action = steamrot::ScenePayload::SceneAction::NONE;
    event_payload.action = steamrot::ScenePayload::SceneAction::CHANGE;
    REQUIRE_FALSE(MatchPayload(filter_payload, event_payload));
  }
}

TEST_CASE("MatchPayload deals with various configurations of SystemPayload",
          "[unit][MatchPayload][SystemPayload]") {

  using namespace steamrot::events;
  steamrot::SystemPayload filter_payload;
  steamrot::SystemPayload event_payload;

  SECTION("Empty payloads match") {
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Mismatched actions do not match") {
    filter_payload.action = steamrot::SystemPayload::SystemAction::NONE;
    event_payload.action = steamrot::SystemPayload::SystemAction::QUIT;
    REQUIRE_FALSE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Matching actions match") {
    filter_payload.action = steamrot::SystemPayload::SystemAction::QUIT;
    event_payload.action = steamrot::SystemPayload::SystemAction::QUIT;
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }
}

TEST_CASE("MatchPayload deals with various configurations of EventPayload",
          "[unit][MatchPayload][EventPayload]") {

  using namespace steamrot::events;

  SECTION("Matching InputPayload EventPayloads match") {
    steamrot::EventPayload filter_payload =
        steamrot::InputPayload(steamrot::InputPayload::InputAction::SELECT);
    steamrot::EventPayload event_payload =
        steamrot::InputPayload(steamrot::InputPayload::InputAction::SELECT);
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Mismatched payload variant types do not match") {
    steamrot::EventPayload filter_payload =
        steamrot::InputPayload(steamrot::InputPayload::InputAction::SELECT);
    steamrot::EventPayload event_payload =
        steamrot::UIPayload(steamrot::UIPayload::UIAction::TOGGLE, "menu");
    REQUIRE_FALSE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Matching UIPayload EventPayloads match") {
    steamrot::EventPayload filter_payload =
        steamrot::UIPayload(steamrot::UIPayload::UIAction::TOGGLE, "menu");
    steamrot::EventPayload event_payload =
        steamrot::UIPayload(steamrot::UIPayload::UIAction::TOGGLE, "menu");
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Matching LogicPayload EventPayloads match") {
    steamrot::EventPayload filter_payload = steamrot::LogicPayload(
        steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD);
    steamrot::EventPayload event_payload = steamrot::LogicPayload(
        steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD);
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Matching ScenePayload EventPayloads match") {
    steamrot::EventPayload filter_payload =
        steamrot::ScenePayload(steamrot::ScenePayload::SceneAction::CHANGE,
                               steamrot::SceneType::TITLE);
    steamrot::EventPayload event_payload =
        steamrot::ScenePayload(steamrot::ScenePayload::SceneAction::CHANGE,
                               steamrot::SceneType::TITLE);
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Matching SystemPayload EventPayloads match") {
    steamrot::EventPayload filter_payload =
        steamrot::SystemPayload(steamrot::SystemPayload::SystemAction::QUIT);
    steamrot::EventPayload event_payload =
        steamrot::SystemPayload(steamrot::SystemPayload::SystemAction::QUIT);
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Matching GhostPayload EventPayloads match") {
    steamrot::Fragment fragment;
    steamrot::GhostInstance instance = steamrot::FragmentInstance{&fragment};
    steamrot::EventPayload filter_payload = steamrot::GhostPayload(
        steamrot::GhostPayload::GhostAction::SELECT, instance);
    steamrot::EventPayload event_payload = steamrot::GhostPayload(
        steamrot::GhostPayload::GhostAction::SELECT, instance);
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Mismatched InputPayload EventPayloads do not match") {
    steamrot::EventPayload filter_payload =
        steamrot::InputPayload(steamrot::InputPayload::InputAction::SELECT);
    steamrot::EventPayload event_payload =
        steamrot::InputPayload(steamrot::InputPayload::InputAction::NONE);
    REQUIRE_FALSE(MatchPayload(filter_payload, event_payload));
  }
}

TEST_CASE("MatchPayload deals with various configurations of GhostPayload",
          "[unit][MatchPayload][GhostPayload]") {

  using namespace steamrot::events;
  steamrot::GhostPayload filter_payload;
  steamrot::GhostPayload event_payload;

  SECTION("Empty payloads (monostate instance) match") {
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Mismatched actions do not match") {
    filter_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    event_payload.action = steamrot::GhostPayload::GhostAction::CLEAR;
    REQUIRE_FALSE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Same FragmentInstance pointer matches") {
    steamrot::Fragment fragment;
    filter_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    filter_payload.m_instance = steamrot::FragmentInstance{&fragment};
    event_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    event_payload.m_instance = steamrot::FragmentInstance{&fragment};
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Different FragmentInstance pointers do not match") {
    steamrot::Fragment fragment_a;
    steamrot::Fragment fragment_b;
    filter_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    filter_payload.m_instance = steamrot::FragmentInstance{&fragment_a};
    event_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    event_payload.m_instance = steamrot::FragmentInstance{&fragment_b};
    REQUIRE_FALSE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Mismatched instance variant types do not match") {
    steamrot::Fragment fragment;
    filter_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    filter_payload.m_instance = steamrot::FragmentInstance{&fragment};
    event_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    event_payload.m_instance = std::monostate{};
    REQUIRE_FALSE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("CLEAR action with monostate instances match") {
    filter_payload.action = steamrot::GhostPayload::GhostAction::CLEAR;
    event_payload.action = steamrot::GhostPayload::GhostAction::CLEAR;
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Same JointInstance pointer matches") {
    steamrot::Joint joint;
    filter_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    filter_payload.m_instance = steamrot::JointInstance{&joint};
    event_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    event_payload.m_instance = steamrot::JointInstance{&joint};
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Different JointInstance pointers do not match") {
    steamrot::Joint joint_a;
    steamrot::Joint joint_b;
    filter_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    filter_payload.m_instance = steamrot::JointInstance{&joint_a};
    event_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    event_payload.m_instance = steamrot::JointInstance{&joint_b};
    REQUIRE_FALSE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("JointInstance and FragmentInstance do not match") {
    steamrot::Fragment fragment;
    steamrot::Joint joint;
    filter_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    filter_payload.m_instance = steamrot::JointInstance{&joint};
    event_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    event_payload.m_instance = steamrot::FragmentInstance{&fragment};
    REQUIRE_FALSE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Monostate filter acts as wildcard for any instance type") {
    steamrot::Fragment fragment;
    filter_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    filter_payload.m_instance = std::monostate{};
    event_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    event_payload.m_instance = steamrot::FragmentInstance{&fragment};
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }
}
