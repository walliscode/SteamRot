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

  // takes in map of scene drawables
  void Update();
  void PopulateActions(const EventFlags &event_flags);

  // if drawable name matches tile linked_drawables, draw to that tile
  void DrawProvidedDrawables(
      std::map<std::string, std::vector<std::shared_ptr<sf::Drawable>>>
          &drawables);

  sf::RenderTexture &DrawTileOverlay();
  // cycle through drawing/rendering events
  void Cycle(std::map<std::string, std::vector<std::shared_ptr<sf::Drawable>>>
                 &drawables);

private:
  // add some defaults to the window otherwise it doens't behave itself
  // may have initialise DisplayManager with a json object so i can pass
  // constructors into the window
  sf::RenderWindow m_window{sf::VideoMode(steamrot::kWindowSize),
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
