////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "Session.h"
#include <iostream>
#include <memory>

namespace steamrot {
Session::Session() : m_tiles() {
  // Add the first tile to the session
  AddTile();
};
////////////////////////////////////////////////////////////
void Session::AddTile() {
  // create a new tile
  std::shared_ptr<Tile> new_tile = std::make_shared<Tile>();

  // and add it to the session's tile vector
  m_tiles.push_back(new_tile);

  // default the active tile to the last added tile
  m_active_tile = new_tile;

  // print whether the tiles are the same
  if (m_tiles.back() == new_tile) {
    std::cout << "Session: New tile added successfully." << std::endl;
  } else {
    std::cout << "Session: Failed to add new tile." << std::endl;
  }
};

////////////////////////////////////////////////////////////
void Session::RemoveTile(std::shared_ptr<Tile> tile) {
  auto it = std::find(m_tiles.begin(), m_tiles.end(), tile);
  if (it != m_tiles.end()) {
    m_tiles.erase(it);
  };
}

////////////////////////////////////////////////////////////
std::vector<std::shared_ptr<Tile>> &Session::GetTiles() { return m_tiles; };

////////////////////////////////////////////////////////////
std::shared_ptr<Tile> Session::GetActiveTile() { return m_active_tile; };

////////////////////////////////////////////////////////////
void Session::SetActiveTile(std::shared_ptr<Tile> tile) {

  m_active_tile = tile;
};

} // namespace steamrot
