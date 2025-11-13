/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EventPacket and EventBus custom matchers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "event_matchers.h"
#include "EventHandler.h"
#include "EventPacket.h"
#include "UserInputBitset.h"
#include "events_generated.h"
#include "scene_change_packet_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

TEST_CASE("EventPacket matcher compares m_event_type correctly",
          "[unit][EventPacket][matcher]") {
  steamrot::EventPacket expected{1};
  expected.m_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;

  steamrot::EventPacket actual{1};

  SECTION("Matcher detects m_event_type differences") {
    actual.m_event_type = steamrot::EventType::EventType_EVENT_QUIT_GAME;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects m_event_type equality") {
    actual.m_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
    REQUIRE_THAT(actual, steamrot::tests::EqualsEventPacket(expected));
  }
}

TEST_CASE("EventPacket matcher compares event_lifetime correctly",
          "[unit][EventPacket][matcher]") {
  steamrot::EventPacket expected{3};
  steamrot::EventPacket actual{1};

  SECTION("Matcher detects event_lifetime differences") {
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects event_lifetime equality") {
    actual.event_lifetime = 3;
    REQUIRE_THAT(actual, steamrot::tests::EqualsEventPacket(expected));
  }
}

TEST_CASE("EventPacket matcher compares event_id correctly",
          "[unit][EventPacket][matcher]") {
  steamrot::EventPacket expected{1};
  expected.event_id = uuids::uuid_system_generator{}();

  steamrot::EventPacket actual{1};
  actual.event_id = uuids::uuid_system_generator{}();

  SECTION("Matcher detects event_id differences") {
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects event_id equality") {
    actual.event_id = expected.event_id;
    REQUIRE_THAT(actual, steamrot::tests::EqualsEventPacket(expected));
  }
}

TEST_CASE("EventPacket matcher compares source_id correctly",
          "[unit][EventPacket][matcher]") {
  steamrot::EventPacket expected{1};
  expected.source_id = uuids::uuid_system_generator{}();

  steamrot::EventPacket actual{1};
  actual.source_id = uuids::uuid_system_generator{}();

  SECTION("Matcher detects source_id differences") {
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects source_id equality") {
    actual.source_id = expected.source_id;
    REQUIRE_THAT(actual, steamrot::tests::EqualsEventPacket(expected));
  }
}

TEST_CASE("EventPacket matcher compares m_event_data with monostate",
          "[unit][EventPacket][matcher]") {
  steamrot::EventPacket expected{1};
  expected.m_event_data = std::monostate{};

  steamrot::EventPacket actual{1};

  SECTION("Matcher detects monostate equality") {
    actual.m_event_data = std::monostate{};
    REQUIRE_THAT(actual, steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects differences when actual has different variant type") {
    actual.m_event_data = steamrot::UserInputBitset{};
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }
}

TEST_CASE("EventPacket matcher compares m_event_data with UserInputBitset",
          "[unit][EventPacket][matcher]") {
  sf::Event::KeyPressed key_event;
  key_event.code = sf::Keyboard::Key::A;
  sf::Event event{key_event};

  steamrot::EventPacket expected{1};
  expected.m_event_data = steamrot::UserInputBitset{{event}};

  steamrot::EventPacket actual{1};

  SECTION("Matcher detects UserInputBitset equality") {
    actual.m_event_data = steamrot::UserInputBitset{{event}};
    REQUIRE_THAT(actual, steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects UserInputBitset differences") {
    sf::Event::KeyPressed different_key_event;
    different_key_event.code = sf::Keyboard::Key::B;
    sf::Event different_event{different_key_event};
    actual.m_event_data = steamrot::UserInputBitset{{different_event}};
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects variant type differences") {
    actual.m_event_data = std::monostate{};
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }
}

TEST_CASE("EventPacket matcher compares m_event_data with SceneChangePacket",
          "[unit][EventPacket][matcher]") {
  uuids::uuid test_uuid = uuids::uuid_system_generator{}();
  steamrot::SceneType scene_type = steamrot::SceneType::SceneType_TITLE_SCREEN;

  steamrot::EventPacket expected{1};
  expected.m_event_data =
      steamrot::SceneChangePacket{test_uuid, scene_type};

  steamrot::EventPacket actual{1};

  SECTION("Matcher detects SceneChangePacket equality") {
    actual.m_event_data = steamrot::SceneChangePacket{test_uuid, scene_type};
    REQUIRE_THAT(actual, steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects SceneChangePacket UUID differences") {
    uuids::uuid different_uuid = uuids::uuid_system_generator{}();
    actual.m_event_data =
        steamrot::SceneChangePacket{different_uuid, scene_type};
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects SceneChangePacket SceneType differences") {
    actual.m_event_data = steamrot::SceneChangePacket{
        test_uuid, steamrot::SceneType::SceneType_CRAFTING_SCREEN};
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects SceneChangePacket with optional UUID presence "
          "differences") {
    steamrot::EventPacket expected_no_uuid{1};
    expected_no_uuid.m_event_data =
        steamrot::SceneChangePacket{std::nullopt, scene_type};

    steamrot::EventPacket actual_with_uuid{1};
    actual_with_uuid.m_event_data =
        steamrot::SceneChangePacket{test_uuid, scene_type};

    REQUIRE_THAT(actual_with_uuid,
                 !steamrot::tests::EqualsEventPacket(expected_no_uuid));
  }

  SECTION("Matcher detects SceneChangePacket equality with no UUID") {
    steamrot::EventPacket expected_no_uuid{1};
    expected_no_uuid.m_event_data =
        steamrot::SceneChangePacket{std::nullopt, scene_type};

    steamrot::EventPacket actual_no_uuid{1};
    actual_no_uuid.m_event_data =
        steamrot::SceneChangePacket{std::nullopt, scene_type};

    REQUIRE_THAT(actual_no_uuid,
                 steamrot::tests::EqualsEventPacket(expected_no_uuid));
  }
}

TEST_CASE("EventPacket matcher compares m_event_data with UIElementName",
          "[unit][EventPacket][matcher]") {
  steamrot::EventPacket expected{1};
  expected.m_event_data = steamrot::UIElementName{"TestElement"};

  steamrot::EventPacket actual{1};

  SECTION("Matcher detects UIElementName equality") {
    actual.m_event_data = steamrot::UIElementName{"TestElement"};
    REQUIRE_THAT(actual, steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects UIElementName differences") {
    actual.m_event_data = steamrot::UIElementName{"DifferentElement"};
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }
}

TEST_CASE("EventPacket matcher works with complete EventPacket comparison",
          "[unit][EventPacket][matcher]") {
  uuids::uuid event_uuid = uuids::uuid_system_generator{}();
  uuids::uuid source_uuid = uuids::uuid_system_generator{}();

  sf::Event::KeyPressed key_event;
  key_event.code = sf::Keyboard::Key::Space;
  sf::Event event{key_event};

  steamrot::EventPacket expected{2};
  expected.m_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
  expected.m_event_data = steamrot::UserInputBitset{{event}};
  expected.event_id = event_uuid;
  expected.source_id = source_uuid;

  steamrot::EventPacket actual{2};
  actual.m_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
  actual.m_event_data = steamrot::UserInputBitset{{event}};
  actual.event_id = event_uuid;
  actual.source_id = source_uuid;

  SECTION("Matcher detects complete equality") {
    REQUIRE_THAT(actual, steamrot::tests::EqualsEventPacket(expected));
  }

  SECTION("Matcher detects any field difference") {
    actual.event_lifetime = 3;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventPacket(expected));
  }
}

TEST_CASE("EventBus matcher compares empty EventBus correctly",
          "[unit][EventBus][matcher]") {
  steamrot::EventBus expected;
  steamrot::EventBus actual;

  REQUIRE_THAT(actual, steamrot::tests::EqualsEventBus(expected));
}

TEST_CASE("EventBus matcher detects size differences",
          "[unit][EventBus][matcher]") {
  steamrot::EventBus expected;
  expected.push_back(steamrot::EventPacket{1});

  steamrot::EventBus actual;

  SECTION("Matcher detects when actual is empty") {
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventBus(expected));
  }

  SECTION("Matcher detects when sizes differ") {
    actual.push_back(steamrot::EventPacket{1});
    actual.push_back(steamrot::EventPacket{2});
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventBus(expected));
  }
}

TEST_CASE("EventBus matcher compares single EventPacket correctly",
          "[unit][EventBus][matcher]") {
  steamrot::EventBus expected;
  steamrot::EventPacket expected_packet{3};
  expected_packet.m_event_type = steamrot::EventType::EventType_EVENT_TEST;
  expected.push_back(expected_packet);

  steamrot::EventBus actual;

  SECTION("Matcher detects equality") {
    steamrot::EventPacket actual_packet{3};
    actual_packet.m_event_type = steamrot::EventType::EventType_EVENT_TEST;
    actual.push_back(actual_packet);
    REQUIRE_THAT(actual, steamrot::tests::EqualsEventBus(expected));
  }

  SECTION("Matcher detects EventPacket differences") {
    steamrot::EventPacket actual_packet{3};
    actual_packet.m_event_type = steamrot::EventType::EventType_EVENT_QUIT_GAME;
    actual.push_back(actual_packet);
    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventBus(expected));
  }
}

TEST_CASE("EventBus matcher compares multiple EventPackets correctly",
          "[unit][EventBus][matcher]") {
  steamrot::EventBus expected;
  
  steamrot::EventPacket packet1{1};
  packet1.m_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
  expected.push_back(packet1);

  steamrot::EventPacket packet2{2};
  packet2.m_event_type = steamrot::EventType::EventType_EVENT_TEST;
  expected.push_back(packet2);

  steamrot::EventPacket packet3{3};
  packet3.m_event_type = steamrot::EventType::EventType_EVENT_QUIT_GAME;
  expected.push_back(packet3);

  steamrot::EventBus actual;

  SECTION("Matcher detects equality with multiple packets") {
    steamrot::EventPacket actual_packet1{1};
    actual_packet1.m_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
    actual.push_back(actual_packet1);

    steamrot::EventPacket actual_packet2{2};
    actual_packet2.m_event_type = steamrot::EventType::EventType_EVENT_TEST;
    actual.push_back(actual_packet2);

    steamrot::EventPacket actual_packet3{3};
    actual_packet3.m_event_type = steamrot::EventType::EventType_EVENT_QUIT_GAME;
    actual.push_back(actual_packet3);

    REQUIRE_THAT(actual, steamrot::tests::EqualsEventBus(expected));
  }

  SECTION("Matcher detects differences in middle packet") {
    steamrot::EventPacket actual_packet1{1};
    actual_packet1.m_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
    actual.push_back(actual_packet1);

    steamrot::EventPacket actual_packet2{2};
    actual_packet2.m_event_type = steamrot::EventType::EventType_EVENT_QUIT_GAME; // Different
    actual.push_back(actual_packet2);

    steamrot::EventPacket actual_packet3{3};
    actual_packet3.m_event_type = steamrot::EventType::EventType_EVENT_QUIT_GAME;
    actual.push_back(actual_packet3);

    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventBus(expected));
  }

  SECTION("Matcher detects differences in last packet") {
    steamrot::EventPacket actual_packet1{1};
    actual_packet1.m_event_type = steamrot::EventType::EventType_EVENT_USER_INPUT;
    actual.push_back(actual_packet1);

    steamrot::EventPacket actual_packet2{2};
    actual_packet2.m_event_type = steamrot::EventType::EventType_EVENT_TEST;
    actual.push_back(actual_packet2);

    steamrot::EventPacket actual_packet3{5}; // Different lifetime
    actual_packet3.m_event_type = steamrot::EventType::EventType_EVENT_QUIT_GAME;
    actual.push_back(actual_packet3);

    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventBus(expected));
  }
}

TEST_CASE("EventBus matcher compares EventPackets with complex event data",
          "[unit][EventBus][matcher]") {
  sf::Event::KeyPressed key_event;
  key_event.code = sf::Keyboard::Key::Enter;
  sf::Event event{key_event};

  steamrot::EventBus expected;
  
  steamrot::EventPacket packet1{1};
  packet1.m_event_data = steamrot::UserInputBitset{{event}};
  expected.push_back(packet1);

  steamrot::EventPacket packet2{2};
  packet2.m_event_data = steamrot::UIElementName{"Button1"};
  expected.push_back(packet2);

  uuids::uuid test_uuid = uuids::uuid_system_generator{}();
  steamrot::EventPacket packet3{3};
  packet3.m_event_data = steamrot::SceneChangePacket{
      test_uuid, steamrot::SceneType::SceneType_CRAFTING_SCREEN};
  expected.push_back(packet3);

  steamrot::EventBus actual;

  SECTION("Matcher detects equality with complex event data") {
    steamrot::EventPacket actual_packet1{1};
    actual_packet1.m_event_data = steamrot::UserInputBitset{{event}};
    actual.push_back(actual_packet1);

    steamrot::EventPacket actual_packet2{2};
    actual_packet2.m_event_data = steamrot::UIElementName{"Button1"};
    actual.push_back(actual_packet2);

    steamrot::EventPacket actual_packet3{3};
    actual_packet3.m_event_data = steamrot::SceneChangePacket{
        test_uuid, steamrot::SceneType::SceneType_CRAFTING_SCREEN};
    actual.push_back(actual_packet3);

    REQUIRE_THAT(actual, steamrot::tests::EqualsEventBus(expected));
  }

  SECTION("Matcher detects UIElementName differences in EventBus") {
    steamrot::EventPacket actual_packet1{1};
    actual_packet1.m_event_data = steamrot::UserInputBitset{{event}};
    actual.push_back(actual_packet1);

    steamrot::EventPacket actual_packet2{2};
    actual_packet2.m_event_data = steamrot::UIElementName{"Button2"}; // Different
    actual.push_back(actual_packet2);

    steamrot::EventPacket actual_packet3{3};
    actual_packet3.m_event_data = steamrot::SceneChangePacket{
        test_uuid, steamrot::SceneType::SceneType_CRAFTING_SCREEN};
    actual.push_back(actual_packet3);

    REQUIRE_THAT(actual, !steamrot::tests::EqualsEventBus(expected));
  }
}
