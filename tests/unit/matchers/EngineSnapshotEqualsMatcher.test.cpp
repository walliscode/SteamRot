/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EngineSnapshotEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EngineSnapshotEqualsMatcher.h"
#include "EngineSnapshot.h"
#include "EventPacket.h"
#include "SceneData.h"
#include "matcher_helpers.h"
#include "test_context.h"
#include <catch2/catch_test_macros.hpp>

using namespace steamrot;
using namespace steamrot::tests;

TEST_CASE("EngineSnapshotEqualsMatcher - minimal comparison "
          "(scene_collection_data only)",
          "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EngineSnapshot expected;
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  actual.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

  SECTION("Minimal snapshot should match") {
    REQUIRE_THAT(actual, EqualsEngineSnapshot(expected));
  }
}

TEST_CASE("EngineSnapshotEqualsMatcher - minimal comparison mismatch",
          "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EngineSnapshot expected;
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  actual.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::CRAFTING, id1, CreateTestPool(5)));

  SECTION("Different scene types should not match") {
    REQUIRE_THAT(actual, !EqualsEngineSnapshot(expected));
  }
}

TEST_CASE("EngineSnapshotEqualsMatcher - with tick_number match",
          "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EngineSnapshot expected;
  expected.tick_number = 42;
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  actual.tick_number = 42;
  actual.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

  SECTION("Matching tick numbers should match") {
    REQUIRE_THAT(actual, EqualsEngineSnapshot(expected));
  }
}

TEST_CASE("EngineSnapshotEqualsMatcher - with tick_number mismatch",
          "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EngineSnapshot expected;
  expected.tick_number = 42;
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  actual.tick_number = 100;
  actual.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

  SECTION("Different tick numbers should not match") {
    REQUIRE_THAT(actual, !EqualsEngineSnapshot(expected));
  }

  SECTION("Description should mention tick number mismatch") {
    auto matcher = EqualsEngineSnapshot(expected);
    matcher.match(actual);
    std::string description = matcher.describe();

    REQUIRE(description.find("Tick number mismatch") != std::string::npos);
    REQUIRE(description.find("expected 42") != std::string::npos);
    REQUIRE(description.find("got 100") != std::string::npos);
  }
}

TEST_CASE("EngineSnapshotEqualsMatcher - tick_number expected but not present",
          "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EngineSnapshot expected;
  expected.tick_number = 42;
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  // No tick_number set
  actual.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

  SECTION("Expected tick_number but actual has none should not match") {
    REQUIRE_THAT(actual, !EqualsEngineSnapshot(expected));
  }

  SECTION("Description should mention missing tick_number") {
    auto matcher = EqualsEngineSnapshot(expected);
    matcher.match(actual);
    std::string description = matcher.describe();

    REQUIRE(description.find(
                "Expected tick_number, but Actual snapshot has none") !=
            std::string::npos);
  }
}

TEST_CASE(
    "EngineSnapshotEqualsMatcher - tick_number not expected and not present",
    "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EngineSnapshot expected;
  // No tick_number set
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  actual.tick_number = 42; // Present but not expected
  actual.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

  SECTION("Tick_number not expected should be ignored if present") {
    REQUIRE_THAT(actual, EqualsEngineSnapshot(expected));
  }
}

TEST_CASE("EngineSnapshotEqualsMatcher - with global_event_bus match",
          "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EventPacket event1{1};
  event1.type = EventType::SCENE;

  EngineSnapshot expected;
  expected.global_event_bus = EventBus{event1};
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  actual.global_event_bus = EventBus{event1};
  actual.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

  SECTION("Matching event buses should match") {
    REQUIRE_THAT(actual, EqualsEngineSnapshot(expected));
  }
}

TEST_CASE("EngineSnapshotEqualsMatcher - with global_event_bus mismatch",
          "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EventPacket event1{1};
  event1.type = EventType::SCENE;

  EventPacket event2{1};
  event2.type = EventType::SYSTEM;

  EngineSnapshot expected;
  expected.global_event_bus = EventBus{event1};
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  actual.global_event_bus = EventBus{event2};
  actual.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

  SECTION("Different event buses should not match") {
    REQUIRE_THAT(actual, !EqualsEngineSnapshot(expected));
  }
}

TEST_CASE(
    "EngineSnapshotEqualsMatcher - global_event_bus expected but not present",
    "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EventPacket event1{1};
  event1.type = EventType::SCENE;

  EngineSnapshot expected;
  expected.global_event_bus = EventBus{event1};
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  // No global_event_bus set
  actual.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

  SECTION("Expected global_event_bus but actual has none should not match") {
    REQUIRE_THAT(actual, !EqualsEngineSnapshot(expected));
  }

  SECTION("Description should mention missing global_event_bus") {
    auto matcher = EqualsEngineSnapshot(expected);
    matcher.match(actual);
    std::string description = matcher.describe();

    REQUIRE(
        description.find("Expected global_event_bus, but actual has none") !=
        std::string::npos);
  }
}

TEST_CASE("EngineSnapshotEqualsMatcher - global_event_bus not expected",
          "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EventPacket event1{1};
  event1.type = EventType::SCENE;

  EngineSnapshot expected;
  // No global_event_bus set
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  actual.global_event_bus = EventBus{event1}; // Present but not expected
  actual.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

  SECTION("Global_event_bus not expected should be ignored if present") {
    REQUIRE_THAT(actual, EqualsEngineSnapshot(expected));
  }
}

TEST_CASE("EngineSnapshotEqualsMatcher - all fields populated match",
          "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EventPacket event1{1};
  event1.type = EventType::SCENE;

  EngineSnapshot expected;
  expected.tick_number = 42;
  expected.global_event_bus = EventBus{event1};
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  actual.tick_number = 42;
  actual.global_event_bus = EventBus{event1};
  actual.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

  SECTION("All fields matching should match") {
    REQUIRE_THAT(actual, EqualsEngineSnapshot(expected));
  }
}

TEST_CASE("EngineSnapshotEqualsMatcher - multiple field mismatches",
          "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EventPacket event1{1};
  event1.type = EventType::SCENE;

  EventPacket event2{1};
  event2.type = EventType::SYSTEM;

  EngineSnapshot expected;
  expected.tick_number = 42;
  expected.global_event_bus = EventBus{event1};
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  actual.tick_number = 100;                   // Different
  actual.global_event_bus = EventBus{event2}; // Different
  actual.scene_collection_data.push_back(CreateTestSceneData(
      SceneType::CRAFTING, id1, CreateTestPool(5))); // Different

  SECTION("Multiple field mismatches should not match") {
    REQUIRE_THAT(actual, !EqualsEngineSnapshot(expected));
  }

  SECTION("Description should contain all mismatches") {
    auto matcher = EqualsEngineSnapshot(expected);
    matcher.match(actual);
    std::string description = matcher.describe();

    REQUIRE(description.find("Tick number mismatch") != std::string::npos);
    REQUIRE(description.find("expected 42") != std::string::npos);
    REQUIRE(description.find("got 100") != std::string::npos);
    // EventBus and SceneCollection mismatches would also be present
  }
}

TEST_CASE("EngineSnapshotEqualsMatcher - selective comparison only tick_number",
          "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EventPacket event1{1};
  event1.type = EventType::SCENE;

  EngineSnapshot expected;
  expected.tick_number = 42;
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  actual.tick_number = 42;
  actual.global_event_bus = EventBus{event1}; // Present but not expected
  actual.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

  SECTION("Only verifies fields set in expected") {
    REQUIRE_THAT(actual, EqualsEngineSnapshot(expected));
  }
}

TEST_CASE("EngineSnapshotEqualsMatcher - selective comparison only event_bus",
          "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EventPacket event1{1};
  event1.type = EventType::SCENE;

  EngineSnapshot expected;
  expected.global_event_bus = EventBus{event1};
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  actual.tick_number = 42; // Present but not expected
  actual.global_event_bus = EventBus{event1};
  actual.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

  SECTION("Only verifies fields set in expected") {
    REQUIRE_THAT(actual, EqualsEngineSnapshot(expected));
  }
}

TEST_CASE("EngineSnapshotEqualsMatcher - describe method success",
          "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EngineSnapshot expected;
  expected.tick_number = 42;
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  actual.tick_number = 42;
  actual.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

  auto matcher = EqualsEngineSnapshot(expected);
  matcher.match(actual);
  std::string description = matcher.describe();

  SECTION("Description should contain success header") {
    REQUIRE(description.find("EngineSnapshot Match") != std::string::npos);
  }

  SECTION("Description should indicate success") {
    REQUIRE(description.find("All fields matched successfully") !=
            std::string::npos);
  }
}

TEST_CASE("EngineSnapshotEqualsMatcher - describe method failure",
          "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EngineSnapshot expected;
  expected.tick_number = 42;
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  actual.tick_number = 100;
  actual.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

  auto matcher = EqualsEngineSnapshot(expected);
  matcher.match(actual);
  std::string description = matcher.describe();

  SECTION("Description should contain mismatch header") {
    REQUIRE(description.find("EngineSnapshot Mismatch") != std::string::npos);
  }

  SECTION("Description should contain specific mismatch details") {
    REQUIRE(description.find("Tick number mismatch") != std::string::npos);
  }
}

TEST_CASE("EngineSnapshotEqualsMatcher - with TestContext success",
          "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EngineSnapshot expected;
  expected.tick_number = 42;
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  actual.tick_number = 42;
  actual.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

  TestContext context{"engine_snapshot_test",
                      "Test EngineSnapshot comparison with context", 5, 20};

  SECTION("Matcher with context should match") {
    REQUIRE_THAT(actual, EqualsEngineSnapshot(expected, context));
  }

  SECTION("Describe with context should include test metadata") {
    auto matcher = EqualsEngineSnapshot(expected, context);
    matcher.match(actual);
    std::string description = matcher.describe();

    REQUIRE(description.find("engine_snapshot_test") != std::string::npos);
    REQUIRE(description.find("Tick: 5/20") != std::string::npos);
  }
}

TEST_CASE("EngineSnapshotEqualsMatcher - with TestContext failure",
          "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EngineSnapshot expected;
  expected.tick_number = 42;
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  actual.tick_number = 100;
  actual.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

  TestContext context{"engine_snapshot_test",
                      "Test EngineSnapshot comparison with context", 5, 20};

  SECTION("Describe on failure with context should include metadata") {
    auto matcher = EqualsEngineSnapshot(expected, context);
    matcher.match(actual);
    std::string description = matcher.describe();

    REQUIRE(description.find("engine_snapshot_test") != std::string::npos);
    REQUIRE(description.find("Tick: 5/20") != std::string::npos);
    REQUIRE(description.find("Tick number mismatch") != std::string::npos);
  }
}

TEST_CASE("EngineSnapshotEqualsMatcher - empty scene collection",
          "[unit][EngineSnapshot][matcher]") {
  EngineSnapshot expected;
  // Empty scene_collection_data

  EngineSnapshot actual;
  // Empty scene_collection_data

  SECTION("Empty scene collections should match") {
    REQUIRE_THAT(actual, EqualsEngineSnapshot(expected));
  }
}

TEST_CASE("EngineSnapshotEqualsMatcher - scene collection size mismatch",
          "[unit][EngineSnapshot][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);

  EngineSnapshot expected;
  expected.scene_collection_data.push_back(
      CreateTestSceneData(SceneType::TEST, id1, pool1));

  EngineSnapshot actual;
  // Empty scene_collection_data

  SECTION("Different scene collection sizes should not match") {
    REQUIRE_THAT(actual, !EqualsEngineSnapshot(expected));
  }
}
