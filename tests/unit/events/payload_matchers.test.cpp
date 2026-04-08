/////////////////////////////////////////////////
/// @file
/// @brief unit tests for payload matchers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "payload_matchers.h"
#include "EventPayload.h"
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
    steamrot::GhostSelection selection = steamrot::FragmentTag{"iron"};
    steamrot::EventPayload filter_payload = steamrot::GhostPayload(
        steamrot::GhostPayload::GhostAction::SELECT, selection);
    steamrot::EventPayload event_payload = steamrot::GhostPayload(
        steamrot::GhostPayload::GhostAction::SELECT, selection);
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

  SECTION("Empty payloads (monostate selection) match") {
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Mismatched actions do not match") {
    filter_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    event_payload.action = steamrot::GhostPayload::GhostAction::CLEAR;
    REQUIRE_FALSE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Matching FragmentTag selections match") {
    filter_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    filter_payload.m_selection = steamrot::FragmentTag{"copper"};
    event_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    event_payload.m_selection = steamrot::FragmentTag{"copper"};
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Different FragmentTag keys do not match") {
    filter_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    filter_payload.m_selection = steamrot::FragmentTag{"copper"};
    event_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    event_payload.m_selection = steamrot::FragmentTag{"iron"};
    REQUIRE_FALSE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Matching EntityTypeTag selections match") {
    filter_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    filter_payload.m_selection = steamrot::EntityTypeTag{"goblin"};
    event_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    event_payload.m_selection = steamrot::EntityTypeTag{"goblin"};
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("Mismatched selection variant types do not match") {
    filter_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    filter_payload.m_selection = steamrot::FragmentTag{"copper"};
    event_payload.action = steamrot::GhostPayload::GhostAction::SELECT;
    event_payload.m_selection = steamrot::EntityTypeTag{"copper"};
    REQUIRE_FALSE(MatchPayload(filter_payload, event_payload));
  }

  SECTION("CLEAR action with monostate selections match") {
    filter_payload.action = steamrot::GhostPayload::GhostAction::CLEAR;
    event_payload.action = steamrot::GhostPayload::GhostAction::CLEAR;
    REQUIRE(MatchPayload(filter_payload, event_payload));
  }
}
