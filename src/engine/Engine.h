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
#include "EngineState.h"
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
  /// @brief Execute a single tick of the game loop.
  ///
  /// Called by Run() for each iteration of the game loop.
  /// Derived classes implement tick logic specific to their context
  /// (real input/rendering vs simulated input/validation).
  ///
  /// @note This method calls ExecuteTick() which uses the new Tick_() pipeline.
  /////////////////////////////////////////////////
  void ExecuteSystemsTick();

  /////////////////////////////////////////////////
  /// @brief Hook called at the beginning of each tick (before any processing).
  ///
  /// Override to add custom logic at tick start (logging, profiling, etc.).
  /// Base implementation does nothing.
  /////////////////////////////////////////////////
  virtual void OnTickBegin() {}

  /////////////////////////////////////////////////
  /// @brief Hook called at the end of each tick (after all processing).
  ///
  /// Override to add custom logic at tick end (data capture, metrics, etc.).
  /// Base implementation does nothing.
  /////////////////////////////////////////////////
  virtual void OnTickEnd() {}

  /////////////////////////////////////////////////
  /// @brief Virtual method capturing the game loop structure.
  /////////////////////////////////////////////////
  virtual void RunGameLoop() = 0;

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
  /// @brief Go through all subscriptions, if active call relevant Logic
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo> ProcessSubscriptions() = 0;

public:
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
  /// This method updates scene manager level logic such as scene change
  /// subscriptions. Can be called individually for testing scene manager
  /// logic in isolation.
  /////////////////////////////////////////////////
  virtual void TickSceneManager();

  /////////////////////////////////////////////////
  /// @brief Process scene-specific logic.
  ///
  /// Must be implemented by derived classes to define scene update behavior.
  /// GameEngine updates all active scenes, TestEngine may use simulation data.
  /// Can be called individually for testing scene logic in isolation.
  /////////////////////////////////////////////////
  virtual void TickSceneLogic() = 0;

  /////////////////////////////////////////////////
  /// @brief Process rendering logic.
  ///
  /// GameEngine renders to the display via DisplayManager.
  /// TestEngine may skip rendering or render for validation only.
  /// Can be called individually for testing rendering in isolation.
  /////////////////////////////////////////////////
  virtual void TickRendering() = 0;

  /////////////////////////////////////////////////
  /// @brief Execute a complete tick using the unified tick pipeline.
  ///
  /// Calls all tick phases in order:
  /// 1. OnTickBegin() - Pre-tick hook
  /// 2. TickEvents() - Event processing
  /// 3. TickEngineLogic() - Engine subscriptions
  /// 4. TickSceneManager() - Scene manager subscriptions
  /// 5. TickSceneLogic() - Scene updates
  /// 6. TickRendering() - Display rendering
  /// 7. OnTickEnd() - Post-tick hook
  ///
  /// This method can be used for integration testing of the full tick cycle.
  /////////////////////////////////////////////////
  void ExecuteTick();

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
  /// @brief A simple wrapper function to indicate running the game
  /////////////////////////////////////////////////
  void RunGame();

  /////////////////////////////////////////////////
  /// @brief Configure subscribers from flatbuffers data
  ///
  /// @param subscriptions Flatbuffers vector of SubscriberConfigFbs objects
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ConfigureSubscribersFromData(
      const ::flatbuffers::Vector<
          ::flatbuffers::Offset<steamrot::SubscriberConfigFbs>> *subscriptions);

  /////////////////////////////////////////////////
  /// @brief Get the engine configuration (includes user preferences).
  ///
  /// @return Const reference to engine configuration
  /////////////////////////////////////////////////
  const EngineConfig &GetEngineConfig() const { return m_engine_config; }

  std::vector<std::shared_ptr<Subscriber>> &GetSubscriptions();
};

} // namespace steamrot
