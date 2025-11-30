#include "GameContext.h"
namespace steamrot {

/////////////////////////////////////////////////
GameContext::GameContext(GameResources &resources,
                         const PathProvider &path_provider)
    : game_resources(resources), game_window(resources.game_window),
      event_handler(resources.event_handler),
      loop_number(resources.loop_number),
      mouse_position(resources.mouse_position),
      asset_manager(resources.asset_manager), path_provider(path_provider) {}
} // namespace steamrot
