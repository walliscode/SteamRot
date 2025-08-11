/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the SceneFactory class.
/////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "SceneFactory.h"
#include "CraftingScene.h"
#include "TitleScene.h"
#include "uuid.h"
#include <iostream>
#include <memory>
#include <string>

namespace steamrot {
////////////////////////////////////////////////////////////
SceneFactory::SceneFactory(const GameContext game_context)
    : m_game_context(game_context) {}

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
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateDefaultScene(const SceneType &scene_type) {

  // generate UUID for the scene
  uuids::uuid scene_uuid = CreateUUID();
  std::cout << "Creating scene with UUID: " << scene_uuid << std::endl;

  switch (scene_type) {

  case SceneType::SceneType_TITLE: {
    std::unique_ptr<TitleScene> title_scene(
        new TitleScene(scene_uuid, m_game_context));
    return title_scene;
  }

  case SceneType::SceneType_CRAFTING: {
    std::unique_ptr<CraftingScene> crafting_scene(
        new CraftingScene(scene_uuid, m_game_context));
    return crafting_scene;
  }

  default:
    FailInfo fail_info(FailMode::NonExistentEnumValue, "SceneType not found");

    return std::unexpected(fail_info);
  }
}

} // namespace steamrot
