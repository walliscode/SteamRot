/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GameContextBuilder class.
/////////////////////////////////////////////////

#include "GameContextBuilder.h"

namespace steamrot {

/////////////////////////////////////////////////
GameContextBuilder &
GameContextBuilder::SetWindow(std::shared_ptr<sf::RenderWindow> window) {
  m_window = window;
  return *this;
}

/////////////////////////////////////////////////
GameContextBuilder &
GameContextBuilder::SetEventHandler(std::shared_ptr<EventHandler> handler) {
  m_event_handler = handler;
  return *this;
}

/////////////////////////////////////////////////
GameContextBuilder &
GameContextBuilder::SetAssetManager(std::shared_ptr<AssetManager> manager) {
  m_asset_manager = manager;
  return *this;
}

/////////////////////////////////////////////////
GameContextBuilder &
GameContextBuilder::SetLoopNumber(std::shared_ptr<const size_t> loop_num) {
  m_loop_number = loop_num;
  return *this;
}

/////////////////////////////////////////////////
GameContextBuilder &
GameContextBuilder::SetEnvironmentType(EnvironmentType env_type) {
  m_env_type = env_type;
  return *this;
}

/////////////////////////////////////////////////
std::expected<GameContext, FailInfo> GameContextBuilder::Build() const {
  // Validate all required fields are set
  if (!m_window) {
    return std::unexpected(
        FailInfo{FailMode::MissingRequiredField, "Window is required"});
  }

  if (!m_event_handler) {
    return std::unexpected(FailInfo{FailMode::MissingRequiredField,
                                    "EventHandler is required"});
  }

  if (!m_asset_manager) {
    return std::unexpected(FailInfo{FailMode::MissingRequiredField,
                                    "AssetManager is required"});
  }

  if (!m_loop_number) {
    return std::unexpected(
        FailInfo{FailMode::MissingRequiredField, "LoopNumber is required"});
  }

  if (m_env_type == EnvironmentType::None) {
    return std::unexpected(FailInfo{FailMode::MissingRequiredField,
                                    "EnvironmentType is required"});
  }

  // Build GameContext with references extracted from smart pointers
  return GameContext{*m_window, *m_event_handler, *m_loop_number,
                     *m_asset_manager, m_env_type};
}

} // namespace steamrot
