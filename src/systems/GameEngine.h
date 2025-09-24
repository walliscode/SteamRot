/////////////////////////////////////////////////
/// @file
/// @brief GameEngine class declaration.
/////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "AssetManager.h"
#include "DisplayManager.h"
#include "EventHandler.h"
#include "SceneManager.h"
#include "Subscriber.h"
#include "game_engine_generated.h"
#include <SFML/Graphics.hpp>
#include <expected>
#include <memory>
#include <variant>
#include <vector>

namespace steamrot {
/////////////////////////////////////////////////
/// @class GameEngine
/// @brief Methods for managing the game loop, user input and object
/// communication.
/////////////////////////////////////////////////
class GameEngine {
private:
  /////////////////////////////////////////////////
  /// @brief Variable to keep track of the current loop number.
  ///
  /// Increments by 1 at the start of each loop, the game will be maxed out at
  /// 60FPS so on a 32 bit system this will last 2.27 years. and on a 64 bit
  /// system this will last 9.75 trillion years. Probably long enough.
  /////////////////////////////////////////////////
  size_t m_loop_number = 1;

  /////////////////////////////////////////////////
  /// @brief Member: RenderWindow for the game engine
  /////////////////////////////////////////////////
  sf::RenderWindow m_window;

  /////////////////////////////////////////////////
  /// @brief EventHandler for managing events, this should be the only instance
  /////////////////////////////////////////////////
  EventHandler m_event_handler;

  /////////////////////////////////////////////////
  /// @brief AssetManager for managing game assets, this should be the only
  /// instance
  /////////////////////////////////////////////////
  AssetManager m_asset_manager;

  /////////////////////////////////////////////////
  /// @brief SceneManager for managing game scenes, this should be the only
  /// instance
  /////////////////////////////////////////////////
  SceneManager m_scene_manager;

  /////////////////////////////////////////////////
  /// @brief DisplayManager for rendering and display management
  /////////////////////////////////////////////////
  DisplayManager m_display_manager;

  /////////////////////////////////////////////////
  /// @brief Wrapper function to update any relevant systems
  /////////////////////////////////////////////////
  void UpdateSystems();

  /////////////////////////////////////////////////
  /// @brief Start up the game engine and load any resources
  /////////////////////////////////////////////////
  void StartUp();

  /////////////////////////////////////////////////
  /// @brief Run the game loop until exit condition is met
  /////////////////////////////////////////////////
  void RunGameLoop(size_t number_of_loops = 0, bool simulation = false);

  ////////////////////////////////////////////////////////////
  /// \brief Shutdown the game engine
  ///
  ////////////////////////////////////////////////////////////
  void ShutDown();

  sf::Vector2i m_mouse_position;

  void UpdateLocalMousePosition();

  std::vector<std::shared_ptr<Subscriber>> m_subscriptions;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for the GameEngine class
  ///
  /// @param env_type Environment type with which to initialize the engine
  /////////////////////////////////////////////////
  GameEngine(const EnvironmentType env_type = EnvironmentType::None);

  /////////////////////////////////////////////////
  /// @brief Container function to configure the GameEngine from flatbuffers
  /// data
  ///
  /// @param game_engine_data Flatbuffers GameEngineData object to configure
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureGameEngineFromData(const GameEngineData *game_engine_data);

  /////////////////////////////////////////////////
  /// @brief Runs the game loop, either indefinitely or for a set number of
  /// loops
  /////////////////////////////////////////////////
  void RunGame(size_t number_of_loops = 0, bool simulation = false);

  ////////////////////////////////////////////////////////////
  /// \brief Get the current loop number
  ///
  ////////////////////////////////////////////////////////////
  size_t GetLoopNumber() const;

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

  /////////////////////////////////////////////////
  /// @brief Returns all registered subscribers for inspection
  /////////////////////////////////////////////////
  const std::vector<std::shared_ptr<Subscriber>> &GetSubscriptions() const;
};

} // namespace steamrot
