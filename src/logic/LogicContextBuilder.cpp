/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the LogicContextBuilder class.
/////////////////////////////////////////////////

#include "LogicContextBuilder.h"

namespace steamrot {

/////////////////////////////////////////////////
LogicContextBuilder &
LogicContextBuilder::SetSceneEntities(std::shared_ptr<EntityMemoryPool> entities) {
  m_scene_entities = entities;
  return *this;
}

/////////////////////////////////////////////////
LogicContextBuilder &LogicContextBuilder::SetArchetypes(
    std::shared_ptr<const std::unordered_map<ArchetypeID, Archetype>>
        archetypes) {
  m_archetypes = archetypes;
  return *this;
}

/////////////////////////////////////////////////
LogicContextBuilder &
LogicContextBuilder::SetSceneTexture(std::shared_ptr<sf::RenderTexture> texture) {
  m_scene_texture = texture;
  return *this;
}

/////////////////////////////////////////////////
LogicContextBuilder &
LogicContextBuilder::SetGameWindow(std::shared_ptr<sf::RenderWindow> window) {
  m_game_window = window;
  return *this;
}

/////////////////////////////////////////////////
LogicContextBuilder &
LogicContextBuilder::SetAssetManager(std::shared_ptr<const AssetManager> manager) {
  m_asset_manager = manager;
  return *this;
}

/////////////////////////////////////////////////
LogicContextBuilder &
LogicContextBuilder::SetEventHandler(std::shared_ptr<EventHandler> handler) {
  m_event_handler = handler;
  return *this;
}

/////////////////////////////////////////////////
LogicContextBuilder &
LogicContextBuilder::SetMousePosition(std::shared_ptr<const sf::Vector2i> mouse_pos) {
  m_mouse_position = mouse_pos;
  return *this;
}

/////////////////////////////////////////////////
std::expected<LogicContext, FailInfo> LogicContextBuilder::Build() const {
  // Validate all required fields are set
  if (!m_scene_entities) {
    return std::unexpected(FailInfo{FailMode::MissingRequiredField,
                                    "SceneEntities is required"});
  }

  if (!m_archetypes) {
    return std::unexpected(
        FailInfo{FailMode::MissingRequiredField, "Archetypes is required"});
  }

  if (!m_scene_texture) {
    return std::unexpected(
        FailInfo{FailMode::MissingRequiredField, "SceneTexture is required"});
  }

  if (!m_game_window) {
    return std::unexpected(
        FailInfo{FailMode::MissingRequiredField, "GameWindow is required"});
  }

  if (!m_asset_manager) {
    return std::unexpected(
        FailInfo{FailMode::MissingRequiredField, "AssetManager is required"});
  }

  if (!m_event_handler) {
    return std::unexpected(
        FailInfo{FailMode::MissingRequiredField, "EventHandler is required"});
  }

  if (!m_mouse_position) {
    return std::unexpected(
        FailInfo{FailMode::MissingRequiredField, "MousePosition is required"});
  }

  // Build LogicContext with references extracted from smart pointers
  return LogicContext{*m_scene_entities, *m_archetypes,    *m_scene_texture,
                      *m_game_window,    *m_asset_manager, *m_event_handler,
                      *m_mouse_position};
}

} // namespace steamrot
