/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the GameContextBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AssetManager.h"
#include "EventHandler.h"
#include "FailInfo.h"
#include "GameContext.h"
#include "PathProvider.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <expected>
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
/// @class GameContextBuilder
/// @brief Builder class for constructing GameContext instances.
///
/// Uses smart pointers internally for flexible construction,
/// then produces a GameContext with references.
/////////////////////////////////////////////////
class GameContextBuilder {
private:
  std::shared_ptr<sf::RenderWindow> m_window{nullptr};
  std::shared_ptr<EventHandler> m_event_handler{nullptr};
  std::shared_ptr<AssetManager> m_asset_manager{nullptr};
  std::shared_ptr<const size_t> m_loop_number{nullptr};
  EnvironmentType m_env_type{EnvironmentType::None};

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor for GameContextBuilder.
  /////////////////////////////////////////////////
  GameContextBuilder() = default;

  /////////////////////////////////////////////////
  /// @brief Set the window for the GameContext.
  ///
  /// @param window Shared pointer to the render window
  /// @return Reference to this builder for chaining
  /////////////////////////////////////////////////
  GameContextBuilder &SetWindow(std::shared_ptr<sf::RenderWindow> window);

  /////////////////////////////////////////////////
  /// @brief Set the event handler for the GameContext.
  ///
  /// @param handler Shared pointer to the event handler
  /// @return Reference to this builder for chaining
  /////////////////////////////////////////////////
  GameContextBuilder &SetEventHandler(std::shared_ptr<EventHandler> handler);

  /////////////////////////////////////////////////
  /// @brief Set the asset manager for the GameContext.
  ///
  /// @param manager Shared pointer to the asset manager
  /// @return Reference to this builder for chaining
  /////////////////////////////////////////////////
  GameContextBuilder &SetAssetManager(std::shared_ptr<AssetManager> manager);

  /////////////////////////////////////////////////
  /// @brief Set the loop number for the GameContext.
  ///
  /// @param loop_num Shared pointer to the loop number
  /// @return Reference to this builder for chaining
  /////////////////////////////////////////////////
  GameContextBuilder &SetLoopNumber(std::shared_ptr<const size_t> loop_num);

  /////////////////////////////////////////////////
  /// @brief Set the environment type for the GameContext.
  ///
  /// @param env_type Environment type enum value
  /// @return Reference to this builder for chaining
  /////////////////////////////////////////////////
  GameContextBuilder &SetEnvironmentType(EnvironmentType env_type);

  /////////////////////////////////////////////////
  /// @brief Build the GameContext with references from smart pointers.
  ///
  /// Validates that all required fields are set before building.
  ///
  /// @return Expected containing GameContext or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<GameContext, FailInfo> Build() const;
};

} // namespace steamrot
