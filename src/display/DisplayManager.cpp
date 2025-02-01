#include "DisplayManager.h"
#include "general_util.h"
#include <fstream>
#include <iostream>

DisplayManager::DisplayManager() {

  // load json file for DisplayManager configuration
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

  SetWindowConfig(config);
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

void DisplayManager::Cycle() {
  m_window.clear(m_background_color);
  // draw other things
  // m_window.draw();
  m_window.display();
};
