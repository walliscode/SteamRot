/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EventDataEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventDataEqualsMatcher.h"
#include "EventPacket.h"
#include "catch2/catch_test_macros.hpp"
#include "conmat.h"
#include "uuid.h"
#include <SFML/Window/Keyboard.hpp>

static uuids::uuid GenerateTestUUID() {
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 generator(seq);
  uuids::uuid_random_generator gen{generator};
  return gen();
}
TEST_CASE("EventDataEqualsMatcher works correctly",
          "[unit][Events][EventData][matcher]") {
  steamrot::EventData expected = std::monostate{};
  steamrot::EventData actual = std::monostate{};
  SECTION("Matcher detects equality for std::monostate") {
    auto matcher = steamrot::tests::EventDataEqualsMatcher(expected);
    REQUIRE(matcher.match(actual));
  }
  SECTION("Matcher detects inequality for different variant types") {
    expected = steamrot::UserInputBitset{};
    actual = std::monostate{};
    auto matcher = steamrot::tests::EventDataEqualsMatcher(expected);
    REQUIRE(!matcher.match(actual));
  }
  SECTION("Matcher detects equality for UserInputBitset") {
    expected = steamrot::UserInputBitset{};
    actual = steamrot::UserInputBitset{};
    auto matcher = steamrot::tests::EventDataEqualsMatcher(expected);
    REQUIRE(matcher.match(actual));
  }
}

TEST_CASE("EventDataEqualsMatcher describes mismatches correctly",
          "[unit][Events][EventData][matcher]") {
  steamrot::EventData expected = steamrot::UserInputBitset{};
  steamrot::EventData actual = std::monostate{};
  auto matcher = steamrot::tests::EventDataEqualsMatcher(expected);
  matcher.match(actual);

  std::ostringstream oss;
  oss << conmat::Header(conmat::TestFailed() + "EventData Mismatch:", 3)
      << "\n";
  oss << conmat::Indent(1) << conmat::TestFailed()
      << "EventData variant type differs:" << "\n";
  oss << conmat::Indent(2)
      << "actual: " << conmat::Colorize("std::monostate", conmat::Color::Red)
      << "\n";
  oss << conmat::Indent(2) << "expected: "
      << conmat::Colorize("UserInputBitset", conmat::Color::Blue) << "\n";
  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("EventDataEqualsMatcher describes matches correctly",
          "[unit][Events][EventData][matcher]") {
  steamrot::EventData expected = steamrot::UserInputBitset{};
  steamrot::EventData actual = steamrot::UserInputBitset{};
  auto matcher = steamrot::tests::EventDataEqualsMatcher(expected);
  matcher.match(actual);
  std::ostringstream oss;
  oss << conmat::Header(conmat::TestPassed() + "EventData Match:", 3) << "\n";
  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("EventDataEqualsMatcher detects UserInputBitset differences",
          "[unit][Events][EventData][matcher]") {
  steamrot::UserInputBitset expected_bitset;
  expected_bitset.setKeyPressed(sf::Keyboard::Key::A);

  steamrot::UserInputBitset actual_bitset;
  actual_bitset.setKeyPressed(sf::Keyboard::Key::B);

  steamrot::EventData expected = expected_bitset;
  steamrot::EventData actual = actual_bitset;
  auto matcher = steamrot::tests::EventDataEqualsMatcher(expected);
  REQUIRE(!matcher.match(actual));
  std::ostringstream oss;
  oss << conmat::Header(conmat::TestFailed() + "EventData Mismatch:", 3)
      << "\n";
  oss << conmat::Indent(1) << conmat::TestFailed()
      << "m_event_data UserInputBitset differs:"
      << "\n";
  oss << conmat::Indent(2)
      << "actual: " << conmat::Colorize(actual_bitset, conmat::Color::Red)
      << "\n";
  oss << conmat::Indent(2)
      << "expected: " << conmat::Colorize(expected_bitset, conmat::Color::Blue)
      << "\n";

  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("EventDataEqualsMatcher detects SceneChangePacket differences",
          "[unit][Events][EventData][matcher]") {

  steamrot::EventData expected_packet{steamrot::SceneChangePacket{
      GenerateTestUUID(), steamrot::SceneType::SceneType_TITLE}};

  steamrot::EventData actual_packet{steamrot::SceneChangePacket{
      GenerateTestUUID(), steamrot::SceneType::SceneType_CRAFTING}};

  auto matcher = steamrot::tests::EventDataEqualsMatcher(expected_packet);
  REQUIRE(!matcher.match(actual_packet));
  std::ostringstream oss;
  oss << conmat::Header(conmat::TestFailed() + "EventData Mismatch:", 3)
      << "\n";
  oss << conmat::Indent(1) << conmat::TestFailed()
      << "m_event_data SceneChangePacket UUID differs:" << "\n";
  oss << conmat::Indent(2) << "actual: "
      << conmat::Colorize(
             std::get<steamrot::SceneChangePacket>(actual_packet).first.value(),
             conmat::Color::Red)
      << "\n";
  oss << conmat::Indent(2) << "expected: "
      << conmat::Colorize(std::get<steamrot::SceneChangePacket>(expected_packet)
                              .first.value(),
                          conmat::Color::Blue)
      << "\n";
  oss << conmat::Indent(1) << conmat::TestFailed()
      << "m_event_data SceneChangePacket SceneType differs:" << "\n";
  oss << conmat::Indent(2) << "actual: "
      << conmat::Colorize(
             steamrot::EnumNameSceneType(
                 std::get<steamrot::SceneChangePacket>(actual_packet).second),
             conmat::Color::Red)
      << "\n";
  oss << conmat::Indent(2) << "expected: "
      << conmat::Colorize(
             steamrot::EnumNameSceneType(
                 std::get<steamrot::SceneChangePacket>(expected_packet).second),
             conmat::Color::Blue)
      << "\n";

  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("EventDataEqualsMatcher detects UserInterfaceName differences",
          "[unit][Events][EventData][matcher]") {
  steamrot::EventData expected_name = steamrot::UserInterfaceName{"MainMenu"};
  steamrot::EventData actual_name = steamrot::UserInterfaceName{"Settings"};
  auto matcher = steamrot::tests::EventDataEqualsMatcher(expected_name);
  REQUIRE(!matcher.match(actual_name));
  std::ostringstream oss;
  oss << conmat::Header(conmat::TestFailed() + "EventData Mismatch:", 3)
      << "\n";
  oss << conmat::Indent(1) << conmat::TestFailed()
      << "m_event_data UserInterfaceName differs:"
      << "\n";
  oss << conmat::Indent(2) << "actual: "
      << conmat::Colorize(std::get<steamrot::UserInterfaceName>(actual_name),
                          conmat::Color::Red)
      << "\n";
  oss << conmat::Indent(2) << "expected: "
      << conmat::Colorize(std::get<steamrot::UserInterfaceName>(expected_name),
                          conmat::Color::Blue)
      << "\n";
  REQUIRE(matcher.describe() == oss.str());
}
