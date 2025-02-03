#include "DisplayManager.h"
#include "general_util.h"
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
  m_sessions[0] = std::make_shared<Session>();
  m_active_session = m_sessions[0];

  // print ou current wndow size
  std::cout << "Window size: " << m_window.getSize().x << "x"
            << m_window.getSize().y << std::endl;

  // add a tile to the active session
  sf::FloatRect viewport_ratio({0.0f, 0.0f}, {1.0f, 1.0f});
  sf::Vector2f window_size = static_cast<sf::Vector2f>(m_window.getSize());

  m_active_session->AddTile(m_tile_config, viewport_ratio, window_size);
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

void DisplayManager::Cycle() {
  m_window.clear(m_background_color);

  // get the active session, cycle through the tiles, set their view and draw
  // their borders
  auto active_session = GetActiveSession();
  auto tiles = active_session->GetTiles();

  for (auto &tile : tiles) {

    m_window.setView(tile->GetView());
    const auto &border_straights = tile->GetBorderStraights();
    for (const auto &border_straight : border_straights) {
      m_window.draw(border_straight);
    }
    const auto &border_corners = tile->GetBorderCorners();
    for (const auto &border_corner : border_corners) {
      m_window.draw(border_corner);
    }
  }
  m_window.display();
};
