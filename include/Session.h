#pragma once
#include "Tile.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <memory>
class Session {
public:
  Session(sf::RenderWindow &window);

  void AddTile(const json &tile_config, sf::RenderWindow &window);

  std::vector<std::shared_ptr<Tile>> &GetTiles();
  std::shared_ptr<Tile> GetActiveTile();
  void SetActiveTile(std::shared_ptr<Tile> tile);
  void SetInactiveTiles(std::shared_ptr<Tile> tile);

  sf::RenderTexture &GetTileOverlay();

private:
  // each Session will have a sf::RenderTexture to draw to, implemnet listeners
  // to cause updating
  sf::RenderTexture m_TileOverlay;

  std::vector<std::shared_ptr<Tile>> m_tiles;
  std::shared_ptr<Tile> m_active_tile;

  const json m_tile_config;
};
