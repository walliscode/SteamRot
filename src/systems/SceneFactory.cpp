////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "SceneFactory.h"

#include "magic_enum/magic_enum.hpp"
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
  json scene_data = m_data_manager.LoadSceneDataFromJson(
      magic_enum::enum_name(scene_type).data());

  if (scene_type == SceneType::menu) {
    return CreateMenuScene(scene_data, scene_uuid);
  }
  // not sure how it would be possible to get here, maybe some kind of cast from
  // size_t
  return nullptr;
}

std::unique_ptr<MenuScene>
SceneFactory::CreateMenuScene(json scene_data, const uuids::uuid &scene_uuid) {
  // create a new menu scene object, we are creating a raw pointer here due to
  // MenuScene having a private constuctor
  std::unique_ptr<MenuScene> menu_scene(
      new MenuScene(100, scene_data, scene_uuid));
  return menu_scene;
}

} // namespace steamrot
