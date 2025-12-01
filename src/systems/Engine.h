/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the abstract Engine base class.
///
/// Engine provides a common foundation for both GameEngine and TestEngine,
/// ensuring consistent resource management and tick execution order.
///
/// ## Data Requirements (Engine Level)
///
/// The Engine base class handles common startup configuration. Each derived
/// Engine provides its own data source via the virtual method pattern:
///
/// ### GameResourcesData (loaded in Engine::StartUp):
///   - window_width, window_height: Window dimensions
///   - window_title: Window title string
///   - framerate_limit: Target FPS
///   Source: Loaded from engine_data.json via FlatbuffersDataLoader
///
/// ### EngineData (provided by derived classes via ConfigureEngineStateFromData):
///   - subscriptions: Engine-level event subscriptions (e.g., quit game)
///   - scene_manager_data: SceneManager configuration
///   Source: Depends on Engine type:
///     - GameEngine: Loads from engine_data.json
///     - TestEngine: Uses injected TestDataConfig
///
/// ## Data Flow
/// ```
/// StartUp()
///   └─▶ ProvideGameResourcesData() [loads window config]
///   └─▶ ConfigureGameResources() [applies to m_game_resources]
///   └─▶ ConfigureEngineStateFromData() [virtual - derived class provides data]
///         ├─▶ GameEngine: Loads subscriptions + scene_manager from files
///         └─▶ TestEngine: Uses m_test_config->starting_engine_state()
/// ```
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
#include <memory>
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
///
/// @note Derived classes must implement ConfigureEngineStateFromData() to
/// provide their specific data source (files for GameEngine, injected config
/// for TestEngine).
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
  /// @brief All subscribers registered to the GameEngine
  /////////////////////////////////////////////////
  std::vector<std::shared_ptr<Subscriber>> m_subscriptions;

  /////////////////////////////////////////////////
  /// @brief Configure the engine from data sources.
  ///
  /// Called by StartUp() to load configuration data.
  /// Derived classes implement data loading specific to their context
  /// (game data files vs test data configs).
  ///
  /// @return Success or failure information
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEngineStateFromData() = 0;

  /////////////////////////////////////////////////
  /// @brief Execute a single tick of the game loop.
  ///
  /// Called by Run() for each iteration of the game loop.
  /// Derived classes implement tick logic specific to their context
  /// (real input/rendering vs simulated input/validation).
  /////////////////////////////////////////////////
  void ExecuteSystemsTick();

  /////////////////////////////////////////////////
  /// @brief Executre engine-level logic for the engine, such as updating
  /// ssubcribers, quitting conditions, etc.
  /////////////////////////////////////////////////
  void ExecuteEngineLevelLogic();

  /////////////////////////////////////////////////
  /// @brief Execute scene-level logic for the engine.
  ///
  /// For the GameEngine, this will be a compile time constant but tfor the test
  /// engine we want to be able to configure this.
  ///
  /////////////////////////////////////////////////
  virtual void ExecuteSceneLevelLogic() = 0;

  /////////////////////////////////////////////////
  /// @brief Calls on any DisplayManager tick execution.
  ///
  /// This may not be relevant, for example in a TestEngine there is no
  /// DisplayManager.
  /////////////////////////////////////////////////
  virtual void ExecuteDisplayManagerTick() = 0;

  /////////////////////////////////////////////////
  /// @brief Virtual method capturing the game loop structure.
  /////////////////////////////////////////////////
  virtual void RunGameLoop() = 0;

  /////////////////////////////////////////////////
  /// @brief Start up the engine and configure resources.
  ///
  /// @return Success or failure information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> StartUp();

  /////////////////////////////////////////////////
  /// @brief Go through all subscriptions, if active call relevant Logic
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo> ProcessSubscriptions() = 0;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor initializing resources and context.
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
  /// @brief Add the Subscriber to the subscriptions vector.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
      RegisterSubscriber(std::shared_ptr<Subscriber>);

  /////////////////////////////////////////////////
  /// @brief Configure subscribers from flatbuffers data
  ///
  /// @param subscriptions Flatbuffers vector of SubscriberData objects
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ConfigureSubscribersFromData(
      const ::flatbuffers::Vector<
          ::flatbuffers::Offset<steamrot::SubscriberData>> *subscriptions);
};

} // namespace steamrot
