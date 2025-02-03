#include "Session.h"
#include <memory>

Session::Session() {};

void Session::AddTile(const json &tile_config,
                      const sf::FloatRect &viewport_ratio,
                      const sf::Vector2f &window_size) {

  std::shared_ptr<Tile> tile =
      std::make_shared<Tile>(tile_config, viewport_ratio, window_size);
  m_tiles.push_back(tile);
  SetActiveTile(tile);
};

void Session::SplitTile() {};

std::vector<std::shared_ptr<Tile>> &Session::GetTiles() { return m_tiles; };
std::shared_ptr<Tile> Session::GetActiveTile() { return m_active_tile; };

void Session::SetActiveTile(std::shared_ptr<Tile> tile) {
  m_active_tile = tile;
  tile->SetBorderColourActive();
};
