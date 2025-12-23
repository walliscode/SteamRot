#include "GameContext.h"
namespace steamrot {

/////////////////////////////////////////////////
GameContext::GameContext(EngineResources &resources)
    : engine_resources(resources), game_window(resources.game_window),
      event_handler(resources.event_handler),
      loop_number(resources.loop_number),
      mouse_position(resources.mouse_position),
      data_access_factory(resources.data_access_factory),
      asset_manager(resources.asset_manager) {}
} // namespace steamrot
