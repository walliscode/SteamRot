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
#include "SceneInfo.h"
#include "simulation_generated.h"
#include "test_data_generated.h"
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

class TestEngine : public Engine {
private:
  /////////////////////////////////////////////////
  /// @brief Test data configuration.
  /////////////////////////////////////////////////
  const TestDataConfig *m_test_config;

  ////////////////////////////////////////////////
  /// @brief Simulation data extracted from test configuration.
  /////////////////////////////////////////////////
  const SimulationData *m_simulation_data;

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
  /// @brief Stores the tick number and data at that point
  /////////////////////////////////////////////////
  std::unordered_map<size_t, std::vector<SceneInfo>> m_data_bank;

  /////////////////////////////////////////////////
  /// @brief Currently, this will be left blank for the TestEngine
  /////////////////////////////////////////////////
  void ExecuteDisplayManagerTick() override {};

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
  /// @brief uses simulation data to put data through functions
  /////////////////////////////////////////////////
  void ExecuteSceneLevelLogic() override;

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
  const std::unordered_map<size_t, std::vector<SceneInfo>> &GetDataBank() const;
};
} // namespace steamrot::tests
