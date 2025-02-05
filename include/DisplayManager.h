#pragma once
#include "Action.h"
#include "Session.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
class DisplayManager {

public:
  DisplayManager();
  void SetWindowConfig(const json &config);

  sf::RenderWindow &GetWindow();
  std::shared_ptr<Session> GetActiveSession();
  const json &GetTileConfig();
  void Cycle();

private:
  // add some defaults to the window otherwise it doens't behave itself
  // may have initialise DisplayManager with a json object so i can pass
  // constructors into the window
  sf::RenderWindow m_window{sf::VideoMode({1280, 900}), "SFML window"};
  sf::Color m_background_color;

  std::array<std::shared_ptr<Session>, 5> m_sessions;
  std::shared_ptr<Session> m_active_session;

  // store Tile and Session config here as well, as there will only be one
  // DisplayManager
  json m_tile_config;

  // action related items
  Action m_action{"display_manager"};
};
