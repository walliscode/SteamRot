/////////////////////////////////////////////////
/// @file
/// @brief GameEngine class declaration.
///
/// ## Data Requirements (GameEngine)
///
/// GameEngine loads its configuration from default FlatBuffers files.
/// The data loading happens in two phases:
///
/// ### Phase 1: Engine::StartUp() (inherited)
///   - GameResourcesData: Window size, title, framerate
///   - Source: engine_data.json → game_resources
///
/// ### Phase 2: ConfigureEngineStateFromData() (GameEngine override)
///   - EngineData.subscriptions: Engine-level event handlers (quit game, etc.)
///   - EngineData.scene_manager_data: SceneManager subscriptions
///   - UserPreferences: User-level settings (audio, display, accessibility)
///   - Source: engine_data.json, default.preferences.bin
///
/// ## Data Flow
/// ```
/// main.cpp
///   └─▶ GameEngine() [default constructor]
///   └─▶ RunGame()
///         └─▶ StartUp() [loads GameResourcesData, calls
///         ConfigureEngineStateFromData]
///               └─▶ ConfigureEngineStateFromData() [loads EngineData + UserPreferences]
///                     └─▶ ConfigureSubscribersFromData()
///                     └─▶ SceneManager::ConfigureSceneManagerFromData()
///                     └─▶ LoadUserPreferences()
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
#include "IUserPreferencesProvider.h"

namespace steamrot {
/////////////////////////////////////////////////
/// @class GameEngine
/// @brief Production game engine that loads configuration from default files.
///
/// GameEngine is the standard game execution environment:
/// - Loads EngineData from engine_data.json
/// - Loads UserPreferences from default.preferences.bin (and user overrides)
/// - Creates and manages the game window via DisplayManager
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
  /// @brief User preferences (loaded from file)
  /////////////////////////////////////////////////
  UserPreferences m_user_preferences;

  /////////////////////////////////////////////////
  /// @brief Execute scene-level logic for the GameEngine
  ///
  /// For the GameEngine, we want this logic to be fixed in the code and not
  /// configurable unlike the TestEngine.
  /////////////////////////////////////////////////
  void ExecuteSceneLevelLogic() override;

  /////////////////////////////////////////////////
  /// @brief Call DisplayManager tick execution
  /////////////////////////////////////////////////
  void ExecuteDisplayManagerTick() override;

  /////////////////////////////////////////////////
  /// @brief For the GameEngine, run the typical SFML game loop
  /////////////////////////////////////////////////
  void RunGameLoop() override;

  /////////////////////////////////////////////////
  /// @brief Execute logic for any active subscriptions at the GameEngine level
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ProcessSubscriptions() override;

  /////////////////////////////////////////////////
  /// @brief Configure engine state from EngineData loaded from files.
  ///
  /// Loads EngineData from engine_data.json and configures:
  /// - Engine-level subscriptions (e.g., quit game handler)
  /// - SceneManager subscriptions (e.g., scene change handler)
  /// - User preferences from default.preferences.bin
  ///
  /// @return Success or failure information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureEngineStateFromData() override;

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor for production use.
  ///
  /// GameEngine loads all configuration from default files during StartUp().
  /////////////////////////////////////////////////
  GameEngine();

  /////////////////////////////////////////////////
  /// @brief Get the current user preferences.
  ///
  /// @return Const reference to user preferences
  /////////////////////////////////////////////////
  const UserPreferences &GetUserPreferences() const { return m_user_preferences; }
};

} // namespace steamrot
