#include "GameContext.h"
namespace steamrot {

/////////////////////////////////////////////////
GameContext::GameContext(GameCore &core)
    : game_core(core), game_window(core.game_window),
      event_handler(core.event_handler), loop_number(core.loop_number),
      mouse_position(core.mouse_position), asset_manager(core.asset_manager) {}
} // namespace steamrot
