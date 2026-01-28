/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EntityTransportEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EntityTransportEqualsMatcher.h"
#include "CMeta.h"
#include "CUserInterface.h"
#include "IEntityImporter.h"
#include "conmat.h"
#include "containers.h"
#include "entity_memory.h"
#include "matcher_helpers.h"
#include "test_context.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <sstream>

using namespace steamrot;
using namespace steamrot::tests;

TEST_CASE("EntityTransportEqualsMatcher - both monostate",
          "[unit][EntityTransport][matcher]") {
  EntityTransportVariant expected = std::monostate{};
  EntityTransportVariant actual = std::monostate{};

  SECTION("Both monostate should match") {
    REQUIRE_THAT(actual, EqualsEntityTransport(expected));
  }
}

TEST_CASE("EntityTransportEqualsMatcher - both EntityMemoryPool",
          "[unit][EntityTransport][matcher]") {
  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(5);

  EntityTransportVariant expected = pool1;
  EntityTransportVariant actual = pool2;

  SECTION("Matching pools should match") {
    REQUIRE_THAT(actual, EqualsEntityTransport(expected));
  }

  SECTION("Different pool sizes should not match") {
    EntityMemoryPool different_pool = CreateTestPool(3);
    EntityTransportVariant different_actual = different_pool;
    REQUIRE_THAT(different_actual, !EqualsEntityTransport(expected));
  }
}

TEST_CASE("EntityTransportEqualsMatcher - both shared_ptr",
          "[unit][EntityTransport][matcher]") {
  auto pool1 = std::make_shared<EntityMemoryPool>(CreateTestPool(5));
  auto pool2 = std::make_shared<EntityMemoryPool>(CreateTestPool(5));

  EntityTransportVariant expected = pool1;
  EntityTransportVariant actual = pool2;

  SECTION("Matching dereferenced pools should match") {
    REQUIRE_THAT(actual, EqualsEntityTransport(expected));
  }

  SECTION("Both null pointers should match") {
    std::shared_ptr<EntityMemoryPool> null_ptr1 = nullptr;
    std::shared_ptr<EntityMemoryPool> null_ptr2 = nullptr;
    EntityTransportVariant null_expected = null_ptr1;
    EntityTransportVariant null_actual = null_ptr2;
    REQUIRE_THAT(null_actual, EqualsEntityTransport(null_expected));
  }

  SECTION("One null pointer should not match") {
    std::shared_ptr<EntityMemoryPool> null_ptr = nullptr;
    EntityTransportVariant null_actual = null_ptr;
    REQUIRE_THAT(null_actual, !EqualsEntityTransport(expected));
  }

  SECTION("Different pool sizes should not match") {
    auto different_pool = std::make_shared<EntityMemoryPool>(CreateTestPool(3));
    EntityTransportVariant different_actual = different_pool;
    REQUIRE_THAT(different_actual, !EqualsEntityTransport(expected));
  }
}

TEST_CASE("EntityTransportEqualsMatcher - variant type mismatch",
          "[unit][EntityTransport][matcher]") {
  EntityMemoryPool pool = CreateTestPool(5);
  auto shared_pool = std::make_shared<EntityMemoryPool>(CreateTestPool(5));

  SECTION("monostate vs EntityMemoryPool should not match") {
    EntityTransportVariant expected = std::monostate{};
    EntityTransportVariant actual = pool;
    REQUIRE_THAT(actual, !EqualsEntityTransport(expected));
  }

  SECTION("EntityMemoryPool vs monostate should not match") {
    EntityTransportVariant expected = pool;
    EntityTransportVariant actual = std::monostate{};
    REQUIRE_THAT(actual, !EqualsEntityTransport(expected));
  }

  SECTION("shared_ptr vs value type should not match") {
    EntityTransportVariant expected = shared_pool;
    EntityTransportVariant actual = pool;
    REQUIRE_THAT(actual, !EqualsEntityTransport(expected));
  }

  SECTION("value type vs shared_ptr should not match") {
    EntityTransportVariant expected = pool;
    EntityTransportVariant actual = shared_pool;
    REQUIRE_THAT(actual, !EqualsEntityTransport(expected));
  }
}

TEST_CASE("EntityTransportEqualsMatcher - IEntityImporter case",
          "[unit][EntityTransport][matcher]") {
  // Mock IEntityImporter for testing
  class MockImporter : public IEntityImporter {
  public:
    std::expected<std::monostate, FailInfo>
    ImportEntities(EntityMemoryPool &emp) override {
      return std::monostate{};
    }
  };

  auto importer1 = std::make_unique<MockImporter>();
  auto importer2 = std::make_unique<MockImporter>();

  EntityTransportVariant expected = std::move(importer1);
  EntityTransportVariant actual = std::move(importer2);

  SECTION("IEntityImporter comparison should fail with clear message") {
    REQUIRE_THAT(actual, !EqualsEntityTransport(expected));
  }
}

TEST_CASE("EntityTransportEqualsMatcher describe method - success",
          "[unit][EntityTransport][matcher]") {
  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(5);

  EntityTransportVariant expected = pool1;
  EntityTransportVariant actual = pool2;

  auto matcher = EqualsEntityTransport(expected);
  matcher.match(actual);

  std::ostringstream oss;
  oss << conmat::Header(conmat::TestPassed() + "EntityTransport Match:", 3)
      << "\n";

  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("EntityTransportEqualsMatcher describe method - variant mismatch",
          "[unit][EntityTransport][matcher]") {
  EntityMemoryPool pool = CreateTestPool(5);
  EntityTransportVariant expected = std::monostate{};
  EntityTransportVariant actual = pool;

  auto matcher = EqualsEntityTransport(expected);
  matcher.match(actual);

  std::string description = matcher.describe();

  SECTION("Description should contain header") {
    REQUIRE(description.find("EntityTransport Mismatch") != std::string::npos);
  }

  SECTION("Description should contain variant type mismatch") {
    REQUIRE(description.find("Variant type mismatch") != std::string::npos);
  }
}

TEST_CASE("EntityTransportEqualsMatcher describe method - null pointer",
          "[unit][EntityTransport][matcher]") {
  auto pool = std::make_shared<EntityMemoryPool>(CreateTestPool(5));
  std::shared_ptr<EntityMemoryPool> null_ptr = nullptr;

  EntityTransportVariant expected = pool;
  EntityTransportVariant actual = null_ptr;

  auto matcher = EqualsEntityTransport(expected);
  matcher.match(actual);

  std::string description = matcher.describe();

  SECTION("Description should contain null pointer message") {
    REQUIRE(description.find("Null pointer in shared_ptr<EntityMemoryPool>") !=
            std::string::npos);
  }
}

TEST_CASE("EntityTransportEqualsMatcher describe method - IEntityImporter",
          "[unit][EntityTransport][matcher]") {
  class MockImporter : public IEntityImporter {
  public:
    std::expected<std::monostate, FailInfo>
    ImportEntities(EntityMemoryPool &emp) override {
      return std::monostate{};
    }
  };

  auto importer1 = std::make_unique<MockImporter>();
  auto importer2 = std::make_unique<MockImporter>();

  EntityTransportVariant expected = std::move(importer1);
  EntityTransportVariant actual = std::move(importer2);

  auto matcher = EqualsEntityTransport(expected);
  matcher.match(actual);

  std::string description = matcher.describe();

  SECTION("Description should contain IEntityImporter not supported message") {
    REQUIRE(description.find("IEntityImporter comparison not supported") !=
            std::string::npos);
  }
}

TEST_CASE("EntityTransportEqualsMatcher with TestContext",
          "[unit][EntityTransport][matcher]") {
  EntityMemoryPool pool1 = CreateTestPool(5);
  EntityMemoryPool pool2 = CreateTestPool(5);

  EntityTransportVariant expected = pool1;
  EntityTransportVariant actual = pool2;

  TestContext context{"my_test", "Test description", 2, 5};

  SECTION("Matcher with context should match") {
    REQUIRE_THAT(actual, EqualsEntityTransport(expected, context));
  }

  SECTION("Describe with context should include test metadata") {
    auto matcher = EqualsEntityTransport(expected, context);
    matcher.match(actual);
    std::string description = matcher.describe();

    REQUIRE(description.find("my_test") != std::string::npos);
    REQUIRE(description.find("Tick: 2/5") != std::string::npos);
  }

  SECTION("Describe on failure with context should include metadata") {
    EntityTransportVariant different_actual = std::monostate{};
    auto matcher = EqualsEntityTransport(expected, context);
    matcher.match(different_actual);
    std::string description = matcher.describe();

    REQUIRE(description.find("my_test") != std::string::npos);
    REQUIRE(description.find("Tick: 2/5") != std::string::npos);
    REQUIRE(description.find("Variant type mismatch") != std::string::npos);
  }
}
