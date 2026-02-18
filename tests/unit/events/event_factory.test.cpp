/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for event_factory free functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "event_factory.h"
#include "EventPacket.h"
#include "EventPayload.h"
#include "EventType.h"
#include <catch2/catch_test_macros.hpp>
#include <variant>

TEST_CASE("CreateInputEventPacket: Creates valid EventPacket with "
          "InputPayload",
          "[unit][events][event_factory]") {
  const uint8_t lifetime = 5;
  const auto action = steamrot::InputPayload::InputAction::SELECT;

  auto result = steamrot::events::CreateInputEventPacket(lifetime, action);

  REQUIRE(result.has_value());
  const auto &packet = result.value();

  REQUIRE(packet.context.lifetime == lifetime);
  REQUIRE(packet.type == steamrot::EventType::USER_INPUT);
  REQUIRE(std::holds_alternative<steamrot::InputPayload>(packet.payload));

  const auto &payload = std::get<steamrot::InputPayload>(packet.payload);
  REQUIRE(payload.action == action);
}

TEST_CASE("CreateInputEventPacket: Creates packet with NONE action",
          "[unit][events][event_factory]") {
  const uint8_t lifetime = 1;
  const auto action = steamrot::InputPayload::InputAction::NONE;

  auto result = steamrot::events::CreateInputEventPacket(lifetime, action);

  REQUIRE(result.has_value());
  const auto &packet = result.value();

  const auto &payload = std::get<steamrot::InputPayload>(packet.payload);
  REQUIRE(payload.action == action);
}

TEST_CASE("CreateUIEventPacket: Creates valid EventPacket with UIPayload",
          "[unit][events][event_factory]") {
  const uint8_t lifetime = 3;
  const auto action = steamrot::UIPayload::UIAction::TOGGLE;
  const std::string ui_state_name = "test_ui_state";

  auto result =
      steamrot::events::CreateUIEventPacket(lifetime, action, ui_state_name);

  REQUIRE(result.has_value());
  const auto &packet = result.value();

  REQUIRE(packet.context.lifetime == lifetime);
  REQUIRE(packet.type == steamrot::EventType::UI);
  REQUIRE(std::holds_alternative<steamrot::UIPayload>(packet.payload));

  const auto &payload = std::get<steamrot::UIPayload>(packet.payload);
  REQUIRE(payload.action == action);
  REQUIRE(payload.c_ui_state_name == ui_state_name);
}

TEST_CASE("CreateUIEventPacket: Creates packet with empty UI state name",
          "[unit][events][event_factory]") {
  const uint8_t lifetime = 1;
  const auto action = steamrot::UIPayload::UIAction::TOGGLE;
  const std::string ui_state_name = "";

  auto result =
      steamrot::events::CreateUIEventPacket(lifetime, action, ui_state_name);

  REQUIRE(result.has_value());
  const auto &packet = result.value();

  const auto &payload = std::get<steamrot::UIPayload>(packet.payload);
  REQUIRE(payload.c_ui_state_name.empty());
}

TEST_CASE("CreateLogicEventPacket: Creates valid EventPacket with "
          "LogicPayload",
          "[unit][events][event_factory]") {
  const uint8_t lifetime = 7;
  const auto toggle_name =
      steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD;

  auto result =
      steamrot::events::CreateLogicEventPacket(lifetime, toggle_name);

  REQUIRE(result.has_value());
  const auto &packet = result.value();

  REQUIRE(packet.context.lifetime == lifetime);
  REQUIRE(packet.type == steamrot::EventType::LOGIC);
  REQUIRE(std::holds_alternative<steamrot::LogicPayload>(packet.payload));

  const auto &payload = std::get<steamrot::LogicPayload>(packet.payload);
  REQUIRE(payload.toggle_name == toggle_name);
}

TEST_CASE(
    "CreateLogicEventPacket: Creates packet with CLEAR_MACHINA_FORM_SCAFFOLD",
    "[unit][events][event_factory]") {
  const uint8_t lifetime = 2;
  const auto toggle_name =
      steamrot::LogicPayload::LogicToggle::CLEAR_MACHINA_FORM_SCAFFOLD;

  auto result =
      steamrot::events::CreateLogicEventPacket(lifetime, toggle_name);

  REQUIRE(result.has_value());
  const auto &packet = result.value();

  const auto &payload = std::get<steamrot::LogicPayload>(packet.payload);
  REQUIRE(payload.toggle_name == toggle_name);
}

TEST_CASE("CreateLogicEventPacket: Creates packet with NONE toggle",
          "[unit][events][event_factory]") {
  const uint8_t lifetime = 1;
  const auto toggle_name = steamrot::LogicPayload::LogicToggle::NONE;

  auto result =
      steamrot::events::CreateLogicEventPacket(lifetime, toggle_name);

  REQUIRE(result.has_value());
  const auto &packet = result.value();

  const auto &payload = std::get<steamrot::LogicPayload>(packet.payload);
  REQUIRE(payload.toggle_name == toggle_name);
}

TEST_CASE(
    "CreateSceneEventPacket: Creates valid EventPacket with ScenePayload "
    "(without scene_id)",
    "[unit][events][event_factory]") {
  const uint8_t lifetime = 4;
  const auto action = steamrot::ScenePayload::SceneAction::CHANGE;
  const auto scene_type = steamrot::SceneType::TITLE;

  auto result =
      steamrot::events::CreateSceneEventPacket(lifetime, action, scene_type);

  REQUIRE(result.has_value());
  const auto &packet = result.value();

  REQUIRE(packet.context.lifetime == lifetime);
  REQUIRE(packet.type == steamrot::EventType::SCENE);
  REQUIRE(std::holds_alternative<steamrot::ScenePayload>(packet.payload));

  const auto &payload = std::get<steamrot::ScenePayload>(packet.payload);
  REQUIRE(payload.action == action);
  REQUIRE(payload.scene_type == scene_type);
  REQUIRE_FALSE(payload.scene_id.has_value());
}

TEST_CASE(
    "CreateSceneEventPacket: Creates valid EventPacket with ScenePayload "
    "(with scene_id)",
    "[unit][events][event_factory]") {
  const uint8_t lifetime = 6;
  const auto action = steamrot::ScenePayload::SceneAction::CHANGE;
  const auto scene_type = steamrot::SceneType::CRAFTING;
  const auto scene_id = uuids::uuid_system_generator{}();

  auto result = steamrot::events::CreateSceneEventPacket(lifetime, action,
                                                         scene_type, scene_id);

  REQUIRE(result.has_value());
  const auto &packet = result.value();

  REQUIRE(packet.context.lifetime == lifetime);
  REQUIRE(packet.type == steamrot::EventType::SCENE);
  REQUIRE(std::holds_alternative<steamrot::ScenePayload>(packet.payload));

  const auto &payload = std::get<steamrot::ScenePayload>(packet.payload);
  REQUIRE(payload.action == action);
  REQUIRE(payload.scene_type == scene_type);
  REQUIRE(payload.scene_id.has_value());
  REQUIRE(payload.scene_id.value() == scene_id);
}

TEST_CASE("CreateSceneEventPacket: Creates packet with NONE action",
          "[unit][events][event_factory]") {
  const uint8_t lifetime = 1;
  const auto action = steamrot::ScenePayload::SceneAction::NONE;
  const auto scene_type = steamrot::SceneType::UNKNOWN;

  auto result =
      steamrot::events::CreateSceneEventPacket(lifetime, action, scene_type);

  REQUIRE(result.has_value());
  const auto &packet = result.value();

  const auto &payload = std::get<steamrot::ScenePayload>(packet.payload);
  REQUIRE(payload.action == action);
  REQUIRE(payload.scene_type == scene_type);
}

TEST_CASE("CreateSystemEventPacket: Creates valid EventPacket with "
          "SystemPayload",
          "[unit][events][event_factory]") {
  const uint8_t lifetime = 10;
  const auto action = steamrot::SystemPayload::SystemAction::QUIT;

  auto result = steamrot::events::CreateSystemEventPacket(lifetime, action);

  REQUIRE(result.has_value());
  const auto &packet = result.value();

  REQUIRE(packet.context.lifetime == lifetime);
  REQUIRE(packet.type == steamrot::EventType::SYSTEM);
  REQUIRE(std::holds_alternative<steamrot::SystemPayload>(packet.payload));

  const auto &payload = std::get<steamrot::SystemPayload>(packet.payload);
  REQUIRE(payload.action == action);
}

TEST_CASE("CreateSystemEventPacket: Creates packet with NONE action",
          "[unit][events][event_factory]") {
  const uint8_t lifetime = 1;
  const auto action = steamrot::SystemPayload::SystemAction::NONE;

  auto result = steamrot::events::CreateSystemEventPacket(lifetime, action);

  REQUIRE(result.has_value());
  const auto &packet = result.value();

  const auto &payload = std::get<steamrot::SystemPayload>(packet.payload);
  REQUIRE(payload.action == action);
}

TEST_CASE("Event factory: All functions respect custom lifetime values",
          "[unit][events][event_factory]") {
  const uint8_t custom_lifetime = 42;

  SECTION("InputEventPacket") {
    auto result = steamrot::events::CreateInputEventPacket(
        custom_lifetime, steamrot::InputPayload::InputAction::SELECT);
    REQUIRE(result.has_value());
    REQUIRE(result.value().context.lifetime == custom_lifetime);
  }

  SECTION("UIEventPacket") {
    auto result = steamrot::events::CreateUIEventPacket(
        custom_lifetime, steamrot::UIPayload::UIAction::TOGGLE, "test");
    REQUIRE(result.has_value());
    REQUIRE(result.value().context.lifetime == custom_lifetime);
  }

  SECTION("LogicEventPacket") {
    auto result = steamrot::events::CreateLogicEventPacket(
        custom_lifetime, steamrot::LogicPayload::LogicToggle::NONE);
    REQUIRE(result.has_value());
    REQUIRE(result.value().context.lifetime == custom_lifetime);
  }

  SECTION("SceneEventPacket without scene_id") {
    auto result = steamrot::events::CreateSceneEventPacket(
        custom_lifetime, steamrot::ScenePayload::SceneAction::CHANGE,
        steamrot::SceneType::TITLE);
    REQUIRE(result.has_value());
    REQUIRE(result.value().context.lifetime == custom_lifetime);
  }

  SECTION("SceneEventPacket with scene_id") {
    auto scene_id = uuids::uuid_system_generator{}();
    auto result = steamrot::events::CreateSceneEventPacket(
        custom_lifetime, steamrot::ScenePayload::SceneAction::CHANGE,
        steamrot::SceneType::TITLE, scene_id);
    REQUIRE(result.has_value());
    REQUIRE(result.value().context.lifetime == custom_lifetime);
  }

  SECTION("SystemEventPacket") {
    auto result = steamrot::events::CreateSystemEventPacket(
        custom_lifetime, steamrot::SystemPayload::SystemAction::QUIT);
    REQUIRE(result.has_value());
    REQUIRE(result.value().context.lifetime == custom_lifetime);
  }
}