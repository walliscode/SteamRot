/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for TestEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestEngine.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

TEST_CASE("TestEngine construction", "[unit][TestEngine]") {
  SECTION("Can be constructed with null config") {
    TestEngine engine(nullptr);
    REQUIRE(engine.GetTickLevel() == TickLevel::Custom);
    REQUIRE(engine.GetCurrentTick() == 0);
  }
}

TEST_CASE("TestEngine tick level switching", "[unit][TestEngine]") {
  TestEngine engine(nullptr);

  SECTION("Default tick level is Custom") {
    REQUIRE(engine.GetTickLevel() == TickLevel::Custom);
  }

  SECTION("UseFullScene switches to FullEngine level") {
    engine.UseFullScene(SceneType::SceneType_TITLE);
    REQUIRE(engine.GetTickLevel() == TickLevel::FullEngine);
  }

  SECTION("UseTickLevel can set all levels") {
    engine.UseTickLevel(TickLevel::FullEngine);
    REQUIRE(engine.GetTickLevel() == TickLevel::FullEngine);

    engine.UseTickLevel(TickLevel::SceneManager);
    REQUIRE(engine.GetTickLevel() == TickLevel::SceneManager);

    engine.UseTickLevel(TickLevel::SceneLogic);
    REQUIRE(engine.GetTickLevel() == TickLevel::SceneLogic);

    engine.UseTickLevel(TickLevel::Custom);
    REQUIRE(engine.GetTickLevel() == TickLevel::Custom);
  }

  SECTION("AddFunction keeps Custom level") {
    engine.AddFunction([](SceneContext &) {});
    REQUIRE(engine.GetTickLevel() == TickLevel::Custom);
  }
}

TEST_CASE("TestEngine SetTicks", "[unit][TestEngine]") {
  TestEngine engine(nullptr);

  SECTION("SetTicks returns self for chaining") {
    TestEngine &result = engine.SetTicks(5);
    REQUIRE(&result == &engine);
  }
}

TEST_CASE("TestEngine UseFullScene", "[unit][TestEngine]") {
  TestEngine engine(nullptr);

  SECTION("UseFullScene returns self for chaining") {
    TestEngine &result = engine.UseFullScene(SceneType::SceneType_TITLE);
    REQUIRE(&result == &engine);
  }
}

TEST_CASE("TestEngine UseTickLevel", "[unit][TestEngine]") {
  TestEngine engine(nullptr);

  SECTION("UseTickLevel returns self for chaining") {
    TestEngine &result = engine.UseTickLevel(TickLevel::SceneManager);
    REQUIRE(&result == &engine);
  }
}

TEST_CASE("TestEngine SetSceneType", "[unit][TestEngine]") {
  TestEngine engine(nullptr);

  SECTION("SetSceneType returns self for chaining") {
    TestEngine &result = engine.SetSceneType(SceneType::SceneType_CRAFTING);
    REQUIRE(&result == &engine);
  }
}

TEST_CASE("TestEngine AddFunction", "[unit][TestEngine]") {
  TestEngine engine(nullptr);

  SECTION("AddFunction returns self for chaining") {
    TestEngine &result =
        engine.AddFunction([](SceneContext &) {}, "TestFunction");
    REQUIRE(&result == &engine);
  }
}

TEST_CASE("TestEngine has same architecture as GameEngine",
          "[unit][TestEngine]") {
  TestEngine engine(nullptr);

  SECTION("Has SceneManager member") {
    // This test just verifies the member exists and is accessible
    SceneManager &sm = engine.GetSceneManager();
    (void)sm; // Suppress unused warning
    SUCCEED("SceneManager accessible");
  }

  SECTION("Const SceneManager accessor works") {
    const TestEngine &const_engine = engine;
    const SceneManager &sm = const_engine.GetSceneManager();
    (void)sm;
    SUCCEED("Const SceneManager accessible");
  }
}

} // namespace steamrot::tests
