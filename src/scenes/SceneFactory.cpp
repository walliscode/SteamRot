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

  // make a pointer to a base Scene and assign it to a derived scene type
  std::unique_ptr<Scene> scene_ptr{nullptr};

  switch (scene_type) {

  case SceneType::SceneType_TITLE: {
    // create a unique pointer to a TitleScene as it can't be constructed
    // directly into scene_ptr
    std::unique_ptr<TitleScene> title_scene(
        new TitleScene(scene_uuid, m_game_context));
    scene_ptr = std::move(title_scene);
    break;
  }

  case SceneType::SceneType_CRAFTING: {
    std::unique_ptr<CraftingScene> crafting_scene(
        new CraftingScene(scene_uuid, m_game_context));

    scene_ptr = std::move(crafting_scene);
    break;
  }

  default:
    FailInfo fail_info(FailMode::NonExistentEnumValue, "SceneType not found");

    return std::unexpected(fail_info);
  }
  std::cout << "Scene of type " << static_cast<int>(scene_type)
            << " created with UUID: " << scene_ptr->GetSceneInfo().id
            << std::endl;
  // configure scene entities from default data
  auto configure_result = scene_ptr->ConfigureFromDefault();
  if (!configure_result) {
    return std::unexpected(configure_result.error());
  }
  std::cout << "Entities configured for scene UUID: "
            << scene_ptr->GetSceneInfo().id << std::endl;
  // Get ArchetypeManager to gerenate all archetypes
  auto archetype_result = scene_ptr->m_entity_manager.GenerateAllArchetypes();
  if (!archetype_result) {
    return std::unexpected(archetype_result.error());
  }
  std::cout << "Archetypes generated for scene UUID: "
            << scene_ptr->GetSceneInfo().id << std::endl;
  // configure LogicMap
  LogicFactory logic_factory(scene_type, scene_ptr->GetLogicContext());
  auto create_map_result = logic_factory.CreateLogicMap();
  if (!create_map_result) {
    return std::unexpected(create_map_result.error());
  }
  // pass the created logic map to the scene
  scene_ptr->SetLogicMap(std::move(create_map_result.value()));
  return scene_ptr;
}

} // namespace steamrot
