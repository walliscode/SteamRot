/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the LogicContextBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ArchetypeManager.h"
#include "AssetManager.h"
#include "EventHandler.h"
#include "FailInfo.h"
#include "LogicContext.h"
#include "containers.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <expected>
#include <memory>
#include <unordered_map>

namespace steamrot {

/////////////////////////////////////////////////
/// @class LogicContextBuilder
/// @brief Builder class for constructing LogicContext instances.
///
/// Uses smart pointers internally for flexible construction,
/// then produces a LogicContext with references.
/////////////////////////////////////////////////
class LogicContextBuilder {
private:
  std::shared_ptr<EntityMemoryPool> m_scene_entities{nullptr};
  std::shared_ptr<const std::unordered_map<ArchetypeID, Archetype>>
      m_archetypes{nullptr};
  std::shared_ptr<sf::RenderTexture> m_scene_texture{nullptr};
  std::shared_ptr<sf::RenderWindow> m_game_window{nullptr};
  std::shared_ptr<const AssetManager> m_asset_manager{nullptr};
  std::shared_ptr<EventHandler> m_event_handler{nullptr};
  std::shared_ptr<const sf::Vector2i> m_mouse_position{nullptr};

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor for LogicContextBuilder.
  /////////////////////////////////////////////////
  LogicContextBuilder() = default;

  /////////////////////////////////////////////////
  /// @brief Set the scene entities for the LogicContext.
  ///
  /// @param entities Shared pointer to the entity memory pool
  /// @return Reference to this builder for chaining
  /////////////////////////////////////////////////
  LogicContextBuilder &
  SetSceneEntities(std::shared_ptr<EntityMemoryPool> entities);

  /////////////////////////////////////////////////
  /// @brief Set the archetypes for the LogicContext.
  ///
  /// @param archetypes Shared pointer to the archetype map
  /// @return Reference to this builder for chaining
  /////////////////////////////////////////////////
  LogicContextBuilder &SetArchetypes(
      std::shared_ptr<const std::unordered_map<ArchetypeID, Archetype>>
          archetypes);

  /////////////////////////////////////////////////
  /// @brief Set the scene texture for the LogicContext.
  ///
  /// @param texture Shared pointer to the render texture
  /// @return Reference to this builder for chaining
  /////////////////////////////////////////////////
  LogicContextBuilder &
  SetSceneTexture(std::shared_ptr<sf::RenderTexture> texture);

  /////////////////////////////////////////////////
  /// @brief Set the game window for the LogicContext.
  ///
  /// @param window Shared pointer to the render window
  /// @return Reference to this builder for chaining
  /////////////////////////////////////////////////
  LogicContextBuilder &
  SetGameWindow(std::shared_ptr<sf::RenderWindow> window);

  /////////////////////////////////////////////////
  /// @brief Set the asset manager for the LogicContext.
  ///
  /// @param manager Shared pointer to the asset manager
  /// @return Reference to this builder for chaining
  /////////////////////////////////////////////////
  LogicContextBuilder &
  SetAssetManager(std::shared_ptr<const AssetManager> manager);

  /////////////////////////////////////////////////
  /// @brief Set the event handler for the LogicContext.
  ///
  /// @param handler Shared pointer to the event handler
  /// @return Reference to this builder for chaining
  /////////////////////////////////////////////////
  LogicContextBuilder &
  SetEventHandler(std::shared_ptr<EventHandler> handler);

  /////////////////////////////////////////////////
  /// @brief Set the mouse position for the LogicContext.
  ///
  /// @param mouse_pos Shared pointer to the mouse position
  /// @return Reference to this builder for chaining
  /////////////////////////////////////////////////
  LogicContextBuilder &
  SetMousePosition(std::shared_ptr<const sf::Vector2i> mouse_pos);

  /////////////////////////////////////////////////
  /// @brief Build the LogicContext with references from smart pointers.
  ///
  /// Validates that all required fields are set before building.
  ///
  /// @return Expected containing LogicContext or FailInfo on error
  /////////////////////////////////////////////////
  std::expected<LogicContext, FailInfo> Build() const;
};

} // namespace steamrot
