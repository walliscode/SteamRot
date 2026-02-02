/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for response_event configuration in UI elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ButtonElement.h"
#include "EventHandler.h"
#include "EventPacket.h"
#include "EventType.h"
#include "configure_ui_elements.h"
#include "event_packet_data_generated.h"
#include "events_generated.h"
#include "user_interface_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <flatbuffers/flatbuffers.h>

TEST_CASE("ConfigureBaseUIElement sets response_event for data-less QUIT_GAME",
          "[unit][configure_ui_elements][response_event]") {
  
  // Create a FlatBuffers builder
  flatbuffers::FlatBufferBuilder builder;

  // Create EventPacketData with QUIT_GAME but NO event_data_data
  auto event_packet_data = steamrot::CreateEventPacketData(
      builder,
      1,  // event_lifetime
      steamrot::EventTypeFbs_EVENT_QUIT_GAME,
      steamrot::EventDataData_NONE,  // No data
      0);  // No event_data_data offset

  // Create minimal UIElementData with response_event_data
  auto position = steamrot::CreateVector2(builder, 0, 0);
  auto size = steamrot::CreateVector2(builder, 100, 50);
  
  auto ui_element_data = steamrot::CreateUIElementData(
      builder,
      position,
      size,
      false,  // is_mouse_over
      0,      // subscriber_data
      event_packet_data,  // response_event_data
      0,      // children
      steamrot::LayoutFbs_Vertical,
      steamrot::SpacingAndSizingFbs_None,
      false);  // children_active

  builder.Finish(ui_element_data);

  // Get pointer to the built data
  const steamrot::UIElementData *data =
      flatbuffers::GetRoot<steamrot::UIElementData>(builder.GetBufferPointer());

  // Create a ButtonElement to configure
  steamrot::ButtonElement button;
  steamrot::EventHandler event_handler;

  // Create the callback (not used in this test but required)
  auto callback = [](const steamrot::UIElementDataUnion &,
                     const void *) -> std::expected<std::unique_ptr<steamrot::UIElement>,
                                                   steamrot::FailInfo> {
    return std::unexpected(steamrot::FailInfo{steamrot::FailMode::NotImplemented,
                                              "Not needed for this test"});
  };

  // Configure the element
  auto result = steamrot::data::configure::ConfigureBaseUIElement(
      button, *data, event_handler, callback);

  // Verify configuration succeeded
  REQUIRE(result.has_value());

  // Verify response_event was set
  REQUIRE(button.response_event.has_value());

  // Verify the event type is QUIT_GAME
  REQUIRE(button.response_event.value().event_type ==
          steamrot::EventType::QUIT_GAME);

  // Verify the event data is std::monostate (data-less event)
  REQUIRE(std::holds_alternative<std::monostate>(
      button.response_event.value().event_data));

  // Verify the event lifetime
  REQUIRE(button.response_event.value().event_lifetime == 1);
}

TEST_CASE("ConfigureBaseUIElement skips response_event when event_type is NONE",
          "[unit][configure_ui_elements][response_event]") {
  
  // Create a FlatBuffers builder
  flatbuffers::FlatBufferBuilder builder;

  // Create EventPacketData with EVENT_NONE
  auto event_packet_data = steamrot::CreateEventPacketData(
      builder,
      1,  // event_lifetime
      steamrot::EventTypeFbs_EVENT_NONE,  // NONE type
      steamrot::EventDataData_NONE,
      0);

  // Create minimal UIElementData with response_event_data
  auto position = steamrot::CreateVector2(builder, 0, 0);
  auto size = steamrot::CreateVector2(builder, 100, 50);
  
  auto ui_element_data = steamrot::CreateUIElementData(
      builder,
      position,
      size,
      false,  // is_mouse_over
      0,      // subscriber_data
      event_packet_data,  // response_event_data
      0,      // children
      steamrot::LayoutFbs_Vertical,
      steamrot::SpacingAndSizingFbs_None,
      false);  // children_active

  builder.Finish(ui_element_data);

  // Get pointer to the built data
  const steamrot::UIElementData *data =
      flatbuffers::GetRoot<steamrot::UIElementData>(builder.GetBufferPointer());

  // Create a ButtonElement to configure
  steamrot::ButtonElement button;
  steamrot::EventHandler event_handler;

  // Create the callback (not used in this test but required)
  auto callback = [](const steamrot::UIElementDataUnion &,
                     const void *) -> std::expected<std::unique_ptr<steamrot::UIElement>,
                                                   steamrot::FailInfo> {
    return std::unexpected(steamrot::FailInfo{steamrot::FailMode::NotImplemented,
                                              "Not needed for this test"});
  };

  // Configure the element
  auto result = steamrot::data::configure::ConfigureBaseUIElement(
      button, *data, event_handler, callback);

  // Verify configuration succeeded
  REQUIRE(result.has_value());

  // Verify response_event was NOT set (should be nullopt)
  REQUIRE_FALSE(button.response_event.has_value());
}
