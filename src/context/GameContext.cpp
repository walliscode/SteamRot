/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GameContext struct
/////////////////////////////////////////////////

#include "GameContext.h"

namespace steamrot {

/////////////////////////////////////////////////
GameContext::GameContext(GameResources &resources)
    : game_window(resources.game_window),
      event_handler(resources.event_handler),
      mouse_position(resources.mouse_position),
      loop_number(resources.loop_number),
      asset_manager(resources.asset_manager),
      env_type(resources.env_type) {}

} // namespace steamrot
