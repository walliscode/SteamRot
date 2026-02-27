/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersEngineSnapshotProvider
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEngineSnapshotProvider.h"
#include "EventHandler.h"
#include "EventPayload.h"
#include "load_engine_snapshot_data.h"
#include <catch2/catch_test_macros.hpp>
#include <variant>

/////////////////////////////////////////////////
// Constructor tests
/////////////////////////////////////////////////

TEST_CASE("FlatbuffersEngineSnapshotProvider constructs with EventHandler and "
          "FlatBuffers data",
          "[unit][FlatbuffersEngineSnapshotProvider]") {
  steamrot::EventHandler event_handler;

  // Load test data
  auto [data, engine_snapshot_fbs] =
      steamrot::tests::LoadEngineSnapshotTestData();

  steamrot::FlatbuffersEngineSnapshotProvider provider(event_handler,
                                                       engine_snapshot_fbs);
  SUCCEED();
}

/////////////////////////////////////////////////
// CreateEngineSnapshot tests
/////////////////////////////////////////////////

TEST_CASE("CreateEngineSnapshot fails when FlatBuffers data is null",
          "[unit][FlatbuffersEngineSnapshotProvider]") {
  steamrot::EventHandler event_handler;
  steamrot::FlatbuffersEngineSnapshotProvider provider(event_handler);

  auto result = provider.CreateEngineSnapshot();

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
  REQUIRE(result.error().message ==
          "FlatBuffers EngineSnapshotFbs pointer is null");
}

// TEST_CASE("CreateEngineSnapshot succeeds with valid FlatBuffers data",
//           "[unit][FlatbuffersEngineSnapshotProvider]") {
//   steamrot::EventHandler event_handler;
//
//   // Load test data
//   auto [data, engine_snapshot_fbs] =
//       steamrot::tests::LoadEngineSnapshotTestData();
//
//   steamrot::FlatbuffersEngineSnapshotProvider provider(event_handler,
//                                                        engine_snapshot_fbs);
//
//   auto result = provider.CreateEngineSnapshot();
//
//   REQUIRE(result.has_value());
//   auto &snapshot = result.value();
//
//   // Verify tick_number was configured
//   REQUIRE(snapshot.tick_number.has_value());
//   REQUIRE(snapshot.tick_number.value() == 42);
//
//   // Verify global_event_bus was configured
//   REQUIRE(snapshot.global_event_bus.has_value());
//   REQUIRE(snapshot.global_event_bus.value().size() == 2);
// }
//
/////////////////////////////////////////////////
// ConfigureEngineSnapshot tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureEngineSnapshot fails when FlatBuffers data is null",
          "[unit][FlatbuffersEngineSnapshotProvider]") {
  steamrot::EventHandler event_handler;
  steamrot::FlatbuffersEngineSnapshotProvider provider(event_handler);
  steamrot::EngineSnapshot snapshot;

  auto result = provider.ConfigureEngineSnapshot(snapshot);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
  REQUIRE(result.error().message ==
          "FlatBuffers EngineSnapshotFbs pointer is null");
}

// TEST_CASE("ConfigureEngineSnapshot succeeds with valid FlatBuffers data",
//           "[unit][FlatbuffersEngineSnapshotProvider]") {
//   steamrot::EventHandler event_handler;
//
//   // Load test data
//   auto [data, engine_snapshot_fbs] =
//       steamrot::tests::LoadEngineSnapshotTestData();
//
//   steamrot::FlatbuffersEngineSnapshotProvider provider(event_handler,
//                                                        engine_snapshot_fbs);
//   steamrot::EngineSnapshot snapshot;
//
//   auto result = provider.ConfigureEngineSnapshot(snapshot);
//
//   REQUIRE(result.has_value());
//
//   // Verify tick_number was configured
//   REQUIRE(snapshot.tick_number.has_value());
//   REQUIRE(snapshot.tick_number.value() == 42);
//
//   // Verify global_event_bus was configured
//   REQUIRE(snapshot.global_event_bus.has_value());
//   REQUIRE(snapshot.global_event_bus.value().size() == 2);
//
//   // Verify first event
//   REQUIRE(snapshot.global_event_bus.value()[0].type ==
//   steamrot::EventType::UI);
//   REQUIRE(snapshot.global_event_bus.value()[0].context.lifetime == 3);
//   REQUIRE(std::holds_alternative<steamrot::UIPayload>(
//       snapshot.global_event_bus.value()[0].payload));
//   steamrot::UIPayload ui_payload = std::get<steamrot::UIPayload>(
//       snapshot.global_event_bus.value()[0].payload);
//   REQUIRE(ui_payload.action == steamrot::UIPayload::UIAction::TOGGLE);
//
//   // Verify second event
//   REQUIRE(snapshot.global_event_bus.value()[1].type ==
//           steamrot::EventType::SCENE);
//   REQUIRE(snapshot.global_event_bus.value()[1].context.lifetime == 1);
//   REQUIRE(std::holds_alternative<steamrot::ScenePayload>(
//       snapshot.global_event_bus.value()[1].payload));
//   steamrot::ScenePayload scene_payload = std::get<steamrot::ScenePayload>(
//       snapshot.global_event_bus.value()[1].payload);
//   REQUIRE(scene_payload.action ==
//   steamrot::ScenePayload::SceneAction::CHANGE);
// }
