#include "GameContext.h"
namespace steamrot {

/////////////////////////////////////////////////
GameContext::GameContext(sf::RenderWindow &window, EventHandler &event_handler,

                         const size_t &loop_number, AssetManager &asset_manager,
                         const EnvironmentType &env_type)
    : game_window(window), event_handler(event_handler),
      loop_number(loop_number), asset_manager(asset_manager),
      env_type(env_type) {}
} // namespace steamrot
