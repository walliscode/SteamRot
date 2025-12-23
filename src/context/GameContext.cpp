#include "GameContext.h"
namespace steamrot {

/////////////////////////////////////////////////
GameContext::GameContext(EngineResources &resources,
                         DataAccessFactory &data_access_factory)
    : engine_resources(resources), game_window(resources.game_window),
      event_handler(resources.event_handler), loop_number(resources.loop_number),
      mouse_position(resources.mouse_position),
      asset_manager(resources.asset_manager),
      data_access_factory(data_access_factory) {}
} // namespace steamrot
