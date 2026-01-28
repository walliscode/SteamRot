/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for SceneDataEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneDataEqualsMatcher.h"
#include "SceneData.h"
#include "SceneInfo.h"
#include "containers.h"
#include "matcher_helpers.h"
#include "test_context.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

using namespace steamrot;
using namespace steamrot::tests;

TEST_CASE("SceneDataEqualsMatcher - complete match",
          "[unit][SceneData][matcher]") {
  uuids::uuid id = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(5);

  SceneData expected = CreateTestSceneData(SceneType::TEST, id, pool1);
  SceneData actual = CreateTestSceneData(SceneType::TEST, id, pool2);

  SECTION("Matching SceneData should match") {
    REQUIRE_THAT(actual, EqualsSceneData(expected));
  }
}

TEST_CASE("SceneDataEqualsMatcher - SceneInfo mismatch",
          "[unit][SceneData][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  uuids::uuid id2 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(5);

  SECTION("Different scene type should not match") {
    SceneData expected = CreateTestSceneData(SceneType::TEST, id1, pool1);
    SceneData actual = CreateTestSceneData(SceneType::TITLE, id1, pool2);
    REQUIRE_THAT(actual, !EqualsSceneData(expected));
  }

  SECTION("Different scene ID should not match") {
    SceneData expected = CreateTestSceneData(SceneType::TEST, id1, pool1);
    SceneData actual = CreateTestSceneData(SceneType::TEST, id2, pool2);
    REQUIRE_THAT(actual, !EqualsSceneData(expected));
  }

  SECTION("Both different should not match") {
    SceneData expected = CreateTestSceneData(SceneType::TEST, id1, pool1);
    SceneData actual = CreateTestSceneData(SceneType::CRAFTING, id2, pool2);
    REQUIRE_THAT(actual, !EqualsSceneData(expected));
  }
}

TEST_CASE("SceneDataEqualsMatcher - entity transport mismatch",
          "[unit][SceneData][matcher]") {
  uuids::uuid id = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(3);

  SECTION("Different pool sizes should not match") {
    SceneData expected = CreateTestSceneData(SceneType::TEST, id, pool1);
    SceneData actual = CreateTestSceneData(SceneType::TEST, id, pool2);
    REQUIRE_THAT(actual, !EqualsSceneData(expected));
  }

  SECTION("Different variant types should not match") {
    SceneData expected = CreateTestSceneData(SceneType::TEST, id, pool1);
    SceneData actual;
    actual.scene_info.type = SceneType::TEST;
    actual.scene_info.id = id;
    actual.entity_transport = std::monostate{};
    REQUIRE_THAT(actual, !EqualsSceneData(expected));
  }
}

TEST_CASE("SceneDataEqualsMatcher - multiple field mismatches",
          "[unit][SceneData][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  uuids::uuid id2 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(3);

  SceneData expected = CreateTestSceneData(SceneType::TEST, id1, pool1);
  SceneData actual = CreateTestSceneData(SceneType::CRAFTING, id2, pool2);

  SECTION("Multiple mismatches should not match") {
    REQUIRE_THAT(actual, !EqualsSceneData(expected));
  }

  SECTION("Description should contain all mismatches") {
    auto matcher = EqualsSceneData(expected);
    matcher.match(actual);
    std::string description = matcher.describe();

    REQUIRE(description.find("SceneData Mismatch") != std::string::npos);
    REQUIRE(description.find("SceneInfo Mismatch") != std::string::npos);
    REQUIRE(description.find("EntityTransport Mismatch") != std::string::npos);
  }
}

TEST_CASE("SceneDataEqualsMatcher describe method - success",
          "[unit][SceneData][matcher]") {
  uuids::uuid id = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(5);

  SceneData expected = CreateTestSceneData(SceneType::TEST, id, pool1);
  SceneData actual = CreateTestSceneData(SceneType::TEST, id, pool2);

  auto matcher = EqualsSceneData(expected);
  matcher.match(actual);

  std::string description = matcher.describe();

  SECTION("Description should contain success header") {
    REQUIRE(description.find("SceneData Match") != std::string::npos);
  }

  SECTION("Description should contain scene UUID") {
    std::string uuid_str = uuids::to_string(id);
    REQUIRE(description.find(uuid_str) != std::string::npos);
  }
}

TEST_CASE("SceneDataEqualsMatcher describe method - SceneInfo mismatch",
          "[unit][SceneData][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  uuids::uuid id2 = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(5);

  SceneData expected = CreateTestSceneData(SceneType::TEST, id1, pool1);
  SceneData actual = CreateTestSceneData(SceneType::TITLE, id2, pool2);

  auto matcher = EqualsSceneData(expected);
  matcher.match(actual);

  std::string description = matcher.describe();

  SECTION("Description should contain mismatch header") {
    REQUIRE(description.find("SceneData Mismatch") != std::string::npos);
  }

  SECTION("Description should contain SceneInfo mismatch details") {
    REQUIRE(description.find("SceneInfo Mismatch") != std::string::npos);
    REQUIRE(description.find("Scene type differs") != std::string::npos);
    REQUIRE(description.find("Scene ID differs") != std::string::npos);
  }

  SECTION("Description should contain scene UUID") {
    std::string uuid_str = uuids::to_string(id1);
    REQUIRE(description.find(uuid_str) != std::string::npos);
  }
}

TEST_CASE("SceneDataEqualsMatcher describe method - entity transport mismatch",
          "[unit][SceneData][matcher]") {
  uuids::uuid id = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(3);

  SceneData expected = CreateTestSceneData(SceneType::TEST, id, pool1);
  SceneData actual = CreateTestSceneData(SceneType::TEST, id, pool2);

  auto matcher = EqualsSceneData(expected);
  matcher.match(actual);

  std::string description = matcher.describe();

  SECTION("Description should contain mismatch header") {
    REQUIRE(description.find("SceneData Mismatch") != std::string::npos);
  }

  SECTION("Description should contain EntityTransport mismatch details") {
    REQUIRE(description.find("EntityTransport Mismatch") != std::string::npos);
  }

  SECTION("Description should contain scene UUID") {
    std::string uuid_str = uuids::to_string(id);
    REQUIRE(description.find(uuid_str) != std::string::npos);
  }
}

TEST_CASE("SceneDataEqualsMatcher with TestContext",
          "[unit][SceneData][matcher]") {
  uuids::uuid id = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(5);

  SceneData expected = CreateTestSceneData(SceneType::TEST, id, pool1);
  SceneData actual = CreateTestSceneData(SceneType::TEST, id, pool2);

  TestContext context{"scene_comparison_test", "Test SceneData comparison", 3,
                      10};

  SECTION("Matcher with context should match") {
    REQUIRE_THAT(actual, EqualsSceneData(expected, context));
  }

  SECTION("Describe with context should include test metadata") {
    auto matcher = EqualsSceneData(expected, context);
    matcher.match(actual);
    std::string description = matcher.describe();

    REQUIRE(description.find("scene_comparison_test") != std::string::npos);
    REQUIRE(description.find("Tick: 3/10") != std::string::npos);
  }

  SECTION("Describe on failure with context should include metadata") {
    SceneData different_actual =
        CreateTestSceneData(SceneType::CRAFTING, id, pool2);
    auto matcher = EqualsSceneData(expected, context);
    matcher.match(different_actual);
    std::string description = matcher.describe();

    REQUIRE(description.find("scene_comparison_test") != std::string::npos);
    REQUIRE(description.find("Tick: 3/10") != std::string::npos);
    REQUIRE(description.find("SceneInfo Mismatch") != std::string::npos);
  }
}

TEST_CASE("SceneDataEqualsMatcher - monostate entity transport",
          "[unit][SceneData][matcher]") {
  uuids::uuid id = uuids::uuid_system_generator{}();

  SceneData expected;
  expected.scene_info.type = SceneType::TEST;
  expected.scene_info.id = id;
  expected.entity_transport = std::monostate{};

  SceneData actual;
  actual.scene_info.type = SceneType::TEST;
  actual.scene_info.id = id;
  actual.entity_transport = std::monostate{};

  SECTION("Both monostate should match") {
    REQUIRE_THAT(actual, EqualsSceneData(expected));
  }
}

TEST_CASE("SceneDataEqualsMatcher - shared_ptr entity transport",
          "[unit][SceneData][matcher]") {
  uuids::uuid id = uuids::uuid_system_generator{}();
  auto pool1 = std::make_shared<EntityMemoryPool>(CreateTestPool(5));
  auto pool2 = std::make_shared<EntityMemoryPool>(CreateTestPool(5));

  SceneData expected;
  expected.scene_info.type = SceneType::TEST;
  expected.scene_info.id = id;
  expected.entity_transport = pool1;

  SceneData actual;
  actual.scene_info.type = SceneType::TEST;
  actual.scene_info.id = id;
  actual.entity_transport = pool2;

  SECTION("Matching shared_ptr pools should match") {
    REQUIRE_THAT(actual, EqualsSceneData(expected));
  }

  SECTION("Different shared_ptr pool sizes should not match") {
    auto different_pool = std::make_shared<EntityMemoryPool>(CreateTestPool(3));
    actual.entity_transport = different_pool;
    REQUIRE_THAT(actual, !EqualsSceneData(expected));
  }
}
