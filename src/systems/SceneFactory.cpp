////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "SceneFactory.h"

#include "SceneType.h"

#include "uuid.h"
#include <memory>
#include <string>

namespace steamrot {
////////////////////////////////////////////////////////////
SceneFactory::SceneFactory() : m_data_manager() {}

////////////////////////////////////////////////////////////
const uuids::uuid SceneFactory::CreateUUID() {

  // taken straight from the uuid library example
  std::random_device rd;
  auto seed_data = std::array<int, std::mt19937::state_size>{};
  std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
  std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
  std::mt19937 generator(seq);
  uuids::uuid_random_generator gen{generator};

  uuids::uuid const id = gen();

  return id;
}
////////////////////////////////////////////////////////////
std::unique_ptr<Scene> SceneFactory::CreateScene(const SceneType &scene_type) {

  // generate UUID for the scene
  uuids::uuid scene_uuid = CreateUUID();

  // load scene data
  const SceneData *scene_data = m_data_manager.ProvideSceneData(scene_type);

  switch (scene_type) {
  case SceneType::title:
    return CreateTitleScene(scene_data, scene_uuid);
  case SceneType::menu:
    return CreateMenuScene(scene_data, scene_uuid);

  default:
    // not sure how it would be possible to get here, maybe some kind of cast
    // from size_t
    return nullptr;
  }
}
////////////////////////////////////////////////////////////
std::unique_ptr<TitleScene>
SceneFactory::CreateTitleScene(const SceneData *scene_data,
                               const uuids::uuid &scene_uuid) {
  // create a new title scene object, we are creating a raw pointer here due to
  // TitleScene having a private constuctor
  std::unique_ptr<TitleScene> title_scene(
      new TitleScene(100, scene_data, scene_uuid));
  return title_scene;
}

////////////////////////////////////////////////////////////
std::unique_ptr<MenuScene>
SceneFactory::CreateMenuScene(const SceneData *scene_data,
                              const uuids::uuid &scene_uuid) {
  // create a new menu scene object, we are creating a raw pointer here due to
  // MenuScene having a private constuctor
  std::unique_ptr<MenuScene> menu_scene(
      new MenuScene(100, scene_data, scene_uuid));
  return menu_scene;
}

} // namespace steamrot
