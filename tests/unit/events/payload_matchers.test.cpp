/////////////////////////////////////////////////
/// @file
/// @brief unit tests for payload matchers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "payload_matchers.h"
#include "EventPacket.h"
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

TEST_CASE("MatchEventPacket matches EventPackets correctly",
          "[unit][MatchEventPacket]") {

  using namespace steamrot::events;

  SECTION("Matching InputPayload EventPackets match") {
    steamrot::EventPacket filter_packet;
    filter_packet.type = steamrot::EventType::USER_INPUT;
    filter_packet.payload =
        steamrot::InputPayload(steamrot::InputPayload::InputAction::SELECT);

    steamrot::EventPacket event_packet;
    event_packet.type = steamrot::EventType::USER_INPUT;
    event_packet.payload =
        steamrot::InputPayload(steamrot::InputPayload::InputAction::SELECT);

    REQUIRE(MatchEventPacket(filter_packet, event_packet));
  }

  SECTION("Mismatched EventType do not match") {
    steamrot::EventPacket filter_packet;
    filter_packet.type = steamrot::EventType::USER_INPUT;
    filter_packet.payload =
        steamrot::InputPayload(steamrot::InputPayload::InputAction::SELECT);

    steamrot::EventPacket event_packet;
    event_packet.type = steamrot::EventType::UI;
    event_packet.payload =
        steamrot::InputPayload(steamrot::InputPayload::InputAction::SELECT);

    REQUIRE_FALSE(MatchEventPacket(filter_packet, event_packet));
  }

  SECTION("Mismatched payload variant types do not match") {
    steamrot::EventPacket filter_packet;
    filter_packet.type = steamrot::EventType::USER_INPUT;
    filter_packet.payload =
        steamrot::InputPayload(steamrot::InputPayload::InputAction::SELECT);

    steamrot::EventPacket event_packet;
    event_packet.type = steamrot::EventType::USER_INPUT;
    event_packet.payload =
        steamrot::UIPayload(steamrot::UIPayload::UIAction::TOGGLE, "menu");

    REQUIRE_FALSE(MatchEventPacket(filter_packet, event_packet));
  }

  SECTION("Matching UIPayload EventPackets match") {
    steamrot::EventPacket filter_packet;
    filter_packet.type = steamrot::EventType::UI;
    filter_packet.payload =
        steamrot::UIPayload(steamrot::UIPayload::UIAction::TOGGLE, "menu");

    steamrot::EventPacket event_packet;
    event_packet.type = steamrot::EventType::UI;
    event_packet.payload =
        steamrot::UIPayload(steamrot::UIPayload::UIAction::TOGGLE, "menu");

    REQUIRE(MatchEventPacket(filter_packet, event_packet));
  }

  SECTION("Matching LogicPayload EventPackets match") {
    steamrot::EventPacket filter_packet;
    filter_packet.type = steamrot::EventType::LOGIC;
    filter_packet.payload = steamrot::LogicPayload(
        steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD);

    steamrot::EventPacket event_packet;
    event_packet.type = steamrot::EventType::LOGIC;
    event_packet.payload = steamrot::LogicPayload(
        steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD);

    REQUIRE(MatchEventPacket(filter_packet, event_packet));
  }

  SECTION("Matching ScenePayload EventPackets match") {
    steamrot::EventPacket filter_packet;
    filter_packet.type = steamrot::EventType::SCENE;
    filter_packet.payload =
        steamrot::ScenePayload(steamrot::ScenePayload::SceneAction::CHANGE,
                               steamrot::SceneType::TITLE);

    steamrot::EventPacket event_packet;
    event_packet.type = steamrot::EventType::SCENE;
    event_packet.payload =
        steamrot::ScenePayload(steamrot::ScenePayload::SceneAction::CHANGE,
                               steamrot::SceneType::TITLE);

    REQUIRE(MatchEventPacket(filter_packet, event_packet));
  }

  SECTION("Matching SystemPayload EventPackets match") {
    steamrot::EventPacket filter_packet;
    filter_packet.type = steamrot::EventType::SYSTEM;
    filter_packet.payload =
        steamrot::SystemPayload(steamrot::SystemPayload::SystemAction::QUIT);

    steamrot::EventPacket event_packet;
    event_packet.type = steamrot::EventType::SYSTEM;
    event_packet.payload =
        steamrot::SystemPayload(steamrot::SystemPayload::SystemAction::QUIT);

    REQUIRE(MatchEventPacket(filter_packet, event_packet));
  }

  SECTION("Mismatched InputPayload EventPackets do not match") {
    steamrot::EventPacket filter_packet;
    filter_packet.type = steamrot::EventType::USER_INPUT;
    filter_packet.payload =
        steamrot::InputPayload(steamrot::InputPayload::InputAction::SELECT);

    steamrot::EventPacket event_packet;
    filter_packet.type = steamrot::EventType::USER_INPUT;
    event_packet.payload =
        steamrot::InputPayload(steamrot::InputPayload::InputAction::NONE);

    REQUIRE_FALSE(MatchEventPacket(filter_packet, event_packet));
  }
}
