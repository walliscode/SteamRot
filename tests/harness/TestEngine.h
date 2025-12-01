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
#include "SceneContext.h"
#include "simulation_generated.h"
#include "test_data_generated.h"
#include <functional>
#include <string>
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
/// @brief Custom function type for TestEngine
/////////////////////////////////////////////////
using TestFunction = std::function<void(SceneContext &)>;

/////////////////////////////////////////////////
/// @brief Named custom function with description
/////////////////////////////////////////////////
struct NamedTestFunction {
  TestFunction function;
  std::string name;
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
  /// @brief Current tick number (1-based, starts at 0 before first tick).
  /////////////////////////////////////////////////
  size_t m_current_tick{0};

  /////////////////////////////////////////////////
  /// @brief Scene type for the test.
  /////////////////////////////////////////////////
  SceneType m_scene_type{SceneType::SceneType_TEST};

  /////////////////////////////////////////////////
  /// @brief Custom functions to execute during each tick.
  /////////////////////////////////////////////////
  std::vector<NamedTestFunction> m_custom_functions;

  /////////////////////////////////////////////////
  /// @brief Stores the tick number and data at that point
  /////////////////////////////////////////////////
  std::unordered_map<size_t, std::vector<SceneData>> m_data_bank;

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
  const std::unordered_map<size_t, std::vector<SceneData>> &GetDataBank() const;

  /////////////////////////////////////////////////
  /// @brief Get current tick level
  /////////////////////////////////////////////////
  TickLevel GetTickLevel() const { return m_tick_level; }

  /////////////////////////////////////////////////
  /// @brief Get current tick number
  /////////////////////////////////////////////////
  size_t GetCurrentTick() const { return m_current_tick; }

  /////////////////////////////////////////////////
  /// @brief Set the number of ticks to run
  ///
  /// @param num_ticks Number of ticks
  /// @return Reference to this for chaining
  /////////////////////////////////////////////////
  TestEngine &SetTicks(size_t num_ticks);

  /////////////////////////////////////////////////
  /// @brief Configure to run a full scene execution
  ///
  /// Sets tick level to FullEngine and configures the scene type.
  ///
  /// @param scene_type Scene type to use
  /// @return Reference to this for chaining
  /////////////////////////////////////////////////
  TestEngine &UseFullScene(SceneType scene_type);

  /////////////////////////////////////////////////
  /// @brief Set the tick execution level
  ///
  /// @param level Execution level
  /// @return Reference to this for chaining
  /////////////////////////////////////////////////
  TestEngine &UseTickLevel(TickLevel level);

  /////////////////////////////////////////////////
  /// @brief Set the scene type
  ///
  /// @param scene_type Scene type to use
  /// @return Reference to this for chaining
  /////////////////////////////////////////////////
  TestEngine &SetSceneType(SceneType scene_type);

  /////////////////////////////////////////////////
  /// @brief Add a custom function to execute during ticks
  ///
  /// @param func Function to execute
  /// @param name Optional name for debugging
  /// @return Reference to this for chaining
  /////////////////////////////////////////////////
  TestEngine &AddFunction(TestFunction func, const std::string &name = "");

  /////////////////////////////////////////////////
  /// @brief Access the SceneManager
  /////////////////////////////////////////////////
  SceneManager &GetSceneManager() { return m_scene_manager; }
  const SceneManager &GetSceneManager() const { return m_scene_manager; }

  /////////////////////////////////////////////////
  /// @brief Run the test engine
  ///
  /// This method starts up the engine and runs for the configured
  /// number of ticks, populating the data bank at each tick.
  /////////////////////////////////////////////////
  void Run();
};
} // namespace steamrot::tests
