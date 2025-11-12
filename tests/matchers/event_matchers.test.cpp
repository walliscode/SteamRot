/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EventPacket and EventBus matchers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "event_bus_matchers.h"
#include "event_packet_matchers.h"
#include "EventHandler.h"
#include "EventPacket.h"
#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <random>

TEST_CASE("EventPacket matcher compares event_type correctly",
          "[unit][event_packet_matchers]") {
  steamrot::EventPacket packet1(steamrot::EventType::EventType_EVENT_QUIT_GAME,
                                steamrot::EventData{std::monostate{}}, 1);
  steamrot::EventPacket packet2(steamrot::EventType::EventType_EVENT_QUIT_GAME,
                                steamrot::EventData{std::monostate{}}, 1);
  steamrot::EventPacket packet3(
      steamrot::EventType::EventType_EVENT_CHANGE_SCENE,
      steamrot::EventData{std::monostate{}}, 1);

  // Same event_type should match
  REQUIRE_THAT(packet1, steamrot::tests::EqualsEventPacket(packet2));

  // Different event_type should not match
  REQUIRE_FALSE(
      steamrot::tests::EqualsEventPacket(packet3).match(packet1));
}

TEST_CASE("EventPacket matcher compares event_lifetime correctly",
          "[unit][event_packet_matchers]") {
  steamrot::EventPacket packet1(steamrot::EventType::EventType_EVENT_QUIT_GAME,
                                steamrot::EventData{std::monostate{}}, 1);
  steamrot::EventPacket packet2(steamrot::EventType::EventType_EVENT_QUIT_GAME,
                                steamrot::EventData{std::monostate{}}, 5);

  // Different event_lifetime should not match
  REQUIRE_FALSE(
      steamrot::tests::EqualsEventPacket(packet2).match(packet1));
}

TEST_CASE("EventPacket matcher compares monostate EventData correctly",
          "[unit][event_packet_matchers]") {
  steamrot::EventPacket packet1(steamrot::EventType::EventType_EVENT_QUIT_GAME,
                                steamrot::EventData{std::monostate{}}, 1);
  steamrot::EventPacket packet2(steamrot::EventType::EventType_EVENT_QUIT_GAME,
                                steamrot::EventData{std::monostate{}}, 1);

  // Both monostate should match
  REQUIRE_THAT(packet1, steamrot::tests::EqualsEventPacket(packet2));
}

TEST_CASE("EventPacket matcher compares UserInputBitset EventData correctly",
          "[unit][event_packet_matchers]") {
  steamrot::UserInputBitset bitset1;
  bitset1.setKeyPressed(sf::Keyboard::Key::A);

  steamrot::UserInputBitset bitset2;
  bitset2.setKeyPressed(sf::Keyboard::Key::A);

  steamrot::UserInputBitset bitset3;
  bitset3.setKeyPressed(sf::Keyboard::Key::B);

  steamrot::EventPacket packet1(steamrot::EventType::EventType_EVENT_USER_INPUT,
                                steamrot::EventData{bitset1}, 1);
  steamrot::EventPacket packet2(steamrot::EventType::EventType_EVENT_USER_INPUT,
                                steamrot::EventData{bitset2}, 1);
  steamrot::EventPacket packet3(steamrot::EventType::EventType_EVENT_USER_INPUT,
                                steamrot::EventData{bitset3}, 1);

  // Same bitset should match
  REQUIRE_THAT(packet1, steamrot::tests::EqualsEventPacket(packet2));

  // Different bitset should not match
  REQUIRE_FALSE(
      steamrot::tests::EqualsEventPacket(packet3).match(packet1));
}

TEST_CASE("EventPacket matcher compares UIElementName EventData correctly",
          "[unit][event_packet_matchers]") {
  steamrot::EventPacket packet1(
      steamrot::EventType::EventType_EVENT_TOGGLE_DROPDOWN,
      steamrot::EventData{steamrot::UIElementName{"button1"}}, 1);
  steamrot::EventPacket packet2(
      steamrot::EventType::EventType_EVENT_TOGGLE_DROPDOWN,
      steamrot::EventData{steamrot::UIElementName{"button1"}}, 1);
  steamrot::EventPacket packet3(
      steamrot::EventType::EventType_EVENT_TOGGLE_DROPDOWN,
      steamrot::EventData{steamrot::UIElementName{"button2"}}, 1);

  // Same UIElementName should match
  REQUIRE_THAT(packet1, steamrot::tests::EqualsEventPacket(packet2));

  // Different UIElementName should not match
  REQUIRE_FALSE(
      steamrot::tests::EqualsEventPacket(packet3).match(packet1));
}

TEST_CASE("EventPacket matcher compares SceneChangePacket EventData correctly",
          "[unit][event_packet_matchers]") {
  // Create UUID for testing
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 generator(seq);
  uuids::uuid_random_generator gen{generator};

  auto uuid1 = gen();
  auto uuid2 = gen();

  steamrot::SceneChangePacket scene_packet1{
      std::optional<uuids::uuid>{uuid1},
      steamrot::SceneType::SceneType_CRAFTING};
  steamrot::SceneChangePacket scene_packet2{
      std::optional<uuids::uuid>{uuid1},
      steamrot::SceneType::SceneType_CRAFTING};
  steamrot::SceneChangePacket scene_packet3{
      std::optional<uuids::uuid>{uuid2},
      steamrot::SceneType::SceneType_CRAFTING};
  steamrot::SceneChangePacket scene_packet4{
      std::optional<uuids::uuid>{uuid1},
      steamrot::SceneType::SceneType_TITLE};
  steamrot::SceneChangePacket scene_packet5{
      std::nullopt, steamrot::SceneType::SceneType_CRAFTING};

  steamrot::EventPacket packet1(
      steamrot::EventType::EventType_EVENT_CHANGE_SCENE,
      steamrot::EventData{scene_packet1}, 1);
  steamrot::EventPacket packet2(
      steamrot::EventType::EventType_EVENT_CHANGE_SCENE,
      steamrot::EventData{scene_packet2}, 1);
  steamrot::EventPacket packet3(
      steamrot::EventType::EventType_EVENT_CHANGE_SCENE,
      steamrot::EventData{scene_packet3}, 1);
  steamrot::EventPacket packet4(
      steamrot::EventType::EventType_EVENT_CHANGE_SCENE,
      steamrot::EventData{scene_packet4}, 1);
  steamrot::EventPacket packet5(
      steamrot::EventType::EventType_EVENT_CHANGE_SCENE,
      steamrot::EventData{scene_packet5}, 1);

  // Same SceneChangePacket should match
  REQUIRE_THAT(packet1, steamrot::tests::EqualsEventPacket(packet2));

  // Different UUID should not match
  REQUIRE_FALSE(
      steamrot::tests::EqualsEventPacket(packet3).match(packet1));

  // Different SceneType should not match
  REQUIRE_FALSE(
      steamrot::tests::EqualsEventPacket(packet4).match(packet1));

  // Different UUID presence should not match
  REQUIRE_FALSE(
      steamrot::tests::EqualsEventPacket(packet5).match(packet1));
}

TEST_CASE("EventPacket matcher compares different EventData types correctly",
          "[unit][event_packet_matchers]") {
  steamrot::EventPacket packet1(steamrot::EventType::EventType_EVENT_USER_INPUT,
                                steamrot::EventData{std::monostate{}}, 1);
  steamrot::EventPacket packet2(
      steamrot::EventType::EventType_EVENT_USER_INPUT,
      steamrot::EventData{steamrot::UserInputBitset{}}, 1);

  // Different EventData types should not match
  REQUIRE_FALSE(
      steamrot::tests::EqualsEventPacket(packet2).match(packet1));
}

TEST_CASE("EventPacket matcher provides detailed mismatch description",
          "[unit][event_packet_matchers]") {
  steamrot::EventPacket packet1(steamrot::EventType::EventType_EVENT_QUIT_GAME,
                                steamrot::EventData{std::monostate{}}, 1);
  steamrot::EventPacket packet2(
      steamrot::EventType::EventType_EVENT_CHANGE_SCENE,
      steamrot::EventData{std::monostate{}}, 5);

  steamrot::tests::EventPacketEqualsMatcher matcher(packet2);
  REQUIRE_FALSE(matcher.match(packet1));

  // Check that description contains relevant mismatch information
  std::string description = matcher.describe();
  REQUIRE(description.find("mismatch") != std::string::npos);
}

TEST_CASE("EventBus matcher compares empty EventBus correctly",
          "[unit][event_bus_matchers]") {
  steamrot::EventBus bus1;
  steamrot::EventBus bus2;

  // Both empty should match
  REQUIRE_THAT(bus1, steamrot::tests::EqualsEventBus(bus2));
}

TEST_CASE("EventBus matcher compares EventBus size correctly",
          "[unit][event_bus_matchers]") {
  steamrot::EventBus bus1;
  bus1.push_back(steamrot::EventPacket(
      steamrot::EventType::EventType_EVENT_QUIT_GAME,
      steamrot::EventData{std::monostate{}}, 1));

  steamrot::EventBus bus2;

  // Different sizes should not match
  REQUIRE_FALSE(steamrot::tests::EqualsEventBus(bus2).match(bus1));
}

TEST_CASE("EventBus matcher compares EventBus with same events correctly",
          "[unit][event_bus_matchers]") {
  steamrot::EventBus bus1;
  bus1.push_back(steamrot::EventPacket(
      steamrot::EventType::EventType_EVENT_QUIT_GAME,
      steamrot::EventData{std::monostate{}}, 1));
  bus1.push_back(steamrot::EventPacket(
      steamrot::EventType::EventType_EVENT_USER_INPUT,
      steamrot::EventData{steamrot::UserInputBitset{}}, 2));

  steamrot::EventBus bus2;
  bus2.push_back(steamrot::EventPacket(
      steamrot::EventType::EventType_EVENT_QUIT_GAME,
      steamrot::EventData{std::monostate{}}, 1));
  bus2.push_back(steamrot::EventPacket(
      steamrot::EventType::EventType_EVENT_USER_INPUT,
      steamrot::EventData{steamrot::UserInputBitset{}}, 2));

  // Same events should match
  REQUIRE_THAT(bus1, steamrot::tests::EqualsEventBus(bus2));
}

TEST_CASE("EventBus matcher compares EventBus with different events correctly",
          "[unit][event_bus_matchers]") {
  steamrot::EventBus bus1;
  bus1.push_back(steamrot::EventPacket(
      steamrot::EventType::EventType_EVENT_QUIT_GAME,
      steamrot::EventData{std::monostate{}}, 1));
  bus1.push_back(steamrot::EventPacket(
      steamrot::EventType::EventType_EVENT_USER_INPUT,
      steamrot::EventData{steamrot::UserInputBitset{}}, 2));

  steamrot::EventBus bus2;
  bus2.push_back(steamrot::EventPacket(
      steamrot::EventType::EventType_EVENT_QUIT_GAME,
      steamrot::EventData{std::monostate{}}, 1));
  bus2.push_back(steamrot::EventPacket(
      steamrot::EventType::EventType_EVENT_CHANGE_SCENE,
      steamrot::EventData{std::monostate{}}, 2));

  // Different events should not match
  REQUIRE_FALSE(steamrot::tests::EqualsEventBus(bus2).match(bus1));
}

TEST_CASE("EventBus matcher provides detailed mismatch description",
          "[unit][event_bus_matchers]") {
  steamrot::EventBus bus1;
  bus1.push_back(steamrot::EventPacket(
      steamrot::EventType::EventType_EVENT_QUIT_GAME,
      steamrot::EventData{std::monostate{}}, 1));

  steamrot::EventBus bus2;

  steamrot::tests::EventBusEqualsMatcher matcher(bus2);
  REQUIRE_FALSE(matcher.match(bus1));

  // Check that description contains size mismatch information
  std::string description = matcher.describe();
  REQUIRE(description.find("size differs") != std::string::npos);
}

TEST_CASE("EventBus matcher supports test metadata",
          "[unit][event_bus_matchers]") {
  steamrot::EventBus bus1;
  bus1.push_back(steamrot::EventPacket(
      steamrot::EventType::EventType_EVENT_QUIT_GAME,
      steamrot::EventData{std::monostate{}}, 1));

  steamrot::EventBus bus2;

  steamrot::tests::EventBusEqualsMatcher matcher(bus2, "Test: my_test");
  REQUIRE_FALSE(matcher.match(bus1));

  // Check that description contains test metadata
  std::string description = matcher.describe();
  REQUIRE(description.find("Test: my_test") != std::string::npos);
}

TEST_CASE("EventBus matcher identifies specific EventPacket mismatches",
          "[unit][event_bus_matchers]") {
  steamrot::EventBus bus1;
  bus1.push_back(steamrot::EventPacket(
      steamrot::EventType::EventType_EVENT_QUIT_GAME,
      steamrot::EventData{std::monostate{}}, 1));
  bus1.push_back(steamrot::EventPacket(
      steamrot::EventType::EventType_EVENT_USER_INPUT,
      steamrot::EventData{steamrot::UserInputBitset{}}, 2));
  bus1.push_back(steamrot::EventPacket(
      steamrot::EventType::EventType_EVENT_CHANGE_SCENE,
      steamrot::EventData{std::monostate{}}, 3));

  steamrot::EventBus bus2;
  bus2.push_back(steamrot::EventPacket(
      steamrot::EventType::EventType_EVENT_QUIT_GAME,
      steamrot::EventData{std::monostate{}}, 1));
  bus2.push_back(steamrot::EventPacket(
      steamrot::EventType::EventType_EVENT_USER_INPUT,
      steamrot::EventData{steamrot::UserInputBitset{}}, 5)); // Different lifetime
  bus2.push_back(steamrot::EventPacket(
      steamrot::EventType::EventType_EVENT_CHANGE_SCENE,
      steamrot::EventData{std::monostate{}}, 3));

  steamrot::tests::EventBusEqualsMatcher matcher(bus2);
  REQUIRE_FALSE(matcher.match(bus1));

  // Check that description identifies the specific EventPacket at index 1
  std::string description = matcher.describe();
  REQUIRE(description.find("index 1") != std::string::npos);
  REQUIRE(description.find("event_lifetime") != std::string::npos);
}
