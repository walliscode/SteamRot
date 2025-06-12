////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "MenuScene.h"
#include "Scene.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <cstddef>

namespace steamrot {
MenuScene::MenuScene(size_t pool_size, const SceneData *scene_data,
                     const uuids::uuid &id)
    : Scene(pool_size, scene_data, id) {}
////////////////////////////////////////////////////////////
void MenuScene::sMovement() {};

void MenuScene::sRender() {};
} // namespace steamrot
