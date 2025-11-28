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
    REQUIRE(engine.GetMode() == TestEngine::Mode::Custom);
    REQUIRE(engine.GetCurrentTick() == 0);
  }
}

TEST_CASE("TestEngine mode switching", "[unit][TestEngine]") {
  TestEngine engine(nullptr);

  SECTION("Default mode is Custom") {
    REQUIRE(engine.GetMode() == TestEngine::Mode::Custom);
  }

  SECTION("UseFullScene switches to Standard mode") {
    engine.UseFullScene(SceneType::SceneType_TITLE);
    REQUIRE(engine.GetMode() == TestEngine::Mode::Standard);
  }

  SECTION("AddFunction keeps Custom mode") {
    engine.AddFunction([](SceneContext &) {});
    REQUIRE(engine.GetMode() == TestEngine::Mode::Custom);
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

TEST_CASE("TestEngine AddFunction", "[unit][TestEngine]") {
  TestEngine engine(nullptr);

  SECTION("AddFunction returns self for chaining") {
    TestEngine &result =
        engine.AddFunction([](SceneContext &) {}, "TestFunction");
    REQUIRE(&result == &engine);
  }
}

} // namespace steamrot::tests
