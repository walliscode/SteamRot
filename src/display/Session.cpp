#include "Session.h"

Session::Session() {
  m_tiles.push_back(std::make_shared<Tile>());
  m_active_tile = m_tiles[0];
};

void Session::SplitTile() {};
