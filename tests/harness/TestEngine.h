/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the TestEngine class.
///
/// TestEngine provides a flexible testing framework that shares the
/// Engine base class with GameEngine for consistent resource management.
/// It uses the same architecture as GameEngine (SceneManager, DisplayManager)
/// to enable testing at different execution levels.
///
/// ## Data Requirements (TestEngine)
///
/// TestEngine receives its game state configuration via constructor injection,
/// but loads default user preferences from files like GameEngine.
///
/// ### UserPreferences (loaded via Engine::StartUp):
///   - Display settings (window size, fullscreen, vsync, framerate)
///   - Audio settings (volume levels, mute state)
///   - Accessibility settings (UI scale, preferred font)
///   - Source: Loaded from default.preferences.bin (same as GameEngine)
///
/// ### TestDataConfig (injected via constructor):
///   - starting_engine_state: EngineData containing:
///     - subscriptions: Engine-level event subscriptions
///     - scene_manager_data: SceneManager configuration
///   - simulation_data: SimulationData for tick execution
///   - num_ticks: Number of ticks to run
///   - metadata: Test identification
///   Source: Loaded from test_data.json files and parsed by test harness
///
/// ### EngineCoreData (loaded via Engine::StartUp):
///   - window_width, window_height, framerate_limit
///   - Source: Still loaded from engine_data.json (shared window config)
///
/// ## Data Flow
/// ```
/// test_file.cpp
///   └─▶ Load test_data.json → TestDataConfig
///   └─▶ TestEngine(config) [stores pointer to injected config]
///   └─▶ RunGame()
///         └─▶ Engine::StartUp() [loads EngineCoreData, UserPreferences, calls
///         ConfigureEngineStateFromData]
///               └─▶ ConfigureEngineStateFromData() [uses m_test_config]
///                     └─▶
///                     m_test_config->starting_engine_state()->subscriptions()
///                     └─▶
///                     m_test_config->starting_engine_state()->scene_manager_data()
///         └─▶ RunGameLoop() [executes m_target_ticks iterations]
/// ```
///
/// @note TestEngine uses injected configuration (TestDataConfig*) for game
/// state but loads default user preferences from files. This ensures tests have
/// consistent preference settings.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "Engine.h"
#include "SceneSnapshot.h"
#include "simulation_generated.h"
#include "test_data_generated.h"
#include <expected>
#include <unordered_map>
#include <vector>

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
/// @brief Testing engine that uses injected configuration for game state.
///
/// TestEngine receives game state configuration via constructor injection
/// (TestDataConfig*), but loads default user preferences from files like
/// GameEngine. This enables:
/// - Custom test configurations for game state
/// - Deterministic test scenarios
/// - Simulation-based testing with tick snapshots
/// - Consistent user preferences between GameEngine and TestEngine
///
/// Both GameEngine and TestEngine share the same Engine::StartUp() flow for:
/// - GameCore (window config)
/// - UserPreferences (from default.preferences.bin)
///
/// They differ in how they obtain EngineData (subscriptions, scene manager):
/// - GameEngine: Loads from engine_data.json, calls LoadTitleScene()
/// - TestEngine: Uses m_test_config->starting_engine_state()
/////////////////////////////////////////////////
class TestEngine : public Engine {
private:
  /////////////////////////////////////////////////
  /// @brief Test data configuration.
  /////////////////////////////////////////////////
  const TestDataConfig *m_test_config;

  ////////////////////////////////////////////////
  /// @brief Simulation data extracted from test configuration.
  /////////////////////////////////////////////////
  const SimulationData *m_simulation_data{nullptr};

  /////////////////////////////////////////////////
  /// @brief Current tick execution level.
  /////////////////////////////////////////////////
  TickLevel m_tick_level = TickLevel::Custom;

  /////////////////////////////////////////////////
  /// @brief Number of ticks to run.
  /////////////////////////////////////////////////
  size_t m_target_ticks{};

  /////////////////////////////////////////////////
  /// @brief Current tick number.
  /////////////////////////////////////////////////
  size_t m_current_tick{1};

  /////////////////////////////////////////////////
  /// @brief Stores the tick number and scene snapshots at that point
  /////////////////////////////////////////////////
  std::unordered_map<size_t, std::vector<SceneSnapshot>> m_data_bank;

  /////////////////////////////////////////////////
  /// @brief No rendering for TestEngine (new Tick_() pipeline method)
  ///
  /// TestEngine doesn't render to display, but scenes still render
  /// to their texture for validation.
  /////////////////////////////////////////////////
  void TickRendering() override {};

  /////////////////////////////////////////////////
  /// @brief For the TestEngine this will execute 1 tick then export data before
  /// proceeding onto the next tick
  /////////////////////////////////////////////////
  void RunGameLoop() override;

  /////////////////////////////////////////////////
  /// @brief Takes a snapshot of the current scenes and adds to the data bank
  /////////////////////////////////////////////////
  void AddToDataBank(size_t tick);

  /////////////////////////////////////////////////
  /// @brief Process scene-specific logic (new Tick_() pipeline method)
  ///
  /// For TestEngine, uses simulation data if available, otherwise
  /// falls back to normal scene updates.
  /////////////////////////////////////////////////
  void TickSceneLogic() override;

  /////////////////////////////////////////////////
  /// @brief Currently no need to run Engine level subscription logic
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ProcessSubscriptions() override {
    return std::monostate{};
  };

  std::expected<std::monostate, FailInfo>
  ConfigureEngineStateFromData() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor taking test data configuration.
  ///
  /// @param config Test data configuration (must remain valid)
  /////////////////////////////////////////////////
  explicit TestEngine(const TestDataConfig *config);

  /////////////////////////////////////////////////
  /// @brief Returns data bank for inspection and testing
  /////////////////////////////////////////////////
  const std::unordered_map<size_t, std::vector<SceneSnapshot>> &GetDataBank() const;
};
} // namespace steamrot::tests
