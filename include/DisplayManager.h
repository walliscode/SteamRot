#pragma once
#include "ActionManager.h"
#include "Session.h"
#include "global_constants.h"
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

  void Update();
  void
  PopulateActions(const std::bitset<SteamRot::kUserInputCount> &user_input);
  void Cycle();

private:
  // add some defaults to the window otherwise it doens't behave itself
  // may have initialise DisplayManager with a json object so i can pass
  // constructors into the window
  sf::RenderWindow m_window{sf::VideoMode(SteamRot::kWindowSize),
                            "SFML window"};

  sf::Color m_background_color;

  std::array<std::shared_ptr<Session>, 5> m_sessions;
  std::shared_ptr<Session> m_active_session;

  // store Tile and Session config here as well, as there will only be one
  // DisplayManager
  json m_tile_config;

  // action related items
  ActionManager m_action{"display_manager"};
  std::vector<std::shared_ptr<Action>> m_action_waiting_room;
};
