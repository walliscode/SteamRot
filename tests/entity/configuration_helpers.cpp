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
#include <unistd.h>

namespace steamrot::tests {

/////////////////////////////////////////////////
void CompareToDefault(const CUserInterface &actual) {

  // create instance of CUserInterface
  CUserInterface c_user_interface;

  REQUIRE(actual.UIName == c_user_interface.UIName);
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

  REQUIRE(actual.UIName == data.ui_name()->str());
  REQUIRE(actual.m_UI_visible == data.start_visible());
}

/////////////////////////////////////////////////
void CompareToData(const CGrimoireMachina &actual,
                   const GrimoireMachinaData &data) {

  REQUIRE(actual.m_all_fragments.size() == data.fragments()->size());
  REQUIRE(actual.m_all_joints.size() == data.joints()->size());
}

/////////////////////////////////////////////////
void TestConfigurationOfEMPfromDefaultData(EntityMemoryPool &entity_memory_pool,
                                           const SceneType scene_type) {

  FlatbuffersDataLoader flatbuffers_data_loader(EnvironmentType::Test);
  // get default scene data
  const SceneData &scene_data =
      *flatbuffers_data_loader.ProvideSceneData(scene_type).value();

  const EntityCollection &entity_collection = *scene_data.entity_collection();
  // check the list of entities is not empty
  REQUIRE(!entity_collection.entities()->empty());
  // some helper values
  size_t entity_count = entity_collection.entities()->size();
  // check the entity memory pool is big enough
  REQUIRE(emp_helpers::GetMemoryPoolSize(entity_memory_pool) >= entity_count);

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
} // namespace steamrot::tests
