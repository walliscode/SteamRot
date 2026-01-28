/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for SceneCollectionDataEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneCollectionDataEqualsMatcher.h"
#include "SceneData.h"
#include "SceneInfo.h"
#include "containers.h"
#include "matcher_helpers.h"
#include "test_context.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

using namespace steamrot;
using namespace steamrot::tests;

/////////////////////////////////////////////////
/// @brief Helper to create a simple SceneData for testing
/////////////////////////////////////////////////
SceneData CreateTestSceneData(SceneType type, const uuids::uuid &id,
                              EntityMemoryPool pool) {
  SceneData data;
  data.scene_info.type = type;
  data.scene_info.id = id;
  data.entity_transport = pool;
  return data;
}

TEST_CASE("SceneCollectionDataEqualsMatcher - empty collections",
          "[unit][SceneCollectionData][matcher]") {
  SceneCollectionData expected;
  SceneCollectionData actual;

  SECTION("Both empty collections should match") {
    REQUIRE_THAT(actual, EqualsSceneCollection(expected));
  }
}

TEST_CASE("SceneCollectionDataEqualsMatcher - single scene match",
          "[unit][SceneCollectionData][matcher]") {
  uuids::uuid id = uuids::uuid_system_generator{}();
  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(5);

  SceneCollectionData expected;
  expected.push_back(CreateTestSceneData(SceneType::TEST, id, pool1));

  SceneCollectionData actual;
  actual.push_back(CreateTestSceneData(SceneType::TEST, id, pool2));

  SECTION("Single matching scene should match") {
    REQUIRE_THAT(actual, EqualsSceneCollection(expected));
  }
}

TEST_CASE("SceneCollectionDataEqualsMatcher - multiple scenes all matching",
          "[unit][SceneCollectionData][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  uuids::uuid id2 = uuids::uuid_system_generator{}();
  uuids::uuid id3 = uuids::uuid_system_generator{}();

  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(3);
  EntityMemoryPool pool3 = CreateTestPool(7);

  SceneCollectionData expected;
  expected.push_back(CreateTestSceneData(SceneType::TEST, id1, pool1));
  expected.push_back(CreateTestSceneData(SceneType::CRAFTING, id2, pool2));
  expected.push_back(CreateTestSceneData(SceneType::TITLE, id3, pool3));

  SceneCollectionData actual;
  actual.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));
  actual.push_back(
      CreateTestSceneData(SceneType::CRAFTING, id2, CreateTestPool(3)));
  actual.push_back(
      CreateTestSceneData(SceneType::TITLE, id3, CreateTestPool(7)));

  SECTION("Multiple matching scenes should match") {
    REQUIRE_THAT(actual, EqualsSceneCollection(expected));
  }
}

TEST_CASE("SceneCollectionDataEqualsMatcher - size mismatch",
          "[unit][SceneCollectionData][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  uuids::uuid id2 = uuids::uuid_system_generator{}();

  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(3);

  SECTION("Expected has more scenes") {
    SceneCollectionData expected;
    expected.push_back(CreateTestSceneData(SceneType::TEST, id1, pool1));
    expected.push_back(CreateTestSceneData(SceneType::CRAFTING, id2, pool2));

    SceneCollectionData actual;
    actual.push_back(
        CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

    REQUIRE_THAT(actual, !EqualsSceneCollection(expected));
  }

  SECTION("Actual has more scenes") {
    SceneCollectionData expected;
    expected.push_back(CreateTestSceneData(SceneType::TEST, id1, pool1));

    SceneCollectionData actual;
    actual.push_back(
        CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));
    actual.push_back(
        CreateTestSceneData(SceneType::CRAFTING, id2, CreateTestPool(3)));

    REQUIRE_THAT(actual, !EqualsSceneCollection(expected));
  }

  SECTION("Description should mention size mismatch") {
    SceneCollectionData expected;
    expected.push_back(CreateTestSceneData(SceneType::TEST, id1, pool1));
    expected.push_back(CreateTestSceneData(SceneType::CRAFTING, id2, pool2));

    SceneCollectionData actual;
    actual.push_back(
        CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

    auto matcher = EqualsSceneCollection(expected);
    matcher.match(actual);
    std::string description = matcher.describe();

    REQUIRE(description.find("Scene collection size mismatch") !=
            std::string::npos);
    REQUIRE(description.find("expected 2") != std::string::npos);
    REQUIRE(description.find("got 1") != std::string::npos);
  }
}

TEST_CASE("SceneCollectionDataEqualsMatcher - one scene mismatch",
          "[unit][SceneCollectionData][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  uuids::uuid id2 = uuids::uuid_system_generator{}();
  uuids::uuid id3 = uuids::uuid_system_generator{}();

  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(3);
  EntityMemoryPool pool3 = CreateTestPool(7);

  SceneCollectionData expected;
  expected.push_back(CreateTestSceneData(SceneType::TEST, id1, pool1));
  expected.push_back(CreateTestSceneData(SceneType::CRAFTING, id2, pool2));
  expected.push_back(CreateTestSceneData(SceneType::TITLE, id3, pool3));

  SECTION("Middle scene has different type") {
    SceneCollectionData actual;
    actual.push_back(
        CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));
    actual.push_back(CreateTestSceneData(SceneType::TITLE, id2,
                                         CreateTestPool(3))); // Different type
    actual.push_back(
        CreateTestSceneData(SceneType::TITLE, id3, CreateTestPool(7)));

    REQUIRE_THAT(actual, !EqualsSceneCollection(expected));
  }

  SECTION("Last scene has different pool size") {
    SceneCollectionData actual;
    actual.push_back(
        CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));
    actual.push_back(
        CreateTestSceneData(SceneType::CRAFTING, id2, CreateTestPool(3)));
    actual.push_back(CreateTestSceneData(SceneType::TITLE, id3,
                                         CreateTestPool(5))); // Different size

    REQUIRE_THAT(actual, !EqualsSceneCollection(expected));
  }

  SECTION("Description should contain scene index and UUID") {
    SceneCollectionData actual;
    actual.push_back(
        CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));
    actual.push_back(CreateTestSceneData(SceneType::TITLE, id2,
                                         CreateTestPool(3))); // Different type
    actual.push_back(
        CreateTestSceneData(SceneType::TITLE, id3, CreateTestPool(7)));

    auto matcher = EqualsSceneCollection(expected);
    matcher.match(actual);
    std::string description = matcher.describe();

    REQUIRE(description.find("Scene [1]") != std::string::npos);
    REQUIRE(description.find(uuids::to_string(id2)) != std::string::npos);
    REQUIRE(description.find("SceneInfo Mismatch") != std::string::npos);
  }
}

TEST_CASE("SceneCollectionDataEqualsMatcher - multiple scene mismatches",
          "[unit][SceneCollectionData][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  uuids::uuid id2 = uuids::uuid_system_generator{}();
  uuids::uuid id3 = uuids::uuid_system_generator{}();

  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(3);
  EntityMemoryPool pool3 = CreateTestPool(7);

  SceneCollectionData expected;
  expected.push_back(CreateTestSceneData(SceneType::TEST, id1, pool1));
  expected.push_back(CreateTestSceneData(SceneType::CRAFTING, id2, pool2));
  expected.push_back(CreateTestSceneData(SceneType::TITLE, id3, pool3));

  SceneCollectionData actual;
  actual.push_back(CreateTestSceneData(
      SceneType::TITLE, id1, CreateTestPool(3))); // Different type and size
  actual.push_back(
      CreateTestSceneData(SceneType::CRAFTING, id2, CreateTestPool(3)));
  actual.push_back(CreateTestSceneData(
      SceneType::TEST, id3, CreateTestPool(5))); // Different type and size

  SECTION("Multiple mismatches should not match") {
    REQUIRE_THAT(actual, !EqualsSceneCollection(expected));
  }

  SECTION("Description should contain all mismatches") {
    auto matcher = EqualsSceneCollection(expected);
    matcher.match(actual);
    std::string description = matcher.describe();

    REQUIRE(description.find("Scene [0]") != std::string::npos);
    REQUIRE(description.find(uuids::to_string(id1)) != std::string::npos);
    REQUIRE(description.find("Scene [2]") != std::string::npos);
    REQUIRE(description.find(uuids::to_string(id3)) != std::string::npos);
  }
}

TEST_CASE("SceneCollectionDataEqualsMatcher - scene order matters",
          "[unit][SceneCollectionData][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  uuids::uuid id2 = uuids::uuid_system_generator{}();

  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(3);

  SceneCollectionData expected;
  expected.push_back(CreateTestSceneData(SceneType::TEST, id1, pool1));
  expected.push_back(CreateTestSceneData(SceneType::CRAFTING, id2, pool2));

  SceneCollectionData actual;
  actual.push_back(CreateTestSceneData(SceneType::CRAFTING, id2,
                                       CreateTestPool(3))); // Swapped order
  actual.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));

  SECTION("Different order should not match") {
    REQUIRE_THAT(actual, !EqualsSceneCollection(expected));
  }

  SECTION("Description should show mismatches at both indices") {
    auto matcher = EqualsSceneCollection(expected);
    matcher.match(actual);
    std::string description = matcher.describe();

    REQUIRE(description.find("Scene [0]") != std::string::npos);
    REQUIRE(description.find("Scene [1]") != std::string::npos);
  }
}

TEST_CASE("SceneCollectionDataEqualsMatcher describe method - success",
          "[unit][SceneCollectionData][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  uuids::uuid id2 = uuids::uuid_system_generator{}();

  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(3);

  SceneCollectionData expected;
  expected.push_back(CreateTestSceneData(SceneType::TEST, id1, pool1));
  expected.push_back(CreateTestSceneData(SceneType::CRAFTING, id2, pool2));

  SceneCollectionData actual;
  actual.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));
  actual.push_back(
      CreateTestSceneData(SceneType::CRAFTING, id2, CreateTestPool(3)));

  auto matcher = EqualsSceneCollection(expected);
  matcher.match(actual);

  std::string description = matcher.describe();

  SECTION("Description should contain success header") {
    REQUIRE(description.find("SceneCollection Match") != std::string::npos);
  }

  SECTION("Description should contain collection size") {
    REQUIRE(description.find("Collection size: 2") != std::string::npos);
  }
}

TEST_CASE("SceneCollectionDataEqualsMatcher describe method - failure",
          "[unit][SceneCollectionData][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  uuids::uuid id2 = uuids::uuid_system_generator{}();

  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(3);

  SceneCollectionData expected;
  expected.push_back(CreateTestSceneData(SceneType::TEST, id1, pool1));
  expected.push_back(CreateTestSceneData(SceneType::CRAFTING, id2, pool2));

  SceneCollectionData actual;
  actual.push_back(CreateTestSceneData(SceneType::TITLE, id1,
                                       CreateTestPool(5))); // Different type
  actual.push_back(
      CreateTestSceneData(SceneType::CRAFTING, id2, CreateTestPool(3)));

  auto matcher = EqualsSceneCollection(expected);
  matcher.match(actual);

  std::string description = matcher.describe();

  SECTION("Description should contain mismatch header") {
    REQUIRE(description.find("SceneCollection Mismatch") != std::string::npos);
  }

  SECTION("Description should contain scene details") {
    REQUIRE(description.find("Scene [0]") != std::string::npos);
    REQUIRE(description.find(uuids::to_string(id1)) != std::string::npos);
  }
}

TEST_CASE("SceneCollectionDataEqualsMatcher with TestContext",
          "[unit][SceneCollectionData][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  uuids::uuid id2 = uuids::uuid_system_generator{}();

  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(3);

  SceneCollectionData expected;
  expected.push_back(CreateTestSceneData(SceneType::TEST, id1, pool1));
  expected.push_back(CreateTestSceneData(SceneType::CRAFTING, id2, pool2));

  SceneCollectionData actual;
  actual.push_back(
      CreateTestSceneData(SceneType::TEST, id1, CreateTestPool(5)));
  actual.push_back(
      CreateTestSceneData(SceneType::CRAFTING, id2, CreateTestPool(3)));

  TestContext context{"scene_collection_test",
                      "Test SceneCollectionData comparison", 5, 20};

  SECTION("Matcher with context should match") {
    REQUIRE_THAT(actual, EqualsSceneCollection(expected, context));
  }

  SECTION("Describe with context should include test metadata") {
    auto matcher = EqualsSceneCollection(expected, context);
    matcher.match(actual);
    std::string description = matcher.describe();

    REQUIRE(description.find("scene_collection_test") != std::string::npos);
    REQUIRE(description.find("Tick: 5/20") != std::string::npos);
  }

  SECTION("Describe on failure with context should include metadata") {
    SceneCollectionData different_actual;
    different_actual.push_back(CreateTestSceneData(
        SceneType::TITLE, id1, CreateTestPool(5))); // Different type
    different_actual.push_back(
        CreateTestSceneData(SceneType::CRAFTING, id2, CreateTestPool(3)));

    auto matcher = EqualsSceneCollection(expected, context);
    matcher.match(different_actual);
    std::string description = matcher.describe();

    REQUIRE(description.find("scene_collection_test") != std::string::npos);
    REQUIRE(description.find("Tick: 5/20") != std::string::npos);
    REQUIRE(description.find("Scene [0]") != std::string::npos);
  }
}

TEST_CASE("SceneCollectionDataEqualsMatcher - monostate entity transport",
          "[unit][SceneCollectionData][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  uuids::uuid id2 = uuids::uuid_system_generator{}();

  SceneData scene1;
  scene1.scene_info.type = SceneType::TEST;
  scene1.scene_info.id = id1;
  scene1.entity_transport = std::monostate{};

  SceneData scene2;
  scene2.scene_info.type = SceneType::CRAFTING;
  scene2.scene_info.id = id2;
  scene2.entity_transport = std::monostate{};

  SceneCollectionData expected;
  expected.push_back(std::move(scene1));
  expected.push_back(std::move(scene2));

  SceneData actual_scene1;
  actual_scene1.scene_info.type = SceneType::TEST;
  actual_scene1.scene_info.id = id1;
  actual_scene1.entity_transport = std::monostate{};

  SceneData actual_scene2;
  actual_scene2.scene_info.type = SceneType::CRAFTING;
  actual_scene2.scene_info.id = id2;
  actual_scene2.entity_transport = std::monostate{};

  SceneCollectionData actual;
  actual.push_back(std::move(actual_scene1));
  actual.push_back(std::move(actual_scene2));

  SECTION("Collections with monostate transports should match") {
    REQUIRE_THAT(actual, EqualsSceneCollection(expected));
  }
}

TEST_CASE("SceneCollectionDataEqualsMatcher - shared_ptr entity transport",
          "[unit][SceneCollectionData][matcher]") {
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  uuids::uuid id2 = uuids::uuid_system_generator{}();

  auto pool1 = std::make_shared<EntityMemoryPool>(CreateTestPool(5));
  auto pool2 = std::make_shared<EntityMemoryPool>(CreateTestPool(3));

  SceneData scene1;
  scene1.scene_info.type = SceneType::TEST;
  scene1.scene_info.id = id1;
  scene1.entity_transport = pool1;

  SceneData scene2;
  scene2.scene_info.type = SceneType::CRAFTING;
  scene2.scene_info.id = id2;
  scene2.entity_transport = pool2;

  SceneCollectionData expected;
  expected.push_back(std::move(scene1));
  expected.push_back(std::move(scene2));

  auto actual_pool1 = std::make_shared<EntityMemoryPool>(CreateTestPool(5));
  auto actual_pool2 = std::make_shared<EntityMemoryPool>(CreateTestPool(3));

  SceneData actual_scene1;
  actual_scene1.scene_info.type = SceneType::TEST;
  actual_scene1.scene_info.id = id1;
  actual_scene1.entity_transport = actual_pool1;

  SceneData actual_scene2;
  actual_scene2.scene_info.type = SceneType::CRAFTING;
  actual_scene2.scene_info.id = id2;
  actual_scene2.entity_transport = actual_pool2;

  SceneCollectionData actual;
  actual.push_back(std::move(actual_scene1));
  actual.push_back(std::move(actual_scene2));

  SECTION("Collections with matching shared_ptr pools should match") {
    REQUIRE_THAT(actual, EqualsSceneCollection(expected));
  }

  SECTION("Collections with different shared_ptr pool sizes should not match") {
    auto different_pool = std::make_shared<EntityMemoryPool>(CreateTestPool(7));
    actual_scene2.entity_transport = different_pool;

    SceneCollectionData different_actual;
    different_actual.push_back(std::move(actual_scene1));
    different_actual.push_back(std::move(actual_scene2));

    REQUIRE_THAT(different_actual, !EqualsSceneCollection(expected));
  }
}
