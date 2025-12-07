/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the TestEngine class.
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
  const std::unordered_map<size_t, std::vector<SceneSnapshot>> &
  GetDataBank() const;
};
} // namespace steamrot::tests
