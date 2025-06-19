#include "GameContext.h"
namespace steamrot {

/////////////////////////////////////////////////
GameContext::GameContext(sf::RenderWindow &window, const EventBitset &events,
                         const sf::Vector2i &mouse_position,
                         const size_t &loop_number, AssetManager &asset_manager,
                         DataManager &data_manager)
    : game_window(window), user_events(events), mouse_position(mouse_position),
      loop_number(loop_number), asset_manager(asset_manager),
      data_manager(data_manager) {}
} // namespace steamrot
