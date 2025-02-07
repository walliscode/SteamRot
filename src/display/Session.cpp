#include "Session.h"
#include <SFML/Graphics/Rect.hpp>
#include <memory>

Session::Session() {};

void Session::AddTile(const json &tile_config,
                      const sf::Vector2f &window_size) {

  // check if there are any tiles
  bool is_first_tile = m_tiles.empty();

  if (is_first_tile) {
    // if there are no tiles, create a new one and pass in a fresh viewport
    sf::FloatRect viewport_ratio = sf::FloatRect({0.f, 0.f}, {1.f, 1.f});
    std::shared_ptr<Tile> tile =
        std::make_shared<Tile>(tile_config, viewport_ratio, window_size);

    m_tiles.push_back(tile);
    SetActiveTile(tile);
    return;

  } else {

    // we are going to split based off of the current active tile
    std::shared_ptr<Tile> active_tile = GetActiveTile();

    // get the active tile viewport ratio
    sf::FloatRect active_tile_viewport = active_tile->GetView().getViewport();

    // get the width and height of current active tile viewport
    const float active_tile_width = active_tile->GetView().getSize().x;
    const float active_tile_height = active_tile->GetView().getSize().y;

    // split the view port into two, the direction of split depends on whether
    // the width or height is larger
    // split direction is always to the right or down
    //
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
    active_tile->SetViewProperties(new_viewport_one, window_size);

    // create the new active tile
    std::shared_ptr<Tile> new_tile =
        std::make_shared<Tile>(tile_config, new_viewport_two, window_size);
    m_tiles.push_back(new_tile);
    SetActiveTile(new_tile);
    SetInactiveTiles(new_tile);
  };
};

void Session::SplitTile() {};

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
