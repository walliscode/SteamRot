/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the abstract Engine base class.
///
/// Engine provides a common foundation for both GameEngine and TestEngine,
/// ensuring consistent resource management and tick execution order.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GameContext.h"
#include "GameResources.h"
#include <cstddef>
#include <expected>
#include <variant>

namespace steamrot {

/////////////////////////////////////////////////
/// @class Engine
/// @brief Abstract base class for unified game/test execution.
///
/// Engine provides:
/// - Shared resource management (GameResources, GameContext)
/// - Common Run() loop structure
/// - Virtual hooks for configuration and tick execution
///
/// Both GameEngine and TestEngine derive from this base class,
/// ensuring consistent behavior between production and test environments.
/////////////////////////////////////////////////
class Engine {
protected:
  /////////////////////////////////////////////////
  /// @brief Game-level resources (window, event handler, asset manager, etc.)
  /////////////////////////////////////////////////
  GameResources m_game_resources;

  /////////////////////////////////////////////////
  /// @brief GameContext for accessing resources
  /////////////////////////////////////////////////
  GameContext m_game_context;

  /////////////////////////////////////////////////
  /// @brief Flag indicating if the engine should continue running
  /////////////////////////////////////////////////
  bool m_running = false;

  /////////////////////////////////////////////////
  /// @brief Configure the engine from data sources.
  ///
  /// Called by StartUp() to load configuration data.
  /// Derived classes implement data loading specific to their context
  /// (game data files vs test data configs).
  ///
  /// @return Success or failure information
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo> ConfigureFromData() = 0;

  /////////////////////////////////////////////////
  /// @brief Execute a single tick of the game loop.
  ///
  /// Called by Run() for each iteration of the game loop.
  /// Derived classes implement tick logic specific to their context
  /// (real input/rendering vs simulated input/validation).
  /////////////////////////////////////////////////
  virtual void ExecuteTick() = 0;

  /////////////////////////////////////////////////
  /// @brief Check if the engine should continue running.
  ///
  /// Default implementation checks if the window is open.
  /// TestEngine can override to run for a fixed number of ticks.
  ///
  /// @return true if the engine should continue running
  /////////////////////////////////////////////////
  virtual bool ShouldContinueRunning() const;

  /////////////////////////////////////////////////
  /// @brief Post-tick processing.
  ///
  /// Called after ExecuteTick() completes. Default increments loop number.
  /// Derived classes can add additional post-tick processing.
  /////////////////////////////////////////////////
  virtual void OnPostTick();

public:
  /////////////////////////////////////////////////
  /// @brief Constructor initializing resources and context.
  ///
  /// @param env_type Environment type for resource configuration
  /////////////////////////////////////////////////
  explicit Engine(EnvironmentType env_type = EnvironmentType::None);

  /////////////////////////////////////////////////
  /// @brief Virtual destructor for proper cleanup.
  /////////////////////////////////////////////////
  virtual ~Engine() = default;

  /////////////////////////////////////////////////
  /// @brief Start up the engine and configure resources.
  ///
  /// Calls ConfigureFromData() to load configuration.
  ///
  /// @return Success or failure information
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo> StartUp();

  /////////////////////////////////////////////////
  /// @brief Run the game loop.
  ///
  /// Runs for the specified number of ticks, or indefinitely if num_ticks is 0.
  /// Each iteration calls ExecuteTick() and OnPostTick().
  ///
  /// @param num_ticks Number of ticks to run (0 = run until stopped)
  /////////////////////////////////////////////////
  void Run(size_t num_ticks = 0);

  /////////////////////////////////////////////////
  /// @brief Stop the engine.
  ///
  /// Sets the running flag to false, causing Run() to exit.
  /////////////////////////////////////////////////
  void Stop();

  /////////////////////////////////////////////////
  /// @brief Get the current loop number.
  ///
  /// @return Current loop/tick number
  /////////////////////////////////////////////////
  size_t GetLoopNumber() const;

  /////////////////////////////////////////////////
  /// @brief Get a reference to the game resources.
  ///
  /// @return Reference to GameResources
  /////////////////////////////////////////////////
  GameResources &GetGameResources();

  /////////////////////////////////////////////////
  /// @brief Get a const reference to the game resources.
  ///
  /// @return Const reference to GameResources
  /////////////////////////////////////////////////
  const GameResources &GetGameResources() const;

  /////////////////////////////////////////////////
  /// @brief Get a reference to the game context.
  ///
  /// @return Reference to GameContext
  /////////////////////////////////////////////////
  GameContext &GetGameContext();

  /////////////////////////////////////////////////
  /// @brief Get a const reference to the game context.
  ///
  /// @return Const reference to GameContext
  /////////////////////////////////////////////////
  const GameContext &GetGameContext() const;
};

} // namespace steamrot
