/////////////////////////////////////////////////
/// @file
/// @brief Implementation of configuration helpers for entity tests
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configuration_helpers.h"
#include "CGrimoireMachina.h"
#include "CUserInterface.h"
#include "FlatbuffersDataLoader.h"
#include "catch2/catch_test_macros.hpp"
#include "emp_helpers.h"
#include "entities_generated.h"
#include "scenes_generated.h"
#include "ui_element_factory_helpers.h"
#include "user_interface_generated.h"
namespace steamrot::tests {

/////////////////////////////////////////////////
void CompareToDefault(const CUserInterface &actual) {

  // create instance of CUserInterface
  CUserInterface c_user_interface;

  REQUIRE(actual.m_name == c_user_interface.m_name);
  REQUIRE(actual.m_active == c_user_interface.m_active);
  REQUIRE(actual.m_UI_visible == c_user_interface.m_UI_visible);
}

/////////////////////////////////////////////////
void CompareToDefault(const CGrimoireMachina &actual) {

  // create instance of CGrimoireMachina
  CGrimoireMachina c_grimoire_machina;

  REQUIRE(actual.m_all_fragments.size() ==
          c_grimoire_machina.m_all_fragments.size());
  REQUIRE(actual.m_active == c_grimoire_machina.m_active);
  REQUIRE(actual.m_machina_forms.size() ==
          c_grimoire_machina.m_machina_forms.size());
  REQUIRE(actual.m_all_joints.size() == c_grimoire_machina.m_all_joints.size());
  REQUIRE(actual.m_holding_form == c_grimoire_machina.m_holding_form);
}

/////////////////////////////////////////////////
void TestEMPIsDefaultConstructed(const EntityMemoryPool &entity_memory_pool) {

  // cycle through entity memory pool and compare all Component instances
  for (size_t i{0}; i < emp_helpers::GetMemoryPoolSize(entity_memory_pool);
       i++) {

    // check each component type
    /////////////////////////////////////////////////
    const CUserInterface &c_user_interface =
        emp_helpers::GetComponent<CUserInterface>(i, entity_memory_pool);
    CompareToDefault(c_user_interface);

    /////////////////////////////////////////////////
    const CGrimoireMachina &c_grimoire_machina =
        emp_helpers::GetComponent<CGrimoireMachina>(i, entity_memory_pool);
    CompareToDefault(c_grimoire_machina);
  }
}

/////////////////////////////////////////////////
void CompareToData(const CUserInterface &actual,
                   const UserInterfaceData &data) {

  REQUIRE(actual.m_name == data.ui_name()->str());
  REQUIRE(actual.m_UI_visible == data.start_visible());
  // test the root element and its nested structure
  REQUIRE(actual.m_root_element != nullptr);
  REQUIRE(data.root_ui_element() != nullptr);

  const auto *root_panel_data = data.root_ui_element();
  TestNestedElementProperties(*actual.m_root_element, root_panel_data,
                              UIElementDataUnion::UIElementDataUnion_PanelData);
}

/////////////////////////////////////////////////
void CompareToData(const CGrimoireMachina &actual,
                   const GrimoireMachinaData &data) {

  REQUIRE(actual.m_all_fragments.size() == data.fragments()->size());
  REQUIRE(actual.m_all_joints.size() == data.joints()->size());
}

/////////////////////////////////////////////////
void TestConfigurationOfEMPfromDefaultData(
    const EntityMemoryPool &entity_memory_pool, const SceneType scene_type) {

  FlatbuffersDataLoader flatbuffers_data_loader(EnvironmentType::Test);

  // get default scene data
  auto scene_data_result = flatbuffers_data_loader.ProvideSceneData(scene_type);
  if (!scene_data_result.has_value()) {
    FAIL("Failed to load scene data: " + scene_data_result.error().message);
  }
  const SceneData &scene_data =
      *flatbuffers_data_loader.ProvideSceneData(scene_type).value();

  const EntityCollection &entity_collection = *scene_data.entity_collection();
  // check the list of entities is not empty
  REQUIRE(!entity_collection.entities()->empty());
  // some helper values
  size_t entity_count = entity_collection.entities()->size();
  // check the entity memory pool is big enough
  REQUIRE(emp_helpers::GetMemoryPoolSize(entity_memory_pool) >= entity_count);
  REQUIRE(emp_helpers::GetMemoryPoolSize(entity_memory_pool) ==
          scene_data.entity_collection()->entity_memory_pool_size());

  // Go through each entity in the scene data and check that the
  // EntityMemoryPool has been configured correctly
  for (size_t i = 0; i < entity_count; ++i) {
    const EntityData *entity_data = entity_collection.entities()->Get(i);
    REQUIRE(entity_data != nullptr); // Ensure the entity data is not null
    // CUserInterface component configuration
    if (entity_data->c_user_interface()) {
      const CUserInterface &c_user_interface =
          emp_helpers::GetComponent<CUserInterface>(i, entity_memory_pool);
      CompareToData(c_user_interface, *entity_data->c_user_interface());
    }
    // CGrimoireMachina component configuration
    if (entity_data->c_grimoire_machina()) {
      const CGrimoireMachina &c_grimoire_machina =
          emp_helpers::GetComponent<CGrimoireMachina>(i, entity_memory_pool);
      CompareToData(c_grimoire_machina, *entity_data->c_grimoire_machina());
    }
  }
}

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
  FlatbuffersDataLoader flatbuffers_data_loader(EnvironmentType::Test);
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
