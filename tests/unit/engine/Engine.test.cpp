/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for Engine class unified tick pipeline.
///
/// Tests the new Tick_() nomenclature methods introduced in
/// ENGINE_ARCHITECTURE_IMPROVEMENTS.md Proposal 1.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Engine.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

namespace {

/////////////////////////////////////////////////
/// @brief Concrete test implementation of Engine for testing
///
/// Provides minimal implementation to test base Engine class functionality.
/////////////////////////////////////////////////
class TestableEngine : public steamrot::Engine {
private:
  bool m_on_tick_begin_called{false};
  bool m_on_tick_end_called{false};
  bool m_tick_events_called{false};
  bool m_tick_engine_logic_called{false};
  bool m_tick_scene_manager_called{false};
  bool m_tick_scene_logic_called{false};
  bool m_tick_rendering_called{false};
  int m_call_order{0};
  int m_on_tick_begin_order{-1};
  int m_tick_events_order{-1};
  int m_tick_engine_logic_order{-1};
  int m_tick_scene_manager_order{-1};
  int m_tick_scene_logic_order{-1};
  int m_tick_rendering_order{-1};
  int m_on_tick_end_order{-1};

  std::expected<std::monostate, steamrot::FailInfo>
  ConfigureEngineStateFromData() override {
    return std::monostate{};
  }

  std::expected<std::monostate, steamrot::FailInfo>
  ProcessSubscriptions() override {
    return std::monostate{};
  }

  void RunGameLoop() override {
    // Minimal implementation for testing
  }

protected:
  void OnTickBegin() override {
    m_on_tick_begin_called = true;
    m_on_tick_begin_order = m_call_order++;
  }

  void OnTickEnd() override {
    m_on_tick_end_called = true;
    m_on_tick_end_order = m_call_order++;
  }

public:
  /////////////////////////////////////////////////
  /// @brief Constructor - properly initialize base Engine
  /////////////////////////////////////////////////
  TestableEngine() : steamrot::Engine() {}

  void TickEvents() override {
    steamrot::Engine::TickEvents();
    m_tick_events_called = true;
    m_tick_events_order = m_call_order++;
  }

  void TickEngineLogic() override {
    steamrot::Engine::TickEngineLogic();
    m_tick_engine_logic_called = true;
    m_tick_engine_logic_order = m_call_order++;
  }

  void TickSceneManager() override {
    steamrot::Engine::TickSceneManager();
    m_tick_scene_manager_called = true;
    m_tick_scene_manager_order = m_call_order++;
  }

  void TickSceneLogic() override {
    m_tick_scene_logic_called = true;
    m_tick_scene_logic_order = m_call_order++;
  }

  void TickRendering() override {
    m_tick_rendering_called = true;
    m_tick_rendering_order = m_call_order++;
  }

  // Accessors for testing
  bool WasOnTickBeginCalled() const { return m_on_tick_begin_called; }
  bool WasOnTickEndCalled() const { return m_on_tick_end_called; }
  bool WasTickEventsCalled() const { return m_tick_events_called; }
  bool WasTickEngineLogicCalled() const { return m_tick_engine_logic_called; }
  bool WasTickSceneManagerCalled() const {
    return m_tick_scene_manager_called;
  }
  bool WasTickSceneLogicCalled() const { return m_tick_scene_logic_called; }
  bool WasTickRenderingCalled() const { return m_tick_rendering_called; }

  int GetOnTickBeginOrder() const { return m_on_tick_begin_order; }
  int GetTickEventsOrder() const { return m_tick_events_order; }
  int GetTickEngineLogicOrder() const { return m_tick_engine_logic_order; }
  int GetTickSceneManagerOrder() const { return m_tick_scene_manager_order; }
  int GetTickSceneLogicOrder() const { return m_tick_scene_logic_order; }
  int GetTickRenderingOrder() const { return m_tick_rendering_order; }
  int GetOnTickEndOrder() const { return m_on_tick_end_order; }

  void ResetCallTracking() {
    m_on_tick_begin_called = false;
    m_on_tick_end_called = false;
    m_tick_events_called = false;
    m_tick_engine_logic_called = false;
    m_tick_scene_manager_called = false;
    m_tick_scene_logic_called = false;
    m_tick_rendering_called = false;
    m_call_order = 0;
    m_on_tick_begin_order = -1;
    m_tick_events_order = -1;
    m_tick_engine_logic_order = -1;
    m_tick_scene_manager_order = -1;
    m_tick_scene_logic_order = -1;
    m_tick_rendering_order = -1;
    m_on_tick_end_order = -1;
  }
};

} // anonymous namespace

TEST_CASE("Engine ExecuteTick calls all tick phases in correct order",
          "[unit][Engine]") {
  TestableEngine engine;

  engine.ExecuteTick();

  // Verify all methods were called
  REQUIRE(engine.WasOnTickBeginCalled());
  REQUIRE(engine.WasTickEventsCalled());
  REQUIRE(engine.WasTickEngineLogicCalled());
  REQUIRE(engine.WasTickSceneManagerCalled());
  REQUIRE(engine.WasTickSceneLogicCalled());
  REQUIRE(engine.WasTickRenderingCalled());
  REQUIRE(engine.WasOnTickEndCalled());

  // Verify call order
  REQUIRE(engine.GetOnTickBeginOrder() == 0);
  REQUIRE(engine.GetTickEventsOrder() == 1);
  REQUIRE(engine.GetTickEngineLogicOrder() == 2);
  REQUIRE(engine.GetTickSceneManagerOrder() == 3);
  REQUIRE(engine.GetTickSceneLogicOrder() == 4);
  REQUIRE(engine.GetTickRenderingOrder() == 5);
  REQUIRE(engine.GetOnTickEndOrder() == 6);
}

TEST_CASE("Engine TickEvents can be called independently", "[unit][Engine]") {
  TestableEngine engine;

  engine.TickEvents();

  // Only TickEvents should be called
  REQUIRE(engine.WasTickEventsCalled());
  REQUIRE_FALSE(engine.WasOnTickBeginCalled());
  REQUIRE_FALSE(engine.WasTickEngineLogicCalled());
  REQUIRE_FALSE(engine.WasTickSceneManagerCalled());
  REQUIRE_FALSE(engine.WasTickSceneLogicCalled());
  REQUIRE_FALSE(engine.WasTickRenderingCalled());
  REQUIRE_FALSE(engine.WasOnTickEndCalled());
}

TEST_CASE("Engine TickEngineLogic can be called independently",
          "[unit][Engine]") {
  TestableEngine engine;

  engine.TickEngineLogic();

  // Only TickEngineLogic should be called
  REQUIRE(engine.WasTickEngineLogicCalled());
  REQUIRE_FALSE(engine.WasOnTickBeginCalled());
  REQUIRE_FALSE(engine.WasTickEventsCalled());
  REQUIRE_FALSE(engine.WasTickSceneManagerCalled());
  REQUIRE_FALSE(engine.WasTickSceneLogicCalled());
  REQUIRE_FALSE(engine.WasTickRenderingCalled());
  REQUIRE_FALSE(engine.WasOnTickEndCalled());
}

TEST_CASE("Engine TickSceneManager can be called independently",
          "[unit][Engine]") {
  TestableEngine engine;

  engine.TickSceneManager();

  // Only TickSceneManager should be called
  REQUIRE(engine.WasTickSceneManagerCalled());
  REQUIRE_FALSE(engine.WasOnTickBeginCalled());
  REQUIRE_FALSE(engine.WasTickEventsCalled());
  REQUIRE_FALSE(engine.WasTickEngineLogicCalled());
  REQUIRE_FALSE(engine.WasTickSceneLogicCalled());
  REQUIRE_FALSE(engine.WasTickRenderingCalled());
  REQUIRE_FALSE(engine.WasOnTickEndCalled());
}

TEST_CASE("Engine TickSceneLogic can be called independently",
          "[unit][Engine]") {
  TestableEngine engine;

  engine.TickSceneLogic();

  // Only TickSceneLogic should be called
  REQUIRE(engine.WasTickSceneLogicCalled());
  REQUIRE_FALSE(engine.WasOnTickBeginCalled());
  REQUIRE_FALSE(engine.WasTickEventsCalled());
  REQUIRE_FALSE(engine.WasTickEngineLogicCalled());
  REQUIRE_FALSE(engine.WasTickSceneManagerCalled());
  REQUIRE_FALSE(engine.WasTickRenderingCalled());
  REQUIRE_FALSE(engine.WasOnTickEndCalled());
}

TEST_CASE("Engine TickRendering can be called independently",
          "[unit][Engine]") {
  TestableEngine engine;

  engine.TickRendering();

  // Only TickRendering should be called
  REQUIRE(engine.WasTickRenderingCalled());
  REQUIRE_FALSE(engine.WasOnTickBeginCalled());
  REQUIRE_FALSE(engine.WasTickEventsCalled());
  REQUIRE_FALSE(engine.WasTickEngineLogicCalled());
  REQUIRE_FALSE(engine.WasTickSceneManagerCalled());
  REQUIRE_FALSE(engine.WasTickSceneLogicCalled());
  REQUIRE_FALSE(engine.WasOnTickEndCalled());
}

TEST_CASE("Engine tick methods can be called in custom order",
          "[unit][Engine]") {
  TestableEngine engine;

  // Call only specific phases in a custom order
  engine.TickSceneManager();
  engine.TickSceneLogic();

  // Verify only the called methods were executed
  REQUIRE(engine.WasTickSceneManagerCalled());
  REQUIRE(engine.WasTickSceneLogicCalled());
  REQUIRE_FALSE(engine.WasTickEventsCalled());
  REQUIRE_FALSE(engine.WasTickEngineLogicCalled());
  REQUIRE_FALSE(engine.WasTickRenderingCalled());
}

TEST_CASE("Engine tick methods can be called multiple times", "[unit][Engine]") {
  TestableEngine engine;

  // Call TickSceneLogic multiple times
  engine.TickSceneLogic();
  REQUIRE(engine.WasTickSceneLogicCalled());

  engine.ResetCallTracking();
  REQUIRE_FALSE(engine.WasTickSceneLogicCalled());

  engine.TickSceneLogic();
  REQUIRE(engine.WasTickSceneLogicCalled());
}

TEST_CASE("Engine OnTickBegin and OnTickEnd hooks are optional",
          "[unit][Engine]") {
  // TestableEngine overrides the hooks, but they should be optional
  // This test verifies the hooks are called when overridden
  TestableEngine engine;

  engine.ExecuteTick();

  REQUIRE(engine.WasOnTickBeginCalled());
  REQUIRE(engine.WasOnTickEndCalled());
}

TEST_CASE("Engine ExecuteSystemsTick uses ExecuteTick internally",
          "[unit][Engine]") {
  TestableEngine engine;

  // ExecuteSystemsTick should call ExecuteTick
  engine.ExecuteSystemsTick();

  // All tick phases should have been called
  REQUIRE(engine.WasOnTickBeginCalled());
  REQUIRE(engine.WasTickEventsCalled());
  REQUIRE(engine.WasTickEngineLogicCalled());
  REQUIRE(engine.WasTickSceneManagerCalled());
  REQUIRE(engine.WasTickSceneLogicCalled());
  REQUIRE(engine.WasTickRenderingCalled());
  REQUIRE(engine.WasOnTickEndCalled());
}
