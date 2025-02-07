#include "Session.h"
#include <SFML/Graphics/Rect.hpp>
#include <iostream>
#include <memory>

Session::Session(sf::RenderWindow &window) {

  // set the size of the RenderTexture to the size of the window
  m_TileOverlay = sf::RenderTexture({window.getSize().x, window.getSize().y});
  std::cout << "Session created" << std::endl;
  std::cout << "TileOverlay size: " << m_TileOverlay.getSize().x << "x"
            << m_TileOverlay.getSize().y << std::endl;

  std::cout << "Window size: " << window.getSize().x << "x"
            << window.getSize().y << std::endl;
};

void Session::AddTile(const json &tile_config, sf::RenderWindow &window) {

  // check if there are any tiles
  bool is_first_tile = m_tiles.empty();

  if (is_first_tile) {
    // if there are no tiles, create a new one and pass in a fresh viewport
    sf::FloatRect viewport_ratio = sf::FloatRect({0.f, 0.f}, {1.f, 1.f});
    std::shared_ptr<Tile> tile =
        std::make_shared<Tile>(tile_config, viewport_ratio, window);

    m_tiles.push_back(tile);
    SetActiveTile(tile);
    return;

  } else {

    // we are going to split based off of the current active tile
    std::shared_ptr<Tile> active_tile = GetActiveTile();

    // get the active tile viewport ratio
    sf::FloatRect active_tile_viewport = active_tile->GetView().getViewport();

    // get the width and height of current active tile viewport
    const float active_tile_width =
        active_tile_viewport.size.x * window.getSize().x;
    const float active_tile_height =
        active_tile_viewport.size.y * window.getSize().y;

    // split the view port into two, the direction of split depends on:w
    // whether the width or height is larger split direction is always to the
    // right or down

    sf::FloatRect new_viewport_one;
    sf::FloatRect new_viewport_two;
    if (active_tile_width >= active_tile_height) {

      new_viewport_one = sf::FloatRect(
          {active_tile_viewport.position},
          {active_tile_viewport.size.x / 2, active_tile_viewport.size.y});

      new_viewport_two = sf::FloatRect(
          {active_tile_viewport.position.x + active_tile_viewport.size.x / 2,
           active_tile_viewport.position.y},
          {active_tile_viewport.size.x / 2, active_tile_viewport.size.y});
    } else {
      new_viewport_one = sf::FloatRect(
          {active_tile_viewport.position},
          {active_tile_viewport.size.x, active_tile_viewport.size.y / 2});
      new_viewport_two = sf::FloatRect(
          {active_tile_viewport.position.x,
           active_tile_viewport.position.y + active_tile_viewport.size.y / 2},
          {active_tile_viewport.size.x, active_tile_viewport.size.y / 2});
    }
    // set current tile properties to new viewport one
    active_tile->SetViewPort(new_viewport_one);
    active_tile->SetBorder();

    // create the new active tile
    std::shared_ptr<Tile> new_tile =
        std::make_shared<Tile>(tile_config, new_viewport_two, window);
    m_tiles.push_back(new_tile);
    SetActiveTile(new_tile);
    SetInactiveTiles(new_tile);
  }
};

sf::RenderTexture &Session::GetTileOverlay() { return m_TileOverlay; };

std::vector<std::shared_ptr<Tile>> &Session::GetTiles() { return m_tiles; };
std::shared_ptr<Tile> Session::GetActiveTile() { return m_active_tile; };

void Session::SetActiveTile(std::shared_ptr<Tile> tile) {
  // set newly made Tile as active
  m_active_tile = tile;
  tile->SetBorderColourActive();
};

void Session::SetInactiveTiles(std::shared_ptr<Tile> tile) {
  for (auto &t : m_tiles) {
    if (t != tile) {
      t->SetBorderColourInactive();
    }
  }
};
