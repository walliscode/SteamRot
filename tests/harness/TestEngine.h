/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the TestEngine class.
///
/// TestEngine provides a flexible testing framework that shares the
/// Engine base class with GameEngine for consistent resource management.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Engine.h"
#include "EntityManager.h"
#include "FlatbuffersConfigurator.h"
#include "ILogicStep.h"
#include "SceneContext.h"
#include "SceneResources.h"
#include "TestDataSource.h"
#include "test_data_generated.h"
#include <functional>
#include <memory>
#include <vector>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class TestEngine
/// @brief Test engine for data-driven testing.
///
/// TestEngine derives from Engine to share resource management with
/// GameEngine. It provides two execution modes:
///
/// **Custom Mode**: Mix and match Logic classes and free functions.
/// Useful for incremental testing and unit tests.
///
/// **Standard Mode**: Uses the same execution path as GameEngine.
/// Useful for integration tests that must match game behavior.
///
/// Example usage:
/// @code
/// TestEngine engine(config);
///
/// // Custom mode - mix and match
/// engine.AddLogic<UICollisionLogic>()
///       .AddFunction([](SceneContext& ctx) { /* validate */ });
///
/// // Or standard mode - identical to GameEngine
/// engine.UseFullScene(SceneType::SceneType_TITLE);
///
/// engine.Initialize();
/// engine.Run(5);  // Run 5 ticks
/// @endcode
/////////////////////////////////////////////////
class TestEngine : public Engine {
public:
  /////////////////////////////////////////////////
  /// @brief Execution mode for the test engine.
  /////////////////////////////////////////////////
  enum class Mode {
    Custom,  ///< Mix and match Logic classes and functions
    Standard ///< Use same execution path as GameEngine
  };

private:
  /////////////////////////////////////////////////
  /// @brief Test data configuration.
  /////////////////////////////////////////////////
  const TestDataConfig *m_test_config;

  /////////////////////////////////////////////////
  /// @brief Current execution mode.
  /////////////////////////////////////////////////
  Mode m_mode = Mode::Custom;

  /////////////////////////////////////////////////
  /// @brief Scene type for standard mode (FullScene).
  /////////////////////////////////////////////////
  SceneType m_scene_type = SceneType::SceneType_TEST;

  /////////////////////////////////////////////////
  /// @brief Number of ticks to run.
  /////////////////////////////////////////////////
  size_t m_target_ticks = 0;

  /////////////////////////////////////////////////
  /// @brief Current tick number.
  /////////////////////////////////////////////////
  size_t m_current_tick = 0;

  /////////////////////////////////////////////////
  /// @brief Custom execution steps.
  /////////////////////////////////////////////////
  std::vector<std::unique_ptr<ILogicStep>> m_custom_steps;

  /////////////////////////////////////////////////
  /// @brief Scene-level resources.
  /////////////////////////////////////////////////
  SceneResources m_scene_resources;

  /////////////////////////////////////////////////
  /// @brief Entity manager for managing entities.
  /////////////////////////////////////////////////
  EntityManager m_entity_manager;

  /////////////////////////////////////////////////
  /// @brief Scene context (created lazily).
  /////////////////////////////////////////////////
  std::unique_ptr<SceneContext> m_scene_context;

  /////////////////////////////////////////////////
  /// @brief Execute custom mode tick.
  /////////////////////////////////////////////////
  void ExecuteCustomTick();

  /////////////////////////////////////////////////
  /// @brief Execute standard mode tick.
  /////////////////////////////////////////////////
  void ExecuteStandardTick();

protected:
  /////////////////////////////////////////////////
  /// @brief Configure the engine from test data.
  ///
  /// @return Success or failure information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ConfigureFromData() override;

  /////////////////////////////////////////////////
  /// @brief Execute a single tick.
  /////////////////////////////////////////////////
  void ExecuteTick() override;

  /////////////////////////////////////////////////
  /// @brief Check if the engine should continue running.
  ///
  /// Returns true if current_tick < target_ticks.
  ///
  /// @return true if more ticks should be executed
  /////////////////////////////////////////////////
  bool ShouldContinueRunning() const override;

  /////////////////////////////////////////////////
  /// @brief Post-tick processing.
  /////////////////////////////////////////////////
  void OnPostTick() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor taking test data configuration.
  ///
  /// @param config Test data configuration (must remain valid)
  /////////////////////////////////////////////////
  explicit TestEngine(const TestDataConfig *config);

  /////////////////////////////////////////////////
  /// @brief Initialize the test engine.
  ///
  /// Must be called before Run(). Configures resources and entities.
  ///
  /// @return Success or failure information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> Initialize();

  /////////////////////////////////////////////////
  /// @brief Initialize with custom entity collection.
  ///
  /// Allows overriding the entity collection from the test config.
  ///
  /// @param entity_collection Entity collection to use
  /// @return Success or failure information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  Initialize(const EntityCollection *entity_collection);

  /////////////////////////////////////////////////
  /// @brief Add a Logic class to custom execution.
  ///
  /// @tparam TLogic Logic class type (must derive from Logic)
  /// @return Reference to this engine for chaining
  /////////////////////////////////////////////////
  template <typename TLogic> TestEngine &AddLogic();

  /////////////////////////////////////////////////
  /// @brief Add a free function to custom execution.
  ///
  /// @param func Function to execute
  /// @param name Optional name for debugging
  /// @return Reference to this engine for chaining
  /////////////////////////////////////////////////
  TestEngine &AddFunction(std::function<void(SceneContext &)> func,
                          const std::string &name = "FunctionStep");

  /////////////////////////////////////////////////
  /// @brief Set to standard mode with full scene execution.
  ///
  /// @param scene_type Scene type to emulate
  /// @return Reference to this engine for chaining
  /////////////////////////////////////////////////
  TestEngine &UseFullScene(SceneType scene_type);

  /////////////////////////////////////////////////
  /// @brief Set the number of ticks to run.
  ///
  /// @param ticks Number of ticks
  /// @return Reference to this engine for chaining
  /////////////////////////////////////////////////
  TestEngine &SetTicks(size_t ticks);

  /////////////////////////////////////////////////
  /// @brief Get the entity memory pool.
  ///
  /// @return Reference to the entity memory pool
  /////////////////////////////////////////////////
  EntityMemoryPool &GetEntityPool();

  /////////////////////////////////////////////////
  /// @brief Get a const reference to the entity memory pool.
  ///
  /// @return Const reference to the entity memory pool
  /////////////////////////////////////////////////
  const EntityMemoryPool &GetEntityPool() const;

  /////////////////////////////////////////////////
  /// @brief Get the entity manager.
  ///
  /// @return Reference to the entity manager
  /////////////////////////////////////////////////
  EntityManager &GetEntityManager();

  /////////////////////////////////////////////////
  /// @brief Get the scene context.
  ///
  /// Creates the scene context if not already created.
  ///
  /// @return Reference to the scene context
  /////////////////////////////////////////////////
  SceneContext &GetSceneContext();

  /////////////////////////////////////////////////
  /// @brief Get the current tick number.
  ///
  /// @return Current tick number
  /////////////////////////////////////////////////
  size_t GetCurrentTick() const;

  /////////////////////////////////////////////////
  /// @brief Get the current execution mode.
  ///
  /// @return Current mode
  /////////////////////////////////////////////////
  Mode GetMode() const;
};

/////////////////////////////////////////////////
// Template implementations
/////////////////////////////////////////////////

template <typename TLogic> TestEngine &TestEngine::AddLogic() {
  static_assert(std::is_base_of_v<Logic, TLogic>,
                "TLogic must derive from Logic");
  m_mode = Mode::Custom;
  m_custom_steps.push_back(
      std::make_unique<LogicClassStep<TLogic>>(GetSceneContext()));
  return *this;
}

} // namespace steamrot::tests
