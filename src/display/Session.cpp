#include "Session.h"
#include "global_constants.h"
#include <SFML/Graphics/Rect.hpp>
#include <array>
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

void Session::RemoveTile() {
  // if there is only one tile return early
  if (m_tiles.size() == 1) {
    std::cout << "Cannot remove last tile" << std::endl;
    return;
  };

  // decide direction of deleteion base on the active tile
  std::shared_ptr<Tile> active_tile = GetActiveTile();
  sf::FloatRect active_tile_viewport = active_tile->GetView().getViewport();

  // get the width and height of current active tile viewport
  const float active_tile_width =
      active_tile_viewport.size.x * SteamRot::kWindowSize.x;

  const float active_tile_height =
      active_tile_viewport.size.y * SteamRot::kWindowSize.y;

  // for all of these, once we've identified direction of neighoour, we need to
  // check there is only 1 direct neighbour in that direction, otherwise we
  // return if height is greater than width then we delete left or right
  if (active_tile_height > active_tile_width) {
    // use the fact that we are splitting the viewport in half each time
    // if the position.x/size.x is even then we are what about when i have
    // splitting branches that use the same code? on the left, so expand the
    // right. If the position.x/size.x is odd then we are on the right, so
    // expand the left
    if ((int(active_tile_viewport.position.x / active_tile_viewport.size.x) %
         2) == 0) {

      // check for number of neighbours on the right
      int right_neighbours = 0;

      for (auto &tile : m_tiles) {
        sf::FloatRect tile_viewport = tile->GetView().getViewport();

        // for the right neighbour, max x coordinate of the active tile must be
        // equal to the neighbour min x coordinate
        bool x_matches{active_tile_viewport.position.x +
                           active_tile_viewport.size.x ==
                       tile_viewport.position.x};

        // for the y coordinate, the neighbour position y cooridnate must be
        // equal or greater than the active tile min y coordinate and less
        // than the active tile max y coordinate
        bool y_matches{
            tile_viewport.position.y >= active_tile_viewport.position.y &&
            tile_viewport.position.y <
                active_tile_viewport.position.y + active_tile_viewport.size.y};

        if (x_matches && y_matches) {
          right_neighbours++;
        }
      }

      // return early if there are too many neighbours
      if (right_neighbours > 1) {
        std::cout << "Cannot remove tile, too many RIGHT neighbours"
                  << std::endl;
        return;
      }

      // if there is only one neighbour, expand the right
      for (auto &tile : m_tiles) {

        sf::FloatRect tile_viewport = tile->GetView().getViewport();

        // for the right neighbour, max x coordinate of the active tile must be
        // equal to the neighbour min x coordinate
        bool x_matches{active_tile_viewport.position.x +
                           active_tile_viewport.size.x ==
                       tile_viewport.position.x};

        // for the y coordinate, the neighbour position y cooridnate must be
        // equal or greater than the active tile min y coordinate and less
        // than the active tile max y coordinate
        bool y_matches{
            tile_viewport.position.y >= active_tile_viewport.position.y &&
            tile_viewport.position.y <
                active_tile_viewport.position.y + active_tile_viewport.size.y};

        if (x_matches && y_matches) {

          tile->SetViewPort(
              sf::FloatRect({active_tile_viewport.position},
                            {active_tile_viewport.size.x + tile_viewport.size.x,
                             active_tile_viewport.size.y}));
          tile->SetBorder();
          SetActiveTile(tile);
          break;
        }
      }
      // delete the active tile
      m_tiles.erase(std::remove(m_tiles.begin(), m_tiles.end(), active_tile),
                    m_tiles.end());
    }

    else if ((int(active_tile_viewport.position.x /
                  active_tile_viewport.size.x) %
              2) == 1) {

      // check for number of neighbours on the left
      int left_neighbours = 0;

      for (auto &tile : m_tiles) {
        sf::FloatRect tile_viewport = tile->GetView().getViewport();

        // for the left neighbour, x cooridinate of the active tile must be
        // equal
        bool x_matches{tile_viewport.position.x + tile_viewport.size.x ==
                       active_tile_viewport.position.x};

        // for the y coordinate, the neighbour position y cooridnate must be
        // equal or greater than the active tile position y coordinate and less
        // than the active tile position y coordinate + size.y
        bool y_matches{
            tile_viewport.position.y >= active_tile_viewport.position.y &&
            tile_viewport.position.y <
                active_tile_viewport.position.y + active_tile_viewport.size.y};

        if (x_matches && y_matches) {
          left_neighbours++;
        }
      }

      if (left_neighbours > 1) {
        std::cout << "Cannot remove tile, too many LEFT neighbours"
                  << std::endl;
        return;
      }

      // if there is only one neighbour, expand the left
      for (auto &tile : m_tiles) {
        sf::FloatRect tile_viewport = tile->GetView().getViewport();

        // for the left neighbour, x cooridinate of the active tile must be
        // equal
        bool x_matches{tile_viewport.position.x + tile_viewport.size.x ==
                       active_tile_viewport.position.x};

        // for the y coordinate, the neighbour position y cooridnate must be
        // equal or greater than the active tile position y coordinate and less
        // than the active tile position y coordinate + size.y
        bool y_matches{
            tile_viewport.position.y >= active_tile_viewport.position.y &&
            tile_viewport.position.y <
                active_tile_viewport.position.y + active_tile_viewport.size.y};

        if (x_matches && y_matches) {
          tile->SetViewPort(
              sf::FloatRect({tile_viewport.position},
                            {active_tile_viewport.size.x + tile_viewport.size.x,
                             active_tile_viewport.size.y}));
          tile->SetBorder();
          SetActiveTile(tile);
          break;
        }
      }
      // delete the active tile
      m_tiles.erase(std::remove(m_tiles.begin(), m_tiles.end(), active_tile),
                    m_tiles.end());
    }
  }

  else if (active_tile_width > active_tile_height) {
    // use the fact that we are splitting the viewport in half each time
    // if the position.y/size.y is even then we are what about when i have
    // splitting branches that use the same code? on the left, so expand the
    // right. If the position.y/size.y is odd then we are on the right, so
    // expand the left
    if ((int(active_tile_viewport.position.y / active_tile_viewport.size.y) %
         2) == 0) {
      // check for number of neighbours on the bottom
      int bottom_neighbours = 0;
      for (auto &tile : m_tiles) {
        sf::FloatRect tile_viewport = tile->GetView().getViewport();
        // for the bottom neighbour, max y coordinate of the active tile must be
        // equal to the neighbour min y coordinate
        bool y_matches{active_tile_viewport.position.y +
                           active_tile_viewport.size.y ==
                       tile_viewport.position.y};
        // for the x coordinate, the neighbour position x cooridnate must be
        // equal or greater than the active tile min x coordinate and less
        // than the active tile max x coordinate
        bool x_matches{
            tile_viewport.position.x >= active_tile_viewport.position.x &&
            tile_viewport.position.x <
                active_tile_viewport.position.x + active_tile_viewport.size.x};
        if (y_matches && x_matches) {
          bottom_neighbours++;
        }
      }
      // return early if there are too many neighbours
      if (bottom_neighbours > 1) {
        std::cout << "Cannot remove tile, too many BOTTOM neighbours"
                  << std::endl;
        return;
      }
      // if there is only one neighbour, expand the bottom
      for (auto &tile : m_tiles) {
        sf::FloatRect tile_viewport = tile->GetView().getViewport();
        // for the bottom neighbour, max y coordinate of the active tile must be
        // equal to the neighbour min y coordinate
        bool y_matches{active_tile_viewport.position.y +
                           active_tile_viewport.size.y ==
                       tile_viewport.position.y};
        // for the x coordinate, the neighbour position x cooridnate must be
        // equal or greater than the active tile min x coordinate
        // and less than the active tile max x coordinate
        bool x_matches{
            tile_viewport.position.x >= active_tile_viewport.position.x &&
            tile_viewport.position.x <
                active_tile_viewport.position.x + active_tile_viewport.size.x};
        if (y_matches && x_matches) {
          tile->SetViewPort(sf::FloatRect(
              {active_tile_viewport.position},
              {active_tile_viewport.size.x,
               active_tile_viewport.size.y + tile_viewport.size.y}));
          tile->SetBorder();
          SetActiveTile(tile);
          break;
        };
      };
      // delete the active tile
      m_tiles.erase(std::remove(m_tiles.begin(), m_tiles.end(), active_tile),
                    m_tiles.end());
    } else if ((int(active_tile_viewport.position.y /
                    active_tile_viewport.size.y) %
                2) == 1) {
      // check for number of neighbours on the top
      int top_neighbours = 0;
      for (auto &tile : m_tiles) {
        sf::FloatRect tile_viewport = tile->GetView().getViewport();
        // for the top neighbour, y cooridinate of the active tile must be
        // equal
        bool y_matches{tile_viewport.position.y + tile_viewport.size.y ==
                       active_tile_viewport.position.y};
        // for the x coordinate, the neighbour position x cooridnate must be
        // equal or greater than the active tile position x coordinate and
        // less than the active tile position x coordinate + size.x
        bool x_matches{
            tile_viewport.position.x >= active_tile_viewport.position.x &&
            tile_viewport.position.x <
                active_tile_viewport.position.x + active_tile_viewport.size.x};
        if (y_matches && x_matches) {
          top_neighbours++;
        }
      }
      if (top_neighbours > 1) {
        std::cout << "Cannot remove tile, too many TOP neighbours" << std::endl;
        return;
      }
      // if there is only one neighbour, expand the top
      for (auto &tile : m_tiles) {
        sf::FloatRect tile_viewport = tile->GetView().getViewport();
        // for the top neighbour, y cooridinate of the active tile must be
        // equal
        bool y_matches{tile_viewport.position.y + tile_viewport.size.y ==
                       active_tile_viewport.position.y};
        // for the x coordinate, the neighbour position x cooridnate must be
        // equal or greater than the active tile position x coordinate and
        // less than the active tile position x coordinate + size.x
        bool x_matches{
            tile_viewport.position.x >= active_tile_viewport.position.x &&
            tile_viewport.position.x <
                active_tile_viewport.position.x + active_tile_viewport.size.x};
        if (y_matches && x_matches) {
          tile->SetViewPort(sf::FloatRect(
              {tile_viewport.position},
              {active_tile_viewport.size.x,
               active_tile_viewport.size.y + tile_viewport.size.y}));
          tile->SetBorder();
          SetActiveTile(tile);
          break;
        }
      }
      // delete the active tile
      m_tiles.erase(std::remove(m_tiles.begin(), m_tiles.end(), active_tile),
                    m_tiles.end());
    }
  };
};

sf::RenderTexture &Session::GetTileOverlay() { return m_TileOverlay; };

std::vector<std::shared_ptr<Tile>> &Session::GetTiles() { return m_tiles; };
std::shared_ptr<Tile> Session::GetActiveTile() { return m_active_tile; };

void Session::SetActiveTile(std::shared_ptr<Tile> tile) {
  // set newly made Tile as active
  m_active_tile = tile;
  tile->SetBorderColourActive();

  SetInactiveTiles(tile);
};

void Session::SetInactiveTiles(std::shared_ptr<Tile> tile) {
  for (auto &t : m_tiles) {
    if (t != tile) {
      t->SetBorderColourInactive();
    }
  }
};

std::array<std::shared_ptr<Tile>, 4> Session::GetTileNeighbours() {

  // the position of the array will correspond to a direction
  // 0 - up, 1 - right, 2 - down, 3 - left
  std::array<std::shared_ptr<Tile>, 4> neighbours;
  neighbours.fill(nullptr);

  // get the active tile
  std::shared_ptr<Tile> active_tile = GetActiveTile();
  // get the active tile viewport
  sf::FloatRect active_tile_viewport = active_tile->GetView().getViewport();

  // up position
  if (active_tile_viewport.position.y != 0) {
    for (auto &tile : m_tiles) {
      sf::FloatRect tile_viewport = tile->GetView().getViewport();

      // the y position must be equal (by offset)
      bool y_matches{tile_viewport.position.y + tile_viewport.size.y ==
                     active_tile_viewport.position.y};

      // we will try and find the best x match
      bool x_matches{active_tile_viewport.position.x ==
                     tile_viewport.position.x};

      if (x_matches && y_matches) {
        neighbours[0] = tile;
        break;
      }
    }
    // however if we can't find a perfect match, we will try and
    // find the next best thing
    if (neighbours[0] == nullptr) {
      for (auto &tile : m_tiles) {
        sf::FloatRect tile_viewport = tile->GetView().getViewport();

        // the y position must be equal (by offset)
        bool y_matches{tile_viewport.position.y + tile_viewport.size.y ==
                       active_tile_viewport.position.y};

        bool x_kind_of_matches{tile_viewport.position.x <
                                   active_tile_viewport.position.x &&
                               tile_viewport.position.x + tile_viewport.size.x >
                                   active_tile_viewport.position.x};

        if (x_kind_of_matches && y_matches) {
          neighbours[0] = tile;
          break;
        }
      }
    }
  };

  // right position
  if (active_tile_viewport.position.x + active_tile_viewport.size.x !=
      SteamRot::kWindowSize.x) {
    for (auto &tile : m_tiles) {
      sf::FloatRect tile_viewport = tile->GetView().getViewport();
      // the x position must be equal (by offset)
      bool x_matches{tile_viewport.position.x ==
                     active_tile_viewport.position.x +
                         active_tile_viewport.size.x};
      // we will try and find the best y match
      bool y_matches{active_tile_viewport.position.y ==
                     tile_viewport.position.y};
      if (x_matches && y_matches) {
        neighbours[1] = tile;
        break;
      }
    }
    // however if we can't find a perfect match, we will try and
    // find the next best thing
    if (neighbours[1] == nullptr) {
      for (auto &tile : m_tiles) {
        sf::FloatRect tile_viewport = tile->GetView().getViewport();
        // the x position must be equal (by offset)
        bool x_matches{tile_viewport.position.x ==
                       active_tile_viewport.position.x +
                           active_tile_viewport.size.x};
        bool y_kind_of_matches{tile_viewport.position.y <
                                   active_tile_viewport.position.y &&
                               tile_viewport.position.y + tile_viewport.size.y >
                                   active_tile_viewport.position.y};
        if (x_matches && y_kind_of_matches) {
          neighbours[1] = tile;
          break;
        }
      }
    }
  };

  // down position
  if (active_tile_viewport.position.y + active_tile_viewport.size.y !=
      SteamRot::kWindowSize.y) {
    for (auto &tile : m_tiles) {
      sf::FloatRect tile_viewport = tile->GetView().getViewport();
      // the y position must be equal (by offset)
      bool y_matches{tile_viewport.position.y ==
                     active_tile_viewport.position.y +
                         active_tile_viewport.size.y};
      // we will try and find the best x match
      bool x_matches{active_tile_viewport.position.x ==
                     tile_viewport.position.x};
      if (x_matches && y_matches) {
        neighbours[2] = tile;
        break;
      };
    }
    // however if we can't find a perfect match, we will try and
    // find the next best thing
    if (neighbours[2] == nullptr) {
      for (auto &tile : m_tiles) {
        sf::FloatRect tile_viewport = tile->GetView().getViewport();
        // the y position must be equal (by offset)
        bool y_matches{tile_viewport.position.y ==
                       active_tile_viewport.position.y +
                           active_tile_viewport.size.y};
        bool x_kind_of_matches{tile_viewport.position.x <
                                   active_tile_viewport.position.x &&
                               tile_viewport.position.x + tile_viewport.size.x >
                                   active_tile_viewport.position.x};
        if (x_kind_of_matches && y_matches) {
          neighbours[2] = tile;
          break;
        };
      };
    };
  };

  // left position
  if (active_tile_viewport.position.x != 0) {
    for (auto &tile : m_tiles) {
      sf::FloatRect tile_viewport = tile->GetView().getViewport();
      // the x position must be equal (by offset)
      bool x_matches{tile_viewport.position.x + tile_viewport.size.x ==
                     active_tile_viewport.position.x};
      // we will try and find the best y match
      bool y_matches{active_tile_viewport.position.y ==
                     tile_viewport.position.y};
      if (x_matches && y_matches) {
        neighbours[3] = tile;
        break;
      }
    }
    // however if we can't find a perfect match, we will try and
    // find the next best thing
    if (neighbours[3] == nullptr) {
      for (auto &tile : m_tiles) {
        sf::FloatRect tile_viewport = tile->GetView().getViewport();
        // the x position must be equal (by offset)
        bool x_matches{tile_viewport.position.x + tile_viewport.size.x ==
                       active_tile_viewport.position.x};
        bool y_kind_of_matches{tile_viewport.position.y <
                                   active_tile_viewport.position.y &&
                               tile_viewport.position.y + tile_viewport.size.y >
                                   active_tile_viewport.position.y};
        if (x_matches && y_kind_of_matches) {
          neighbours[3] = tile;
          break;
        }
      }
    }
  }

  return neighbours;
};
