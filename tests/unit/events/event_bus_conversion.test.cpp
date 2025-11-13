/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EventBus conversion utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "event_bus_conversion.h"
#include "EventHandler.h"
#include "EventPacket.h"
#include "event_matchers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConvertEventBusDataToEventBus handles null input", "[unit][event_bus_conversion]") {
  auto result = steamrot::event::conversion::ConvertEventBusDataToEventBus(nullptr);
  
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().fail_mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("ConvertEventBusDataToEventBus handles empty EventBusData", "[unit][event_bus_conversion]") {
  // Create empty EventBusData manually (this would normally come from FlatBuffers)
  // Since we can't easily create FlatBuffers data in tests, we'll use a builder
  flatbuffers::FlatBufferBuilder builder;
  
  auto event_bus_data_offset = steamrot::CreateEventBusData(builder);
  builder.Finish(event_bus_data_offset);
  
  const steamrot::EventBusData *event_bus_data = 
      steamrot::GetEventBusData(builder.GetBufferPointer());
  
  auto result = steamrot::event::conversion::ConvertEventBusDataToEventBus(event_bus_data);
  
  REQUIRE(result.has_value());
  REQUIRE(result.value().empty());
}

TEST_CASE("ConvertEventBusDataToEventBus converts single event", "[unit][event_bus_conversion]") {
  flatbuffers::FlatBufferBuilder builder;
  
  // Create a simple EventPacketData
  auto event_packet_offset = steamrot::CreateEventPacketData(
      builder,
      5,  // event_lifetime
      steamrot::EventType::EventType_EVENT_TEST,
      steamrot::EventDataData_NONE
  );
  
  std::vector<flatbuffers::Offset<steamrot::EventPacketData>> events;
  events.push_back(event_packet_offset);
  
  auto event_bus_data_offset = steamrot::CreateEventBusData(
      builder,
      builder.CreateVector(events)
  );
  builder.Finish(event_bus_data_offset);
  
  const steamrot::EventBusData *event_bus_data = 
      steamrot::GetEventBusData(builder.GetBufferPointer());
  
  auto result = steamrot::event::conversion::ConvertEventBusDataToEventBus(event_bus_data);
  
  REQUIRE(result.has_value());
  REQUIRE(result.value().size() == 1);
  REQUIRE(result.value()[0].m_event_type == steamrot::EventType::EventType_EVENT_TEST);
  REQUIRE(result.value()[0].event_lifetime == 5);
}

TEST_CASE("ConvertEventBusDataToEventBus converts multiple events", "[unit][event_bus_conversion]") {
  flatbuffers::FlatBufferBuilder builder;
  
  // Create multiple EventPacketData
  auto event1_offset = steamrot::CreateEventPacketData(
      builder,
      3,  // event_lifetime
      steamrot::EventType::EventType_EVENT_TEST,
      steamrot::EventDataData_NONE
  );
  
  auto event2_offset = steamrot::CreateEventPacketData(
      builder,
      1,  // event_lifetime
      steamrot::EventType::EventType_EVENT_QUIT_GAME,
      steamrot::EventDataData_NONE
  );
  
  std::vector<flatbuffers::Offset<steamrot::EventPacketData>> events;
  events.push_back(event1_offset);
  events.push_back(event2_offset);
  
  auto event_bus_data_offset = steamrot::CreateEventBusData(
      builder,
      builder.CreateVector(events)
  );
  builder.Finish(event_bus_data_offset);
  
  const steamrot::EventBusData *event_bus_data = 
      steamrot::GetEventBusData(builder.GetBufferPointer());
  
  auto result = steamrot::event::conversion::ConvertEventBusDataToEventBus(event_bus_data);
  
  REQUIRE(result.has_value());
  REQUIRE(result.value().size() == 2);
  REQUIRE(result.value()[0].m_event_type == steamrot::EventType::EventType_EVENT_TEST);
  REQUIRE(result.value()[0].event_lifetime == 3);
  REQUIRE(result.value()[1].m_event_type == steamrot::EventType::EventType_EVENT_QUIT_GAME);
  REQUIRE(result.value()[1].event_lifetime == 1);
}

TEST_CASE("ConfigureEventHandlerFromEventBusData populates global event bus", "[unit][event_bus_conversion]") {
  flatbuffers::FlatBufferBuilder builder;
  
  // Create multiple EventPacketData
  auto event1_offset = steamrot::CreateEventPacketData(
      builder,
      3,  // event_lifetime
      steamrot::EventType::EventType_EVENT_TEST,
      steamrot::EventDataData_NONE
  );
  
  auto event2_offset = steamrot::CreateEventPacketData(
      builder,
      2,  // event_lifetime
      steamrot::EventType::EventType_EVENT_QUIT_GAME,
      steamrot::EventDataData_NONE
  );
  
  std::vector<flatbuffers::Offset<steamrot::EventPacketData>> events;
  events.push_back(event1_offset);
  events.push_back(event2_offset);
  
  auto event_bus_data_offset = steamrot::CreateEventBusData(
      builder,
      builder.CreateVector(events)
  );
  builder.Finish(event_bus_data_offset);
  
  const steamrot::EventBusData *event_bus_data = 
      steamrot::GetEventBusData(builder.GetBufferPointer());
  
  // Create EventHandler
  steamrot::EventHandler event_handler;
  
  // Verify global event bus is initially empty
  REQUIRE(event_handler.GetGlobalEventBus().empty());
  
  // Configure from EventBusData
  auto result = steamrot::event::conversion::ConfigureEventHandlerFromEventBusData(
      event_bus_data, event_handler);
  
  REQUIRE(result.has_value());
  
  // Verify global event bus is populated
  const steamrot::EventBus &global_bus = event_handler.GetGlobalEventBus();
  REQUIRE(global_bus.size() == 2);
  REQUIRE(global_bus[0].m_event_type == steamrot::EventType::EventType_EVENT_TEST);
  REQUIRE(global_bus[0].event_lifetime == 3);
  REQUIRE(global_bus[1].m_event_type == steamrot::EventType::EventType_EVENT_QUIT_GAME);
  REQUIRE(global_bus[1].event_lifetime == 2);
}

TEST_CASE("ConfigureEventHandlerFromEventBusData handles null input", "[unit][event_bus_conversion]") {
  steamrot::EventHandler event_handler;
  
  auto result = steamrot::event::conversion::ConfigureEventHandlerFromEventBusData(
      nullptr, event_handler);
  
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().fail_mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("ConfigureEventHandlerFromEventBusData handles empty EventBusData", "[unit][event_bus_conversion]") {
  flatbuffers::FlatBufferBuilder builder;
  
  auto event_bus_data_offset = steamrot::CreateEventBusData(builder);
  builder.Finish(event_bus_data_offset);
  
  const steamrot::EventBusData *event_bus_data = 
      steamrot::GetEventBusData(builder.GetBufferPointer());
  
  steamrot::EventHandler event_handler;
  
  auto result = steamrot::event::conversion::ConfigureEventHandlerFromEventBusData(
      event_bus_data, event_handler);
  
  REQUIRE(result.has_value());
  REQUIRE(event_handler.GetGlobalEventBus().empty());
}
