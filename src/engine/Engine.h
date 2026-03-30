/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the abstract Engine base class.
///
/// Engine provides a common foundation for both GameEngine and TestEngine,
/// ensuring consistent core management and tick execution order.
///
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EngineConfig.h"
#include "EngineResources.h"
#include "EngineSnapshot.h"
#include "EngineState.h"
#include "FailInfo.h"
#include "GameContext.h"
#include "SceneManager.h"
#include <expected>
#include <memory>
#include <variant>

namespace steamrot {

/////////////////////////////////////////////////
/// @class Engine
/// @brief Abstract base class for unified game/test execution.
///
/// Engine provides:
/// - Shared resource management (EngineResources, EngineConfig, EngineState)
/// - Common Run() loop structure
/// - Virtual hooks for configuration and tick execution
///
/// Both GameEngine and TestEngine derive from this base class,
/// ensuring consistent behavior between production and test environments.
///
/// @note Derived classes must implement ConfigureEngineStateFromData() to
/// provide their specific data source (files for GameEngine, injected config
/// for TestEngine).
/////////////////////////////////////////////////
class Engine {
protected:
  /////////////////////////////////////////////////
  /// @brief Engine-level global resources (window, event handler, asset
  /// manager, etc.)
  /////////////////////////////////////////////////
  EngineResources m_engine_resources;

  /////////////////////////////////////////////////
  /// @brief GameContext for accessing core objects
  /////////////////////////////////////////////////
  GameContext m_game_context;

  /////////////////////////////////////////////////
  /// @brief SceneManager instance for the Game
  /////////////////////////////////////////////////
  SceneManager m_scene_manager;

  /////////////////////////////////////////////////
  /// @brief Engine configuration (display settings, user preferences)
  /////////////////////////////////////////////////
  EngineConfig m_engine_config;

  /////////////////////////////////////////////////
  /// @brief Engine runtime state (subscriptions, flags, performance metrics)
  /////////////////////////////////////////////////
  EngineState m_engine_state;

  /////////////////////////////////////////////////
  /// @brief Virtual method capturing the game loop structure.
  /////////////////////////////////////////////////
  virtual void RunGameLoop() = 0;

  /////////////////////////////////////////////////
  /// @brief Execute a complete tick using the unified tick pipeline.
  ///
  /// Calls all tick phases in order:
  /// 1. OnTickBegin() - Pre-tick hook
  /// 2. TickEvents() - Event processing
  /// 3. TickEngineLogic() - Engine subscriptions
  /// 4. TickSceneManager() - Scene manager subscriptions + scene updates
  /// 5. TickRendering() - Display rendering
  /// 6. OnTickEnd() - Post-tick hook
  ///
  /// This method can be used for integration testing of the full tick cycle.
  /////////////////////////////////////////////////
  void ExecuteTick();

  /////////////////////////////////////////////////
  /// @brief Hook called at the beginning of each tick (before any processing).
  ///
  /// Override to add custom logic at tick start (logging, profiling, etc.).
  /// Base implementation does nothing.
  /////////////////////////////////////////////////
  virtual void OnTickBegin() {}

  /////////////////////////////////////////////////
  /// @brief Process SFML events and update event bus.
  ///
  /// This method handles window events, keyboard/mouse input, and updates
  /// the event bus with new events. Can be called individually for testing
  /// event processing in isolation.
  /////////////////////////////////////////////////
  virtual void TickEvents();

  /////////////////////////////////////////////////
  /// @brief Process engine-level subscriptions and logic.
  ///
  /// This method updates engine-level subscribers (e.g., quit game handler).
  /// Can be called individually for testing engine logic in isolation.
  /////////////////////////////////////////////////
  virtual void TickEngineLogic();

  /////////////////////////////////////////////////
  /// @brief Process scene manager subscriptions and logic.
  ///
  /// This method is the single entry point for all SceneManager work per tick:
  /// it processes scene manager subscriptions (e.g. scene-change events) and
  /// then updates all active scenes by calling their logic systems.
  /// Override in derived classes to customise scene update behaviour while
  /// keeping subscription processing intact.
  /////////////////////////////////////////////////
  virtual void TickSceneManager();

  /////////////////////////////////////////////////
  /// @brief Process rendering logic.
  ///
  /// GameEngine renders to the display via DisplayManager.
  /// TestEngine may skip rendering or render for validation only.
  /// Can be called individually for testing rendering in isolation.
  /////////////////////////////////////////////////
  virtual void TickRendering() = 0;

  /////////////////////////////////////////////////
  /// @brief Hook called at the end of each tick (after all processing).
  ///
  /// Override to add custom logic at tick end (data capture, metrics, etc.).
  /// Base implementation does nothing.
  /////////////////////////////////////////////////
  virtual void OnTickEnd() {}

  /////////////////////////////////////////////////
  /// @brief Go through all subscriptions, if active call relevant Logic
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo> ProcessSubscriptions() = 0;

  /////////////////////////////////////////////////
  /// @brief Capture the state of the Engine at the current tick.
  /////////////////////////////////////////////////
  std::expected<EngineSnapshot, FailInfo> CaptureEngineSnapshot();

public:
  /////////////////////////////////////////////////
  /// @brief Constructor initializing core objects and context.
  /////////////////////////////////////////////////
  explicit Engine();

  /////////////////////////////////////////////////
  /// @brief Virtual destructor for proper cleanup.
  /////////////////////////////////////////////////
  virtual ~Engine() = default;

  /////////////////////////////////////////////////
  /// @brief Start up the engine and configure core objects.
  ///
  /// This is virtual to allow derived classes to customize startup.
  /// Base implementation:
  /// 1. Loads EngineCoreData from engine_data.json
  /// 2. Configures GameCore (window, etc.)
  /// 3. Loads default user preferences from default.preferences.bin
  /// 4. Calls ConfigureEngineStateFromData() (virtual)
  ///
  /// GameEngine overrides to also load user-saved preferences.
  /// TestEngine uses base implementation (default preferences only).
  ///
  /// @return Success or failure information
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo> StartUp();

  /////////////////////////////////////////////////
  /// @brief A simple wrapper function to indicate running the game
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> RunGame();

  /////////////////////////////////////////////////
  /// @brief Get the engine configuration (includes user preferences).
  ///
  /// @return Const reference to engine configuration
  /////////////////////////////////////////////////
  const EngineConfig &GetEngineConfig() const { return m_engine_config; }

  std::vector<std::shared_ptr<Subscriber>> &GetSubscriptions();
};

} // namespace steamrot
