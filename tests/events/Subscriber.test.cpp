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

TEST_CASE("Subscriber: trigger event functionality", "[Subscriber]") {
  // Create trigger event data
  steamrot::EventData trigger_data = std::string("test_ui_element");
  
  // Create subscriber with trigger event
  steamrot::Subscriber subscriber_with_trigger{
    steamrot::EventType::EventType_EVENT_TOGGLE_DROPDOWN, 
    trigger_data
  };
  
  // Check trigger event is set correctly
  auto trigger_event = subscriber_with_trigger.GetTriggerEvent();
  REQUIRE(trigger_event.has_value());
  REQUIRE(trigger_event.value() == trigger_data);
  
  // Test ShouldActivate with matching data
  REQUIRE(subscriber_with_trigger.ShouldActivate(trigger_data));
  
  // Test ShouldActivate with non-matching data
  steamrot::EventData different_data = std::string("different_element");
  REQUIRE(!subscriber_with_trigger.ShouldActivate(different_data));
  
  // Test subscriber without trigger event (should always activate)
  steamrot::Subscriber subscriber_no_trigger{steamrot::EventType::EventType_EVENT_TEST};
  REQUIRE(!subscriber_no_trigger.GetTriggerEvent().has_value());
  REQUIRE(subscriber_no_trigger.ShouldActivate(trigger_data));
  REQUIRE(subscriber_no_trigger.ShouldActivate(different_data));
}
