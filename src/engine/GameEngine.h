/////////////////////////////////////////////////
/// @file
/// @brief GameEngine class declaration.
///
/// ## Data Requirements (GameEngine)
///
/// GameEngine loads its configuration from default FlatBuffers files.
/// The data loading happens in two phases:
///
/// ### Phase 1: Engine::StartUp() (base implementation)
///   - EngineCoreData: Window size, title, framerate
///   - UserPreferences: Default preferences from default.preferences.bin
///   - Source: engine_data.json → engine_core
///
/// ### Phase 2: GameEngine::StartUp() (override)
///   - Loads saved user preferences if they exist (overrides defaults)
///   - Calls base StartUp() for common initialization
///   - Calls SceneManager::LoadTitleScene() to start the game
///
/// ### Phase 3: ConfigureEngineStateFromData() (GameEngine override)
///   - EngineData.subscriptions: Engine-level event handlers (quit game, etc.)
///   - EngineData.scene_manager_data: SceneManager subscriptions
///   - Source: engine_data.json
///
/// ## Data Flow
/// ```
/// main.cpp
///   └─▶ GameEngine() [default constructor]
///   └─▶ RunGame()
///         └─▶ StartUp() [GameEngine override]
///               └─▶ Engine::StartUp() [loads defaults + calls
///               ConfigureEngineStateFromData] └─▶ LoadSavedUserPreferences()
///               [overrides defaults if file exists] └─▶
///               SceneManager::LoadTitleScene() [starts the game]
///         └─▶ RunGameLoop()
/// ```
///
/// @note GameEngine uses file-based configuration. For injected/mock data,
/// use TestEngine instead.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DisplayManager.h"
#include "Engine.h"
#include "FailInfo.h"

namespace steamrot {
/////////////////////////////////////////////////
/// @class GameEngine
/// @brief Production game engine that loads configuration from default files.
///
/// GameEngine is the standard game execution environment:
/// - Loads EngineData from engine_data.json
/// - Loads default UserPreferences from default.preferences.bin (via Engine)
/// - Optionally loads saved user preferences from user_preferences.bin
/// - Creates and manages the game window via DisplayManager
/// - Loads title scene on startup
/// - Runs the standard SFML game loop
///
/// For testing, use TestEngine which accepts injected TestDataConfig.
/////////////////////////////////////////////////
class GameEngine : public Engine {

private:
  /////////////////////////////////////////////////
  /// @brief DisplayManager for rendering and display management
  /////////////////////////////////////////////////
  DisplayManager m_display_manager;

  /////////////////////////////////////////////////
  /// @brief Process scene-specific logic (new Tick_() pipeline method)
  ///
  /// For GameEngine, updates all active scenes via SceneManager.
  /////////////////////////////////////////////////
  void TickSceneLogic() override;

  /////////////////////////////////////////////////
  /// @brief Process rendering logic (new Tick_() pipeline method)
  ///
  /// For GameEngine, renders to display via DisplayManager.
  /////////////////////////////////////////////////
  void TickRendering() override;

  /////////////////////////////////////////////////
  /// @brief For the GameEngine, run the typical SFML game loop
  /////////////////////////////////////////////////
  void RunGameLoop() override;

  /////////////////////////////////////////////////
  /// @brief Execute logic for any active subscriptions at the GameEngine level
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ProcessSubscriptions() override;

  /////////////////////////////////////////////////
  /// @brief Start up the GameEngine.
  ///
  /// Overrides Engine::StartUp() to:
  /// 1. Call base StartUp() (loads defaults + configures engine)
  /// 2. Load saved user preferences if they exist (overrides defaults)
  /// 3. Call SceneManager::LoadTitleScene() to start the game
  ///
  /// @return Success or failure information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> StartUp() override;

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor for production use.
  ///
  /// GameEngine loads all configuration from default files during StartUp().
  /////////////////////////////////////////////////
  GameEngine();
};

} // namespace steamrot
