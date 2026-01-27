/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for event type conversion functions
/////////////////////////////////////////////////

#include "event_type_conversion.h"
#include "EventType.h"
#include "events_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConvertEventTypeFbsToEventType converts NONE",
          "[unit][event_type_conversion]") {
  auto result = steamrot::event::ConvertEventTypeFbsToEventType(
      steamrot::EventTypeFbs_EVENT_NONE);
  
  REQUIRE(result.has_value());
  REQUIRE(result.value() == steamrot::EventType::NONE);
}

TEST_CASE("ConvertEventTypeFbsToEventType converts TEST",
          "[unit][event_type_conversion]") {
  auto result = steamrot::event::ConvertEventTypeFbsToEventType(
      steamrot::EventTypeFbs_EVENT_TEST);
  
  REQUIRE(result.has_value());
  REQUIRE(result.value() == steamrot::EventType::TEST);
}

TEST_CASE("ConvertEventTypeFbsToEventType converts USER_INPUT",
          "[unit][event_type_conversion]") {
  auto result = steamrot::event::ConvertEventTypeFbsToEventType(
      steamrot::EventTypeFbs_EVENT_USER_INPUT);
  
  REQUIRE(result.has_value());
  REQUIRE(result.value() == steamrot::EventType::USER_INPUT);
}

TEST_CASE("ConvertEventTypeFbsToEventType converts TOGGLE_UI",
          "[unit][event_type_conversion]") {
  auto result = steamrot::event::ConvertEventTypeFbsToEventType(
      steamrot::EventTypeFbs_EVENT_TOGGLE_UI);
  
  REQUIRE(result.has_value());
  REQUIRE(result.value() == steamrot::EventType::TOGGLE_UI);
}

TEST_CASE("ConvertEventTypeFbsToEventType converts CHANGE_SCENE",
          "[unit][event_type_conversion]") {
  auto result = steamrot::event::ConvertEventTypeFbsToEventType(
      steamrot::EventTypeFbs_EVENT_CHANGE_SCENE);
  
  REQUIRE(result.has_value());
  REQUIRE(result.value() == steamrot::EventType::CHANGE_SCENE);
}

TEST_CASE("ConvertEventTypeFbsToEventType converts QUIT_GAME",
          "[unit][event_type_conversion]") {
  auto result = steamrot::event::ConvertEventTypeFbsToEventType(
      steamrot::EventTypeFbs_EVENT_QUIT_GAME);
  
  REQUIRE(result.has_value());
  REQUIRE(result.value() == steamrot::EventType::QUIT_GAME);
}

TEST_CASE("ConvertEventTypeFbsToEventType converts TOGGLE_DROPDOWN",
          "[unit][event_type_conversion]") {
  auto result = steamrot::event::ConvertEventTypeFbsToEventType(
      steamrot::EventTypeFbs_EVENT_TOGGLE_DROPDOWN);
  
  REQUIRE(result.has_value());
  REQUIRE(result.value() == steamrot::EventType::TOGGLE_DROPDOWN);
}

TEST_CASE("ConvertEventTypeFbsToEventType handles invalid enum value",
          "[unit][event_type_conversion]") {
  // Cast an invalid value to EventTypeFbs
  auto invalid_value = static_cast<steamrot::EventTypeFbs>(9999);
  
  auto result = steamrot::event::ConvertEventTypeFbsToEventType(invalid_value);
  
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::EnumValueNotHandled);
}

TEST_CASE("ConvertEventTypeFbsToEventType all enum values have mappings",
          "[unit][event_type_conversion]") {
  // Test all valid EventTypeFbs values to ensure complete coverage
  std::vector<steamrot::EventTypeFbs> all_event_types = {
      steamrot::EventTypeFbs_EVENT_NONE,
      steamrot::EventTypeFbs_EVENT_TEST,
      steamrot::EventTypeFbs_EVENT_USER_INPUT,
      steamrot::EventTypeFbs_EVENT_TOGGLE_UI,
      steamrot::EventTypeFbs_EVENT_CHANGE_SCENE,
      steamrot::EventTypeFbs_EVENT_QUIT_GAME,
      steamrot::EventTypeFbs_EVENT_TOGGLE_DROPDOWN
  };
  
  for (const auto& event_type_fbs : all_event_types) {
    auto result = steamrot::event::ConvertEventTypeFbsToEventType(event_type_fbs);
    REQUIRE(result.has_value());
  }
}
