#include "DisplayManager.h"
#include "Session.h"
#include "general_util.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include <fstream>
#include <iostream>

DisplayManager::DisplayManager() {

  // load json file for  configurations
  // create file path

  std::string file_name =
      std::string(RESOURCES_DIR) + "/jsons/display_config.json";
  // check file exists
  //
  if (!utils::fileExists(file_name)) {
    std::cout << "File does not exist: " << file_name << std::endl;
    return;
  }
  std::ifstream f(file_name);

  json config = json::parse(f);
  json display_manager_config = config["display_manager_config"];
  json tile_config = config["tile_config"];

  SetWindowConfig(display_manager_config);
  m_tile_config = tile_config;

  // set initial Sesssions
  m_sessions.fill(nullptr);
  // set the first session as active
  m_sessions[0] = std::make_shared<Session>(m_window);
  m_active_session = m_sessions[0];

  m_active_session->AddTile(m_tile_config, m_window);
};

void DisplayManager::SetWindowConfig(const json &config) {
  // cast json values to to the correct types, to show intent
  sf::Vector2u size{config["width"], config["height"]};
  m_window.setSize(size);

  const std::string title = config["title"];
  m_window.setTitle(title);

  unsigned int frame_rate_limit = config["framerate_limit"];
  m_window.setFramerateLimit(frame_rate_limit);

  m_background_color.r = config["background_color"]["r"];
  m_background_color.g = config["background_color"]["g"];
  m_background_color.b = config["background_color"]["b"];
};

sf::RenderWindow &DisplayManager::GetWindow() { return m_window; };

std::shared_ptr<Session> DisplayManager::GetActiveSession() {
  return m_active_session;
};

const json &DisplayManager::GetTileConfig() { return m_tile_config; };

void DisplayManager::Update() {
  // update all necessary window logic functions

  // cycle through generated actions
  for (auto action : m_action_waiting_room) {
    // std::cout << "Action: " << action << std::endl;

    if (action->m_name == "ACTION_ADD_TILE") {
      std::cout << "adding Tile" << std::endl;
      // add a tile to the active session

      m_active_session->AddTile(m_tile_config, m_window);
    }
  }
};

void DisplayManager::Cycle() {
  // get active Session render texture and draw tile borders to it
  sf::RenderTexture &tile_overlay = m_active_session->GetTileOverlay();

  // go through clear, draw, display cycle for sf::RenderTexture
  tile_overlay.clear(m_background_color);
  for (auto tile : m_active_session->GetTiles()) {
    // draw tile borders to the render texture
    for (auto border_straight : tile->GetBorderStraights()) {

      tile_overlay.draw(border_straight);
    }
    // for (auto border_corner : tile->GetBorderCorners()) {
    //   tile_overlay.draw(border_corner);
    // }
  }

  tile_overlay.display();

  // now the sf::RenderWindow cycle
  m_window.clear(m_background_color);

  // draw tile overlay to window
  sf::Sprite tile_overlay_sprite(tile_overlay.getTexture());
  m_window.draw(tile_overlay_sprite);

  m_window.display();
};

void DisplayManager::PopulateActions(
    const std::bitset<SteamRot::kUserInputCount> &user_input) {

  // Action class looks for registered actions and passes back vector of
  // action (strings)
  m_action_waiting_room = m_action.GenerateActions(user_input);
};
