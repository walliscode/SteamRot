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
#include "DisplayManager.h"
#include "Engine.h"
#include "EntityManager.h"
#include "FlatbuffersConfigurator.h"
#include "ILogicStep.h"
#include "SceneContext.h"
#include "SceneManager.h"
#include "SceneResources.h"
#include "TestDataSource.h"
#include "test_data_generated.h"
#include <functional>
#include <memory>
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
  /// @brief Scene type for scene-level operations.
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
  /// @brief SceneManager - SAME as GameEngine
  /////////////////////////////////////////////////
  SceneManager m_scene_manager;

  /////////////////////////////////////////////////
  /// @brief DisplayManager - SAME as GameEngine
  /////////////////////////////////////////////////
  DisplayManager m_display_manager;

  /////////////////////////////////////////////////
  /// @brief Scene-level resources (for custom mode).
  /////////////////////////////////////////////////
  SceneResources m_scene_resources;

  /////////////////////////////////////////////////
  /// @brief Entity manager (for custom mode).
  /////////////////////////////////////////////////
  EntityManager m_entity_manager;

  /////////////////////////////////////////////////
  /// @brief Scene context (created lazily for custom mode).
  /////////////////////////////////////////////////
  std::unique_ptr<SceneContext> m_scene_context;

  /////////////////////////////////////////////////
  /// @brief Execute full engine tick (mirrors GameEngine::UpdateSystems).
  /////////////////////////////////////////////////
  void TickFullEngine();

  /////////////////////////////////////////////////
  /// @brief Execute scene manager tick (mirrors SceneManager::UpdateSceneManager).
  /////////////////////////////////////////////////
  void TickSceneManagerLevel();

  /////////////////////////////////////////////////
  /// @brief Execute scene logic tick (sAction, sCollision, sRender).
  /////////////////////////////////////////////////
  void TickSceneLogicLevel();

  /////////////////////////////////////////////////
  /// @brief Execute custom mode tick.
  /////////////////////////////////////////////////
  void TickCustomLevel();

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
  /// @brief Set the tick execution level.
  ///
  /// Determines which level of the game architecture to execute:
  /// - FullEngine: Mirrors GameEngine::UpdateSystems (includes rendering)
  /// - SceneManager: Mirrors SceneManager::UpdateSceneManager
  /// - SceneLogic: Runs sAction, sCollision, sRender on current scene
  /// - Custom: Mix and match Logic classes and functions
  ///
  /// @param level Tick execution level
  /// @return Reference to this engine for chaining
  /////////////////////////////////////////////////
  TestEngine &UseTickLevel(TickLevel level);

  /////////////////////////////////////////////////
  /// @brief Set to full engine level with specific scene type.
  ///
  /// Convenience method combining UseTickLevel(FullEngine) with SetSceneType.
  ///
  /// @param scene_type Scene type to load
  /// @return Reference to this engine for chaining
  /////////////////////////////////////////////////
  TestEngine &UseFullScene(SceneType scene_type);

  /////////////////////////////////////////////////
  /// @brief Set the scene type for scene-level operations.
  ///
  /// @param scene_type Scene type to use
  /// @return Reference to this engine for chaining
  /////////////////////////////////////////////////
  TestEngine &SetSceneType(SceneType scene_type);

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
  /// @brief Get the current tick execution level.
  ///
  /// @return Current tick level
  /////////////////////////////////////////////////
  TickLevel GetTickLevel() const;

  /////////////////////////////////////////////////
  /// @brief Get the scene manager.
  ///
  /// @return Reference to the scene manager
  /////////////////////////////////////////////////
  SceneManager &GetSceneManager();

  /////////////////////////////////////////////////
  /// @brief Get a const reference to the scene manager.
  ///
  /// @return Const reference to the scene manager
  /////////////////////////////////////////////////
  const SceneManager &GetSceneManager() const;
};

/////////////////////////////////////////////////
// Template implementations
/////////////////////////////////////////////////

template <typename TLogic> TestEngine &TestEngine::AddLogic() {
  static_assert(std::is_base_of_v<Logic, TLogic>,
                "TLogic must derive from Logic");
  m_tick_level = TickLevel::Custom;
  m_custom_steps.push_back(
      std::make_unique<LogicClassStep<TLogic>>(GetSceneContext()));
  return *this;
}

} // namespace steamrot::tests
