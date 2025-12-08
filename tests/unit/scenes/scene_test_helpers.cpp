/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Scene test helpers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "scene_test_helpers.h"
#include "EntityMemoryPoolEqualsMatcher.h"
#include "EventHandler.h"
#include "FlatbuffersConfigurator.h"
#include "entity_test_helpers.h"
#include "logic_test_helpers.h"
#include "uuid.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

namespace steamrot::tests {

/////////////////////////////////////////////////
const uuids::uuid CreateUuid() {
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

/////////////////////////////////////////////////
void CheckDefaultSceneConfiguration(const Scene &scene) {

  const SceneType scene_type = scene.GetSceneInfo().type;

  // check entity memory pool default configuration by creating expected pool
  EntityMemoryPool expected_pool;
  steamrot::EventHandler event_handler;
  FlatbuffersEntityConfigurator configurator(event_handler);
  auto result =
      configurator.ConfigureEntitiesFromDefaultData(expected_pool, scene_type);
  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  REQUIRE_THAT(scene.GetEntityMemoryPool(),
               EqualsEntityMemoryPool(expected_pool));

  // check archetypes of configured entity memory pool
  TestArchetypesOfConfiguredEMPfromDefaultData(scene.GetArchetypes(),
                                               scene_type);

  // check logic map default configuration
  CheckStaticLogicCollections(scene.GetLogicMap(), scene_type);
}
} // namespace steamrot::tests
