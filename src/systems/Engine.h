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
#include "SceneManager.h"
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
  /// @brief SceneManager instance for the Game
  /////////////////////////////////////////////////
  SceneManager m_scene_manager;

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
  virtual std::expected<std::monostate, FailInfo> ConfigureEngineFromData() = 0;

  /////////////////////////////////////////////////
  /// @brief Execute a single tick of the game loop.
  ///
  /// Called by Run() for each iteration of the game loop.
  /// Derived classes implement tick logic specific to their context
  /// (real input/rendering vs simulated input/validation).
  /////////////////////////////////////////////////
  void ExecuteSystemsTick();

  void ExecuteEngineLevelLogic();

  virtual void ExecuteSceneLevelLogic() = 0;

  virtual void ExecuteDisplayManagerTick() = 0;

  virtual void RunGameLoop() = 0;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor initializing resources and context.
  ///
  /// @param env_type Environment type for resource configuration
  /////////////////////////////////////////////////
  explicit Engine();

  /////////////////////////////////////////////////
  /// @brief Virtual destructor for proper cleanup.
  /////////////////////////////////////////////////
  virtual ~Engine() = default;

  /////////////////////////////////////////////////
  /// @brief Start up the engine and configure resources.
  ///
  /// @return Success or failure information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> StartUp();

  /////////////////////////////////////////////////
  /// @brief A simple wrapper function to indicate running the game
  /////////////////////////////////////////////////
  void Run();
};

} // namespace steamrot
