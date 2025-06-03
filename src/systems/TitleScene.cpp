#include "TitleScene.h"

namespace steamrot {
TitleScreen::TitleScreen(size_t pool_size, const SceneData *scene_data,
                         const uuids::uuid &id)
    : Scene(pool_size, scene_data, id) {
  // Initialization code specific to TitleScreen can go here
}
} // namespace steamrot
