#pragma once
#include "Tile.h"

#include <SFML/Graphics/Rect.hpp>
#include <memory>
class Session {
public:
  Session();

  void AddTile(const json &tile_config, const sf::FloatRect &viewport_ratio,
               const sf::Vector2f &window_size);

  std::vector<std::shared_ptr<Tile>> &GetTiles();
  std::shared_ptr<Tile> GetActiveTile();
  void SetActiveTile(std::shared_ptr<Tile> tile);

  void SplitTile();

private:
  std::vector<std::shared_ptr<Tile>> m_tiles;
  std::shared_ptr<Tile> m_active_tile;

  const json m_tile_config;
};
