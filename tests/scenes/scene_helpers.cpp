/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Scene test helpers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "scene_helpers.h"
#include "PathProvider.h"
#include "uuid.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
const uuids::uuid create_uuid() {
  // taken straight from the uuid library example
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 generator(seq);
  uuids::uuid_random_generator gen{generator};
  uuids::uuid const id = gen();
  return id;
}

/////////////////////////////////////////////////
const GameContext create_game_context() {
  sf::RenderWindow window(sf::VideoMode({800, 600}), "Test Window");
  steamrot::EventHandler event_handler;
  sf::Vector2i mouse_position(0, 0);
  size_t loop_number = 0;
  steamrot::AssetManager asset_manager{EnvironmentType::Test};
  steamrot::DataManager data_manager;

  steamrot::GameContext game_context(window, event_handler, mouse_position,
                                     loop_number, asset_manager, data_manager,
                                     EnvironmentType::Test);

  return game_context;
}
} // namespace steamrot::tests
