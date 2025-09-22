/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the Subscriber class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Subscriber.h"
#include "events_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <variant>

TEST_CASE("Subscriber: check functions", "[Subscriber]") {

  // create a Subscriber object
  steamrot::Subscriber subscriber{steamrot::EventType::EventType_EVENT_TEST};

  // check the event type
  auto registration_info = subscriber.GetRegistrationInfo();
  REQUIRE(registration_info.first == steamrot::EventType::EventType_EVENT_TEST);
  REQUIRE(std::holds_alternative<std::monostate>(
      registration_info.second)); // check the event data is monostate

  // check the subscriber is not active
  REQUIRE(!subscriber.IsActive());
  // activate the subscriber
  auto result = subscriber.SetActive();
  REQUIRE(result.has_value());    // check the result is valid
  REQUIRE(subscriber.IsActive()); // check the subscriber is now active
  // deactivate the subscriber
  auto deactivate_result = subscriber.SetInactive();
  REQUIRE(deactivate_result.has_value()); // check the result is valid
  REQUIRE(!subscriber.IsActive()); // check the subscriber is now inactive
}
