/////////////////////////////////////////////////
/// @file
/// @brief GameEngine class declaration.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Engine.h"
#include "GameResources.h"
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
class GameEngine : public Engine {
private:
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

  /////////////////////////////////////////////////
  /// \brief Shutdown the game engine
  ///
  /////////////////////////////////////////////////
  void ShutDown();

  /////////////////////////////////////////////////
  /// @brief All subscribers registered to the GameEngine
  /////////////////////////////////////////////////
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

  /////////////////////////////////////////////////
  /// \brief Get the current loop number
  ///
  /////////////////////////////////////////////////
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

  /////////////////////////////////////////////////
  /// @brief Go through all subscriptions, if active call relevant Logic
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ProcessSubscriptions();

  /////////////////////////////////////////////////
  /// @brief Returns a constant reference to the RenderWindow
  /////////////////////////////////////////////////
  const sf::RenderWindow &GetWindow() const;

  /////////////////////////////////////////////////
  /// @brief Return a refence to the GameContext that lives on the GameEngine
  ///
  /// @return GameContext&
  /////////////////////////////////////////////////
  const GameContext &GetGameContext() const;

  /////////////////////////////////////////////////
  /// @brief Update the game resources
  ///
  /// @param game_resources to update
  /////////////////////////////////////////////////
  void UpdateGameResources(GameResources &game_resources);
};

} // namespace steamrot
