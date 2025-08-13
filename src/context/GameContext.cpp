#include "GameContext.h"
namespace steamrot {

/////////////////////////////////////////////////
GameContext::GameContext(sf::RenderWindow &window, EventHandler &event_handler,
                         const sf::Vector2i &mouse_position,
                         const size_t &loop_number, AssetManager &asset_manager,
                         DataManager &data_manager,
                         const EnvironmentType &env_type)
    : game_window(window), event_handler(event_handler),
      mouse_position(mouse_position), loop_number(loop_number),
      asset_manager(asset_manager), data_manager(data_manager),
      env_type(env_type) {}
} // namespace steamrot
