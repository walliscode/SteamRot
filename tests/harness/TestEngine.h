/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the TestEngine class.
///
/// TestEngine provides a flexible testing framework that shares the
/// Engine base class with GameEngine for consistent resource management.
/// It uses the same architecture as GameEngine (SceneManager, DisplayManager)
/// to enable testing at different execution levels.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Engine.h"
#include "test_data_generated.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Execution level for TestEngine ticks.
///
/// Determines which level of the game architecture to execute:
/// - FullEngine: Complete GameEngine tick (UpdateSystems)
/// - SceneManager: SceneManager level tick
/// - SceneLogic: Individual scene logic vector
/// - Custom: Mix and match Logic classes and functions
/////////////////////////////////////////////////
enum class TickLevel {
  FullEngine,   ///< Mirrors GameEngine::UpdateSystems
  SceneManager, ///< Mirrors SceneManager::UpdateSceneManager
  SceneLogic,   ///< Runs sAction, sCollision, sRender on current scene
  Custom        ///< Mix and match Logic classes and functions
};

/////////////////////////////////////////////////
/// @class TestEngine
/// @brief Test engine for data-driven testing.
///
/// TestEngine derives from Engine to share resource management with
/// GameEngine. It uses the SAME architecture as GameEngine with
/// SceneManager and DisplayManager members, enabling testing at
/// different execution levels:
///
/// **TickLevel::FullEngine**: Identical to GameEngine::UpdateSystems
/// **TickLevel::SceneManager**: Identical to SceneManager::UpdateSceneManager
/// **TickLevel::SceneLogic**: Runs scene logic (sAction, sCollision, sRender)
/// **TickLevel::Custom**: Mix and match Logic classes and free functions
///
/// Example usage:
/// @code
/// TestEngine engine(config);
///
/// // Test at full engine level (identical to GameEngine)
/// engine.UseTickLevel(TickLevel::FullEngine);
/// engine.Initialize();
/// engine.Run(5);
///
/// // Test at scene manager level
/// engine.UseTickLevel(TickLevel::SceneManager);
/// engine.Initialize();
/// engine.Run(5);
///
/// // Custom mode - mix and match
/// engine.UseTickLevel(TickLevel::Custom);
/// engine.AddLogic<UICollisionLogic>()
///       .AddFunction([](SceneContext& ctx) { /* validate */ });
/// engine.Initialize();
/// engine.Run(5);
/// @endcode
/////////////////////////////////////////////////
class TestEngine : public Engine {
private:
  /////////////////////////////////////////////////
  /// @brief Test data configuration.
  /////////////////////////////////////////////////
  const TestDataConfig *m_test_config;

  /////////////////////////////////////////////////
  /// @brief Current tick execution level.
  /////////////////////////////////////////////////
  TickLevel m_tick_level = TickLevel::Custom;

  /////////////////////////////////////////////////
  /// @brief Number of ticks to run.
  /////////////////////////////////////////////////
  size_t m_target_ticks = 0;

  /////////////////////////////////////////////////
  /// @brief Current tick number.
  /////////////////////////////////////////////////
  size_t m_current_tick = 0;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor taking test data configuration.
  ///
  /// @param config Test data configuration (must remain valid)
  /////////////////////////////////////////////////
  explicit TestEngine(const TestDataConfig *config);
};
} // namespace steamrot::tests
