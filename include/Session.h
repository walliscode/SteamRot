#pragma once
#include "Tile.h"

#include <memory>
class Session {
public:
  Session();

  void SplitTile();

private:
  std::vector<std::shared_ptr<Tile>> m_tiles;
  std::shared_ptr<Tile> m_active_tile;
};
