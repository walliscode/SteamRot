/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EventPayloadEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPayloadEqualsMatcher.h"
#include "EventPayload.h"
#include "catch2/catch_test_macros.hpp"
#include "conmat.h"
#include "uuid.h"

static uuids::uuid GenerateTestUUID() {
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 generator(seq);
  uuids::uuid_random_generator gen{generator};
  return gen();
}

TEST_CASE("EventPayloadEqualsMatcher works correctly for InputPayload",
          "[unit][Events][EventPayload][matcher]") {
  steamrot::InputPayload expected{steamrot::InputPayload::InputAction::SELECT};
  steamrot::InputPayload actual{steamrot::InputPayload::InputAction::SELECT};

  SECTION("Matcher detects equality for InputPayload") {
    steamrot::EventPayload expected_payload = expected;
    steamrot::EventPayload actual_payload = actual;
    auto matcher = steamrot::tests::EventPayloadEqualsMatcher(expected_payload);
    REQUIRE(matcher.match(actual_payload));
  }
}

TEST_CASE("EventPayloadEqualsMatcher works correctly for UIPayload",
          "[unit][Events][EventPayload][matcher]") {
  SECTION("Matcher detects equality for UIPayload with all fields") {
    steamrot::UIPayload expected{};
    expected.c_user_interface_name = "MainMenu";
    expected.c_ui_state_name = "Active";
    expected.action = steamrot::UIPayload::UIAction::TOGGLE;

    steamrot::UIPayload actual{};
    actual.c_user_interface_name = "MainMenu";
    actual.c_ui_state_name = "Active";
    actual.action = steamrot::UIPayload::UIAction::TOGGLE;

    steamrot::EventPayload expected_payload = expected;
    steamrot::EventPayload actual_payload = actual;
    auto matcher = steamrot::tests::EventPayloadEqualsMatcher(expected_payload);
    REQUIRE(matcher.match(actual_payload));
  }

  SECTION("Matcher detects inequality for different c_user_interface_name") {
    steamrot::UIPayload expected{};
    expected.c_user_interface_name = "MainMenu";
    expected.action = steamrot::UIPayload::UIAction::TOGGLE;

    steamrot::UIPayload actual{};
    actual.c_user_interface_name = "Settings";
    actual.action = steamrot::UIPayload::UIAction::TOGGLE;

    steamrot::EventPayload expected_payload = expected;
    steamrot::EventPayload actual_payload = actual;
    auto matcher = steamrot::tests::EventPayloadEqualsMatcher(expected_payload);
    REQUIRE(!matcher.match(actual_payload));
  }

  SECTION("Matcher detects inequality for different c_ui_state_name") {
    steamrot::UIPayload expected{};
    expected.c_ui_state_name = "Active";
    expected.action = steamrot::UIPayload::UIAction::TOGGLE;

    steamrot::UIPayload actual{};
    actual.c_ui_state_name = "Inactive";
    actual.action = steamrot::UIPayload::UIAction::TOGGLE;

    steamrot::EventPayload expected_payload = expected;
    steamrot::EventPayload actual_payload = actual;
    auto matcher = steamrot::tests::EventPayloadEqualsMatcher(expected_payload);
    REQUIRE(!matcher.match(actual_payload));
  }
}

TEST_CASE("EventPayloadEqualsMatcher works correctly for LogicPayload",
          "[unit][Events][EventPayload][matcher]") {
  SECTION("Matcher detects equality for LogicPayload") {
    steamrot::LogicPayload expected{
        steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD};
    steamrot::LogicPayload actual{
        steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD};

    steamrot::EventPayload expected_payload = expected;
    steamrot::EventPayload actual_payload = actual;
    auto matcher = steamrot::tests::EventPayloadEqualsMatcher(expected_payload);
    REQUIRE(matcher.match(actual_payload));
  }

  SECTION("Matcher detects inequality for different toggle_name") {
    steamrot::LogicPayload expected{
        steamrot::LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD};
    steamrot::LogicPayload actual{
        steamrot::LogicPayload::LogicToggle::CLEAR_MACHINA_FORM_SCAFFOLD};

    steamrot::EventPayload expected_payload = expected;
    steamrot::EventPayload actual_payload = actual;
    auto matcher = steamrot::tests::EventPayloadEqualsMatcher(expected_payload);
    REQUIRE(!matcher.match(actual_payload));
  }
}

TEST_CASE("EventPayloadEqualsMatcher works correctly for ScenePayload",
          "[unit][Events][EventPayload][matcher]") {
  SECTION("Matcher detects equality for ScenePayload with all fields") {
    steamrot::ScenePayload expected{};
    expected.optional_scene_type = steamrot::SceneType::TITLE;
    expected.optional_scene_id = GenerateTestUUID();
    expected.action = steamrot::ScenePayload::SceneAction::CHANGE;

    steamrot::ScenePayload actual{};
    actual.optional_scene_type = expected.optional_scene_type;
    actual.optional_scene_id = expected.optional_scene_id;
    actual.action = expected.action;

    steamrot::EventPayload expected_payload = expected;
    steamrot::EventPayload actual_payload = actual;
    auto matcher = steamrot::tests::EventPayloadEqualsMatcher(expected_payload);
    REQUIRE(matcher.match(actual_payload));
  }

  SECTION("Matcher detects inequality for different scene_type") {
    steamrot::ScenePayload expected{};
    expected.optional_scene_type = steamrot::SceneType::TITLE;
    expected.action = steamrot::ScenePayload::SceneAction::CHANGE;

    steamrot::ScenePayload actual{};
    actual.optional_scene_type = steamrot::SceneType::CRAFTING;
    actual.action = steamrot::ScenePayload::SceneAction::CHANGE;

    steamrot::EventPayload expected_payload = expected;
    steamrot::EventPayload actual_payload = actual;
    auto matcher = steamrot::tests::EventPayloadEqualsMatcher(expected_payload);
    REQUIRE(!matcher.match(actual_payload));
  }

  SECTION("Matcher detects inequality for different scene_id") {
    steamrot::ScenePayload expected{};
    expected.optional_scene_id = GenerateTestUUID();
    expected.action = steamrot::ScenePayload::SceneAction::CHANGE;

    steamrot::ScenePayload actual{};
    actual.optional_scene_id = GenerateTestUUID();
    actual.action = steamrot::ScenePayload::SceneAction::CHANGE;

    steamrot::EventPayload expected_payload = expected;
    steamrot::EventPayload actual_payload = actual;
    auto matcher = steamrot::tests::EventPayloadEqualsMatcher(expected_payload);
    REQUIRE(!matcher.match(actual_payload));
  }
}

TEST_CASE("EventPayloadEqualsMatcher works correctly for SystemPayload",
          "[unit][Events][EventPayload][matcher]") {
  SECTION("Matcher detects equality for SystemPayload") {
    steamrot::SystemPayload expected{
        steamrot::SystemPayload::SystemAction::QUIT};
    steamrot::SystemPayload actual{steamrot::SystemPayload::SystemAction::QUIT};

    steamrot::EventPayload expected_payload = expected;
    steamrot::EventPayload actual_payload = actual;
    auto matcher = steamrot::tests::EventPayloadEqualsMatcher(expected_payload);
    REQUIRE(matcher.match(actual_payload));
  }
}

TEST_CASE("EventPayloadEqualsMatcher detects different variant types",
          "[unit][Events][EventPayload][matcher]") {
  steamrot::InputPayload input{steamrot::InputPayload::InputAction::SELECT};
  steamrot::SystemPayload system{steamrot::SystemPayload::SystemAction::QUIT};

  steamrot::EventPayload expected_payload = input;
  steamrot::EventPayload actual_payload = system;

  auto matcher = steamrot::tests::EventPayloadEqualsMatcher(expected_payload);
  REQUIRE(!matcher.match(actual_payload));
}

TEST_CASE("EventPayloadEqualsMatcher describes mismatches correctly",
          "[unit][Events][EventPayload][matcher]") {
  steamrot::InputPayload input{steamrot::InputPayload::InputAction::SELECT};
  steamrot::SystemPayload system{steamrot::SystemPayload::SystemAction::QUIT};

  steamrot::EventPayload expected_payload = input;
  steamrot::EventPayload actual_payload = system;

  auto matcher = steamrot::tests::EventPayloadEqualsMatcher(expected_payload);
  matcher.match(actual_payload);

  std::ostringstream oss;
  oss << conmat::Header(conmat::TestFailed() + "EventPayload Mismatch:", 3)
      << "\n";
  oss << conmat::Indent(1) << conmat::TestFailed()
      << "EventPayload variant type differs:" << "\n";
  oss << conmat::Indent(2)
      << "actual: " << conmat::Colorize("SystemPayload", conmat::Color::Red)
      << "\n";
  oss << conmat::Indent(2)
      << "expected: " << conmat::Colorize("InputPayload", conmat::Color::Blue)
      << "\n";
  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("EventPayloadEqualsMatcher describes matches correctly",
          "[unit][Events][EventPayload][matcher]") {
  steamrot::InputPayload input{steamrot::InputPayload::InputAction::SELECT};
  steamrot::EventPayload expected_payload = input;
  steamrot::EventPayload actual_payload = input;

  auto matcher = steamrot::tests::EventPayloadEqualsMatcher(expected_payload);
  matcher.match(actual_payload);

  std::ostringstream oss;
  oss << conmat::Header(conmat::TestPassed() + "EventPayload Match:", 3)
      << "\n";
  REQUIRE(matcher.describe() == oss.str());
}
