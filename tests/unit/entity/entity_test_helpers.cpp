/////////////////////////////////////////////////
/// @file
/// @brief Implementation of configuration helpers for entity tests
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "entity_test_helpers.h"
#include "CGrimoireMachina.h"
#include "CUserInterface.h"
#include "FlatbuffersDataLoader.h"
#include "archetype_helpers.h"
#include "catch2/catch_test_macros.hpp"
#include "scenes_generated.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
void TestArchetypesOfUnconfiguredEMP(
    const std::unordered_map<ArchetypeID, Archetype> &archetypes) {
  // check that the archetypes map is empty
  REQUIRE(archetypes.empty());
  // check that the only archetype member is 0 and that it contains all the
  // entity indexes
  REQUIRE(archetypes.size() == 1);
  // check that the key is 0
  REQUIRE(archetypes.begin()->first == 0);
}

/////////////////////////////////////////////////
void TestArchetypesOfConfiguredEMPfromDefaultData(
    const std::unordered_map<ArchetypeID, Archetype> &archetypes,
    const SceneType scene_type) {

  // load the scene data
  FlatbuffersDataLoader flatbuffers_data_loader;
  const SceneData &scene_data =
      *flatbuffers_data_loader.ProvideSceneData(scene_type).value();
  const EntityCollection &entity_collection = *scene_data.entity_collection();

  // create a vector of all possible positions from entity memory pool size
  std::vector<size_t> all_positions;
  for (size_t i = 0;
       i < scene_data.entity_collection()->entity_memory_pool_size(); ++i) {
    all_positions.push_back(i);
  }
  // create own map and fill it
  std::unordered_map<ArchetypeID, Archetype> expected_archetypes;
  for (size_t i = 0; i < entity_collection.entities()->size(); ++i) {
    const EntityData *entity_data = entity_collection.entities()->Get(i);
    if (entity_data == nullptr) {
      continue; // Skip null entities
    }

    // Generate ArchetypeID for the entity
    ArchetypeID archetype_id{0};
    // Check each component and set the corresponding bit in the ArchetypeID if
    // it is active
    if (entity_data->c_user_interface()) {
      CUserInterface c_user_interface;
      archetype_id.set(c_user_interface.GetComponentRegisterIndex());
    }
    if (entity_data->c_grimoire_machina()) {
      CGrimoireMachina c_grimoire_machina;
      archetype_id.set(c_grimoire_machina.GetComponentRegisterIndex());
    }
    // fill in further components as needed

    // Add the entity index to the archetype
    expected_archetypes[archetype_id].push_back(i);

    // remove the position from all_positions
    auto it = std::find(all_positions.begin(), all_positions.end(), i);
    if (it != all_positions.end()) {
      all_positions.erase(it);
    }
  }

  // 0 is the empty archetype ID
  expected_archetypes[0] = all_positions;

  // compare the expected archetypes with the actual ones
  REQUIRE(archetypes == expected_archetypes);
}

} // namespace steamrot::tests
