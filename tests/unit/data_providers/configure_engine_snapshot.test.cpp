/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for configure_engine_snapshot functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_engine_snapshot.h"
#include "EngineSnapshot.h"
#include "EventHandler.h"
#include "engine_snapshot_generated.h"
#include "events_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <flatbuffers/flatbuffers.h>
#include <iostream>

/////////////////////////////////////////////////
// ConfigureEngineSnapshot tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureEngineSnapshot fails with null data",
          "[unit][configure_engine_snapshot]") {
  steamrot::EngineSnapshot snapshot;
  steamrot::EventHandler event_handler;

  auto result = steamrot::data::configure::ConfigureEngineSnapshot(
      snapshot, nullptr, event_handler);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "EngineSnapshotFbs is null");
}

TEST_CASE("ConfigureEngineSnapshot handles empty snapshot",
          "[unit][configure_engine_snapshot]") {
  steamrot::EngineSnapshot snapshot;
  steamrot::EventHandler event_handler;

  // Create empty FlatBuffers data
  flatbuffers::FlatBufferBuilder builder;
  auto snapshot_offset = steamrot::CreateEngineSnapshotFbs(builder);
  builder.Finish(snapshot_offset);
  const steamrot::EngineSnapshotFbs *snapshot_fbs =
      flatbuffers::GetRoot<steamrot::EngineSnapshotFbs>(
          builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureEngineSnapshot(
      snapshot, snapshot_fbs, event_handler);

  REQUIRE(result.has_value());
  // All fields should be empty/default
  REQUIRE_FALSE(snapshot.tick_number.has_value());
  REQUIRE_FALSE(snapshot.global_event_bus.has_value());
  REQUIRE_FALSE(snapshot.scene_manager_data.has_value());
}

TEST_CASE("ConfigureEngineSnapshot configures tick_number",
          "[unit][configure_engine_snapshot]") {
  steamrot::EngineSnapshot snapshot;
  steamrot::EventHandler event_handler;

  // Create FlatBuffers data with tick_number
  flatbuffers::FlatBufferBuilder builder;
  auto snapshot_offset =
      steamrot::CreateEngineSnapshotFbs(builder, 42); // tick_number = 42
  builder.Finish(snapshot_offset);
  const steamrot::EngineSnapshotFbs *snapshot_fbs =
      flatbuffers::GetRoot<steamrot::EngineSnapshotFbs>(
          builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureEngineSnapshot(
      snapshot, snapshot_fbs, event_handler);

  REQUIRE(result.has_value());
  REQUIRE(snapshot.tick_number.has_value());
  REQUIRE(snapshot.tick_number.value() == 42);
}

TEST_CASE("ConfigureEngineSnapshot configures global_event_bus with events",
          "[unit][configure_engine_snapshot]") {
  steamrot::EngineSnapshot snapshot;
  steamrot::EventHandler event_handler;

  // Create FlatBuffers data with event bus
  flatbuffers::FlatBufferBuilder builder;

  // Create the UI name data for the event
  auto ui_name_str = builder.CreateString("test_ui");
  auto ui_name_data = steamrot::CreateUserInterfaceNameData(builder, ui_name_str);

  // Create an event packet with proper union data
  auto event_packet_offset = steamrot::CreateEventPacketData(
      builder, 1, steamrot::EventTypeFbs_EVENT_TOGGLE_UI,
      steamrot::EventDataData_UserInterfaceNameData, ui_name_data.Union());

  std::vector<flatbuffers::Offset<steamrot::EventPacketData>> events_vector;
  events_vector.push_back(event_packet_offset);
  auto events_offset = builder.CreateVector(events_vector);

  auto event_bus_offset = steamrot::CreateEventBusData(builder, events_offset);

  auto snapshot_offset = steamrot::CreateEngineSnapshotFbs(
      builder, 0, event_bus_offset); // tick_number = 0 (not set)
  builder.Finish(snapshot_offset);
  const steamrot::EngineSnapshotFbs *snapshot_fbs =
      flatbuffers::GetRoot<steamrot::EngineSnapshotFbs>(
          builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureEngineSnapshot(
      snapshot, snapshot_fbs, event_handler);

  REQUIRE(result.has_value());
  REQUIRE(snapshot.global_event_bus.has_value());
  REQUIRE(snapshot.global_event_bus.value().size() == 1);
  REQUIRE(snapshot.global_event_bus.value()[0].event_type ==
          steamrot::EventTypeFbs_EVENT_TOGGLE_UI);
  REQUIRE(snapshot.global_event_bus.value()[0].event_lifetime == 1);
}

TEST_CASE("ConfigureEngineSnapshot configures empty event bus",
          "[unit][configure_engine_snapshot]") {
  steamrot::EngineSnapshot snapshot;
  steamrot::EventHandler event_handler;

  // Create FlatBuffers data with empty event bus
  flatbuffers::FlatBufferBuilder builder;

  std::vector<flatbuffers::Offset<steamrot::EventPacketData>> events_vector;
  auto events_offset = builder.CreateVector(events_vector);
  auto event_bus_offset = steamrot::CreateEventBusData(builder, events_offset);

  auto snapshot_offset = steamrot::CreateEngineSnapshotFbs(
      builder, 0, event_bus_offset); // tick_number = 0 (not set)
  builder.Finish(snapshot_offset);
  const steamrot::EngineSnapshotFbs *snapshot_fbs =
      flatbuffers::GetRoot<steamrot::EngineSnapshotFbs>(
          builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureEngineSnapshot(
      snapshot, snapshot_fbs, event_handler);

  REQUIRE(result.has_value());
  REQUIRE(snapshot.global_event_bus.has_value());
  REQUIRE(snapshot.global_event_bus.value().size() == 0);
}

TEST_CASE("ConfigureEngineSnapshot configures scene_manager_data",
          "[unit][configure_engine_snapshot]") {
  steamrot::EngineSnapshot snapshot;
  steamrot::EventHandler event_handler;

  // Create FlatBuffers data with scene_manager_data
  flatbuffers::FlatBufferBuilder builder;

  // Create minimal SceneManagerDataFbs with empty subscriptions
  std::vector<flatbuffers::Offset<steamrot::SubscriberFbs>> subs_vector;
  auto subs_offset = builder.CreateVector(subs_vector);
  auto state_offset =
      steamrot::CreateSceneManagerStateFbs(builder, subs_offset);
  auto scene_manager_data_offset =
      steamrot::CreateSceneManagerDataFbs(builder, state_offset);

  auto snapshot_offset = steamrot::CreateEngineSnapshotFbs(
      builder, 0, 0, scene_manager_data_offset);
  builder.Finish(snapshot_offset);
  const steamrot::EngineSnapshotFbs *snapshot_fbs =
      flatbuffers::GetRoot<steamrot::EngineSnapshotFbs>(
          builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureEngineSnapshot(
      snapshot, snapshot_fbs, event_handler);

  REQUIRE(result.has_value());
  REQUIRE(snapshot.scene_manager_data.has_value());
  REQUIRE(snapshot.scene_manager_data.value()
              .scene_manager_state.subscriptions.size() == 0);
}

TEST_CASE("ConfigureEngineSnapshot configures empty scene_collection_data",
          "[unit][configure_engine_snapshot]") {
  steamrot::EngineSnapshot snapshot;
  steamrot::EventHandler event_handler;

  // Create FlatBuffers data with empty scene_collection_data
  flatbuffers::FlatBufferBuilder builder;

  // Create minimal SceneCollectionDataFbs (empty)
  std::vector<flatbuffers::Offset<steamrot::SceneDataFbs>> scenes_vector;
  auto scenes_offset = builder.CreateVector(scenes_vector);
  auto scene_collection_offset =
      steamrot::CreateSceneCollectionDataFbs(builder, scenes_offset);

  auto snapshot_offset = steamrot::CreateEngineSnapshotFbs(
      builder, 0, 0, 0, scene_collection_offset);
  builder.Finish(snapshot_offset);
  const steamrot::EngineSnapshotFbs *snapshot_fbs =
      flatbuffers::GetRoot<steamrot::EngineSnapshotFbs>(
          builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureEngineSnapshot(
      snapshot, snapshot_fbs, event_handler);

  REQUIRE(result.has_value());
  REQUIRE(snapshot.scene_collection_data.size() == 0);
}

TEST_CASE("ConfigureEngineSnapshot handles malformed EventPacketData with null "
          "union data",
          "[unit][configure_engine_snapshot]") {
  steamrot::EngineSnapshot snapshot;
  steamrot::EventHandler event_handler;

  // Create FlatBuffers data with event bus containing malformed EventPacketData
  flatbuffers::FlatBufferBuilder builder;

  // Create an event packet with union type but NO union data (null pointer)
  // This simulates the bug that caused the original SIGSEGV
  auto event_packet_offset = steamrot::CreateEventPacketData(
      builder, 1, steamrot::EventTypeFbs_EVENT_TOGGLE_UI,
      steamrot::EventDataData_UserInterfaceNameData,
      0); // Passing 0 (null offset) for union data

  std::vector<flatbuffers::Offset<steamrot::EventPacketData>> events_vector;
  events_vector.push_back(event_packet_offset);
  auto events_offset = builder.CreateVector(events_vector);

  auto event_bus_offset = steamrot::CreateEventBusData(builder, events_offset);

  auto snapshot_offset =
      steamrot::CreateEngineSnapshotFbs(builder, 0, event_bus_offset);
  builder.Finish(snapshot_offset);
  const steamrot::EngineSnapshotFbs *snapshot_fbs =
      flatbuffers::GetRoot<steamrot::EngineSnapshotFbs>(
          builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureEngineSnapshot(
      snapshot, snapshot_fbs, event_handler);

  // Should fail gracefully with an error instead of segfaulting
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
  REQUIRE(result.error().message ==
          "CreateEventData: UserInterfaceNameData pointer is null");
}
