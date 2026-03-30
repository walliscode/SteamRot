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
#include "FailInfo.h"
#include "TestData.h"
#include <expected>
#include <variant>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class TestEngine
/// @brief Testing engine that uses injected configuration for game state.
/////////////////////////////////////////////////
class TestEngine : public Engine {
private:
  /////////////////////////////////////////////////
  /// @brief Constant reference to the test data for this instance of the
  /// engine.
  /////////////////////////////////////////////////
  const TestData &m_test_data;

  /////////////////////////////////////////////////
  /// @brief Number of ticks to run.
  /////////////////////////////////////////////////
  size_t m_target_ticks{1};

  /////////////////////////////////////////////////
  /// @brief Current tick number.
  /////////////////////////////////////////////////
  size_t m_current_tick{1};

  /////////////////////////////////////////////////
  /// @brief EngineSnapshot captured at a certain tick
  /////////////////////////////////////////////////
  std::map<size_t, EngineSnapshot> m_data_bank;

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
  /// @brief Override of TickSceneManager to support both production-mirroring
  /// and custom simulation modes.
  ///
  /// When SimulationData::use_default_logic is true, delegates entirely to
  /// Engine::TickSceneManager() — SceneManager runs its subscriptions and
  /// UpdateScenes, exactly mirroring production GameEngine behaviour.
  ///
  /// When use_default_logic is false, Engine::TickSceneManager() is skipped
  /// and only the explicit SimulationRunner steps are executed, preventing
  /// data from being passed through logic classes twice.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> TickSceneManager() override;

  /////////////////////////////////////////////////
  /// @brief Currently no need to run Engine level subscription logic
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ProcessSubscriptions() override {
    return std::monostate{};
  };

  /////////////////////////////////////////////////
  /// @brief Copy over the current engine snapshot and store it in the data bank
  ///
  /// This will convert the shared pointer to the EntityMemoryPool into a copy
  /// constructed EMP instance
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> StoreEngineSnapShot();

public:
  /////////////////////////////////////////////////
  /// @brief Constructor taking test data configuration.
  ///
  /// @param config Test data configuration (must remain valid)
  /////////////////////////////////////////////////
  explicit TestEngine(const TestData &test_data);

  /////////////////////////////////////////////////
  /// @brief TestEngine specific startup routine
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> StartUp() override;

  /////////////////////////////////////////////////
  /// @brief returns the target number of ticks to run
  ///
  /// @return The target number of ticks
  /////////////////////////////////////////////////
  const size_t &GetTargetTicks() const { return m_target_ticks; }

  /////////////////////////////////////////////////
  /// @brief returns the current tick number
  ///
  /// @return The current tick number
  /////////////////////////////////////////////////
  const size_t &GetCurrentTick() const { return m_current_tick; }

  /////////////////////////////////////////////////
  /// @brief Return a constant reference to the data bank for testing purposes.
  /////////////////////////////////////////////////
  const std::map<size_t, EngineSnapshot> &GetDataBank() const;

  /////////////////////////////////////////////////
  /// @brief Get the SceneManager for testing purposes.
  ///
  /// @return Const reference to the SceneManager
  /////////////////////////////////////////////////
  const SceneManager &GetSceneManager() const { return m_scene_manager; }

  /////////////////////////////////////////////////
  /// @brief Get the EngineResources for testing purposes.
  ///
  /// @return Const reference to the EngineResources
  /////////////////////////////////////////////////
  const EngineResources &GetEngineResources() const {
    return m_engine_resources;
  }
};
} // namespace steamrot::tests
