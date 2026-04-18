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
#include <set>
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

  auto result = steamrot::events::CreateLogicEventPacket(lifetime, toggle_name);

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

  auto result = steamrot::events::CreateLogicEventPacket(lifetime, toggle_name);

  REQUIRE(result.has_value());
  const auto &packet = result.value();

  const auto &payload = std::get<steamrot::LogicPayload>(packet.payload);
  REQUIRE(payload.toggle_name == toggle_name);
}

TEST_CASE("CreateLogicEventPacket: Creates packet with NONE toggle",
          "[unit][events][event_factory]") {
  const uint8_t lifetime = 1;
  const auto toggle_name = steamrot::LogicPayload::LogicToggle::NONE;

  auto result = steamrot::events::CreateLogicEventPacket(lifetime, toggle_name);

  REQUIRE(result.has_value());
  const auto &packet = result.value();

  const auto &payload = std::get<steamrot::LogicPayload>(packet.payload);
  REQUIRE(payload.toggle_name == toggle_name);
}

TEST_CASE("CreateSceneEventPacket: Creates valid EventPacket with ScenePayload "
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
  REQUIRE(payload.optional_scene_type == scene_type);
  REQUIRE_FALSE(payload.optional_scene_id.has_value());
}

TEST_CASE("CreateSceneEventPacket: Creates valid EventPacket with ScenePayload "
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
  REQUIRE(payload.optional_scene_type == scene_type);
  REQUIRE(payload.optional_scene_id.has_value());
  REQUIRE(payload.optional_scene_id.value() == scene_id);
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
  REQUIRE(payload.optional_scene_type == scene_type);
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

TEST_CASE("CreateGhostEventPacket: Creates valid EventPacket with "
          "GhostPayload (FragmentTag)",
          "[unit][events][event_factory]") {
  const uint8_t lifetime = 3;
  const auto action = steamrot::GhostPayload::GhostAction::SELECT;
  const steamrot::GhostSelection selection = steamrot::FragmentTag{"copper"};

  auto result =
      steamrot::events::CreateGhostEventPacket(lifetime, action, selection);

  REQUIRE(result.has_value());
  const auto &packet = result.value();

  REQUIRE(packet.context.lifetime == lifetime);
  REQUIRE(packet.type == steamrot::EventType::GHOST);
  REQUIRE(std::holds_alternative<steamrot::GhostPayload>(packet.payload));

  const auto &payload = std::get<steamrot::GhostPayload>(packet.payload);
  REQUIRE(payload.action == action);
  REQUIRE(
      std::holds_alternative<steamrot::FragmentTag>(payload.m_selection));
  REQUIRE(std::get<steamrot::FragmentTag>(payload.m_selection).key == "copper");
}

TEST_CASE("CreateGhostEventPacket: Creates valid EventPacket with "
          "GhostPayload (JointTag)",
          "[unit][events][event_factory]") {
  const uint8_t lifetime = 3;
  const auto action = steamrot::GhostPayload::GhostAction::SELECT;
  const steamrot::GhostSelection selection = steamrot::JointTag{"pivot"};

  auto result =
      steamrot::events::CreateGhostEventPacket(lifetime, action, selection);

  REQUIRE(result.has_value());
  const auto &packet = result.value();

  REQUIRE(packet.context.lifetime == lifetime);
  REQUIRE(packet.type == steamrot::EventType::GHOST);
  REQUIRE(std::holds_alternative<steamrot::GhostPayload>(packet.payload));

  const auto &payload = std::get<steamrot::GhostPayload>(packet.payload);
  REQUIRE(payload.action == action);
  REQUIRE(std::holds_alternative<steamrot::JointTag>(payload.m_selection));
  REQUIRE(std::get<steamrot::JointTag>(payload.m_selection).key == "pivot");
}

TEST_CASE("CreateGhostEventPacket: Creates packet with CLEAR action",
          "[unit][events][event_factory]") {
  const uint8_t lifetime = 1;
  const auto action = steamrot::GhostPayload::GhostAction::CLEAR;
  const steamrot::GhostSelection selection = std::monostate{};

  auto result =
      steamrot::events::CreateGhostEventPacket(lifetime, action, selection);

  REQUIRE(result.has_value());
  const auto &packet = result.value();

  const auto &payload = std::get<steamrot::GhostPayload>(packet.payload);
  REQUIRE(payload.action == action);
  REQUIRE(std::holds_alternative<std::monostate>(payload.m_selection));
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

  SECTION("GhostEventPacket") {
    const steamrot::GhostSelection selection = std::monostate{};
    auto result = steamrot::events::CreateGhostEventPacket(
        custom_lifetime, steamrot::GhostPayload::GhostAction::CLEAR,
        selection);
    REQUIRE(result.has_value());
    REQUIRE(result.value().context.lifetime == custom_lifetime);
  }
}

TEST_CASE("CreateRandomEventPacket: Generates valid random EventPackets",
          "[unit][events][event_factory]") {
  // Run multiple times to test randomness and validity
  for (int i = 0; i < 20; ++i) {
    auto result = steamrot::events::CreateRandomEventPacket();

    REQUIRE(result.has_value());
    const auto &packet = result.value();

    // Verify lifetime is within valid range (1 to 255)
    REQUIRE(packet.context.lifetime >= 1);
    REQUIRE(packet.context.lifetime <= 255);

    // Verify event type is valid (not NONE)
    REQUIRE(packet.type != steamrot::EventType::NONE);

    // Verify payload matches event type and contains valid data
    switch (packet.type) {
    case steamrot::EventType::USER_INPUT: {
      REQUIRE(std::holds_alternative<steamrot::InputPayload>(packet.payload));
      const auto &payload = std::get<steamrot::InputPayload>(packet.payload);
      // Verify action is valid (not NONE for random generation)
      REQUIRE(payload.action != steamrot::InputPayload::InputAction::NONE);
      break;
    }

    case steamrot::EventType::UI: {
      REQUIRE(std::holds_alternative<steamrot::UIPayload>(packet.payload));
      const auto &payload = std::get<steamrot::UIPayload>(packet.payload);
      // Verify UI state name is not empty
      REQUIRE_FALSE(payload.c_ui_state_name.empty());
      break;
    }

    case steamrot::EventType::LOGIC: {
      REQUIRE(std::holds_alternative<steamrot::LogicPayload>(packet.payload));
      const auto &payload = std::get<steamrot::LogicPayload>(packet.payload);
      // Verify toggle is valid (not NONE for random generation)
      REQUIRE(payload.toggle_name != steamrot::LogicPayload::LogicToggle::NONE);
      break;
    }

    case steamrot::EventType::SCENE: {
      REQUIRE(std::holds_alternative<steamrot::ScenePayload>(packet.payload));
      const auto &payload = std::get<steamrot::ScenePayload>(packet.payload);
      // Verify action is valid (not NONE for random generation)
      REQUIRE(payload.action != steamrot::ScenePayload::SceneAction::NONE);
      // Verify scene type is valid (not UNKNOWN for random generation)
      REQUIRE(payload.optional_scene_type != steamrot::SceneType::UNKNOWN);
      break;
    }

    case steamrot::EventType::SYSTEM: {
      REQUIRE(std::holds_alternative<steamrot::SystemPayload>(packet.payload));
      const auto &payload = std::get<steamrot::SystemPayload>(packet.payload);
      // Verify action is valid (not NONE for random generation)
      REQUIRE(payload.action != steamrot::SystemPayload::SystemAction::NONE);
      break;
    }

    case steamrot::EventType::GHOST: {
      REQUIRE(std::holds_alternative<steamrot::GhostPayload>(packet.payload));
      const auto &payload = std::get<steamrot::GhostPayload>(packet.payload);
      // Random generation always uses CLEAR with monostate selection
      REQUIRE(payload.action == steamrot::GhostPayload::GhostAction::CLEAR);
      REQUIRE(std::holds_alternative<std::monostate>(payload.m_selection));
      break;
    }

    case steamrot::EventType::CAMERA: {
      REQUIRE(std::holds_alternative<steamrot::CameraPayload>(packet.payload));
      break;
    }

    default:
      FAIL("Unexpected event type encountered");
    }
  }
}

TEST_CASE("CreateRandomEventPacket: Generates different packets across "
          "multiple calls",
          "[unit][events][event_factory]") {
  // Generate multiple packets and verify they're not all identical
  std::vector<steamrot::EventPacket> packets;
  for (int i = 0; i < 10; ++i) {
    auto result = steamrot::events::CreateRandomEventPacket();
    REQUIRE(result.has_value());
    packets.push_back(result.value());
  }

  // Check that not all packets have the same event type
  bool has_variation = false;
  auto first_type = packets[0].type;
  for (size_t i = 1; i < packets.size(); ++i) {
    if (packets[i].type != first_type) {
      has_variation = true;
      break;
    }
  }

  // Note: There's a small chance this could fail due to randomness,
  // but with 10 packets and 6 event types, it's very unlikely
  REQUIRE(has_variation);
}

TEST_CASE("CreateRandomEventPacket: Lifetime varies across calls",
          "[unit][events][event_factory]") {
  // Generate multiple packets and verify lifetimes vary
  std::set<uint8_t> unique_lifetimes;
  for (int i = 0; i < 20; ++i) {
    auto result = steamrot::events::CreateRandomEventPacket();
    REQUIRE(result.has_value());
    unique_lifetimes.insert(result.value().context.lifetime);
  }

  // With 20 calls, we should see at least some variation
  // (very unlikely to get the same lifetime 20 times)
  REQUIRE(unique_lifetimes.size() > 1);
}
