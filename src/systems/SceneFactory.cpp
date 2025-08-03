////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "SceneFactory.h"
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
std::unique_ptr<Scene> SceneFactory::CreateScene(const SceneType &scene_type) {

  // generate UUID for the scene
  uuids::uuid scene_uuid = CreateUUID();
  std::cout << "Creating scene with UUID: " << scene_uuid << std::endl;

  // load scene data
  const SceneData *scene_data =
      m_game_context.data_manager.ProvideSceneData(scene_type);

  switch (scene_type) {

    // TitleScene
  case SceneType::SceneType_TITLE:
    std::cout << "Title scene type detected." << std::endl;
    return CreateTitleScene(scene_data, scene_uuid);

  case SceneType::SceneType_CRAFTING:
    std::cout << "Crafting scene type detected." << std::endl;
    // create a new crafting scene object, we are creating a raw pointer here
    // due to CraftingScene having a private constuctor

    return CreateCraftingScene(scene_data, scene_uuid);

  default:
    std::cerr << "Unknown scene type: " << static_cast<int>(scene_type)
              << std::endl;
    throw std::runtime_error("SceneFactory::CreateScene: Unknown scene type.");
  }
}
////////////////////////////////////////////////////////////
std::unique_ptr<TitleScene>
SceneFactory::CreateTitleScene(const SceneData *scene_data,
                               const uuids::uuid &scene_uuid) {

  // create a new title scene object, we are creating a raw pointer here due to
  // TitleScene having a private constuctor
  std::unique_ptr<TitleScene> title_scene(
      new TitleScene(100, scene_data, scene_uuid, m_game_context));
  std::cout << "Created TitleScene with UUID: " << scene_uuid << std::endl;
  return title_scene;
}

/////////////////////////////////////////////////
std::unique_ptr<CraftingScene>
SceneFactory::CreateCraftingScene(const SceneData *scene_data,
                                  const uuids::uuid &scene_uuid) {
  // create a new crafting scene object, we are creating a raw pointer here due
  // to CraftingScene having a private constuctor
  std::unique_ptr<CraftingScene> crafting_scene(
      new CraftingScene(100, scene_data, scene_uuid, m_game_context));
  std::cout << "Created CraftingScene with UUID: " << scene_uuid << std::endl;
  return crafting_scene;
}
} // namespace steamrot
