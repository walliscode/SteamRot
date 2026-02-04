/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for SceneManager
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneManager.h"
#include "FlatbuffersSceneDataProvider.h"
#include "Subscriber.h"
#include "TestFixture.h"
#include "load_scene_collection_data.h"
#include "scene_types_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SceneManager::SceneManager initializes correctly",
          "[SceneManager]") {
  // Arrange
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  // Act
  steamrot::SceneManager scene_manager(fixture.GetGameContext());
  // Assert
  SUCCEED("SceneManager initialized without errors");
}

TEST_CASE("SceneManager::StartUp loads configuration successfully",
          "[SceneManager]") {
  // Arrange
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneManager scene_manager(fixture.GetGameContext());
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().engine_resources.event_handler;

  // initial check - no subscribers
  REQUIRE(scene_manager.GetSubscriptions().empty());
  REQUIRE(event_handler.GetSubcriberRegister().empty());
  // Act
  auto result = scene_manager.StartUp();
  // Assert
  if (!result.has_value())
    FAIL(result.error().message);

  // check subscribers
  const std::vector<std::shared_ptr<steamrot::Subscriber>> &subscriptions =
      scene_manager.GetSubscriptions();
  REQUIRE(subscriptions.size() == 1);
  REQUIRE(subscriptions[0]->m_trigger_event_type ==
          steamrot::EventType::CHANGE_SCENE);

  // check subscribers have been registered with the EventHandler
  const auto &registered_subscribers = event_handler.GetSubcriberRegister();
  REQUIRE(registered_subscribers.size() == 1);
}

TEST_CASE("SceneManager::AddScenesFromSceneCollectionData clears exisiting "
          "scenes and adds new ones",
          "[SceneManager]") {

  // Arrange
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneManager scene_manager(fixture.GetGameContext());
  const auto &scene_map = scene_manager.GetScenes();

  auto scene_add_result =
      scene_manager.AddSceneFromDefault(steamrot::SceneType::TITLE);
  if (!scene_add_result.has_value()) {
    FAIL(scene_add_result.error().message);
  }
  // check that there is one scene in the map
  REQUIRE(scene_map.size() == 1);

  // create empty SceneCollectionData
  steamrot::SceneCollectionData scene_collection_data;

  // Act
  auto scene_collection_add_result =
      scene_manager.AddScenesFromSceneCollectionData(scene_collection_data);
  if (!scene_collection_add_result.has_value()) {
    FAIL(scene_collection_add_result.error().message);
  }
  // Assert
  // check that the scene map is now empty
  REQUIRE(scene_map.empty());
}

TEST_CASE("SceneManager::AddScenesFromSceneCollectionData loads three scenes "
          "with valid data",
          "[SceneManager]") {

  // Load test data from JSON (compiled to binary)
  auto [data_buffer, scene_collection_data_fbs] = LoadSceneCollectionTestData();
  REQUIRE(scene_collection_data_fbs != nullptr);

  // Verify the loaded data structure
  REQUIRE(scene_collection_data_fbs->scene_data() != nullptr);
  const auto *scenes = scene_collection_data_fbs->scene_data();
  REQUIRE(scenes->size() == 3);

  // Verify Scene 1 (TITLE scene)
  const auto *scene_1 = scenes->Get(0);
  REQUIRE(scene_1 != nullptr);
  REQUIRE(scene_1->scene_info() != nullptr);
  REQUIRE(scene_1->scene_info()->scene_type() == steamrot::SceneTypeFbs_TITLE);
  REQUIRE(scene_1->scene_resources_config() != nullptr);
  REQUIRE(scene_1->scene_resources_config()->texture_width() == 800);
  REQUIRE(scene_1->scene_resources_config()->texture_height() == 600);
  REQUIRE(scene_1->entity_collection() != nullptr);
  REQUIRE(scene_1->entity_collection()->entity_memory_pool_size() == 30);

  // Check Scene 1 entities
  const auto *scene_1_entities = scene_1->entity_collection()->entities();
  REQUIRE(scene_1_entities != nullptr);
  REQUIRE(scene_1_entities->size() == 2);

  // Check Scene 1, Entity 0 (UI element)
  const auto *scene_1_entity_0 = scene_1_entities->Get(0);
  REQUIRE(scene_1_entity_0 != nullptr);
  REQUIRE(scene_1_entity_0->c_user_interface() != nullptr);
  REQUIRE(scene_1_entity_0->c_user_interface()->ui_name()->str() ==
          "title_ui_main_panel");
  REQUIRE(scene_1_entity_0->c_user_interface()->is_visible() == true);
  REQUIRE(scene_1_entity_0->c_user_interface()->root_ui_element() != nullptr);
  const auto *scene_1_root_element =
      scene_1_entity_0->c_user_interface()->root_ui_element();
  REQUIRE(scene_1_root_element->base_data() != nullptr);
  REQUIRE(scene_1_root_element->base_data()->position() != nullptr);
  REQUIRE(scene_1_root_element->base_data()->position()->x() == 50.0f);
  REQUIRE(scene_1_root_element->base_data()->position()->y() == 100.0f);
  REQUIRE(scene_1_root_element->base_data()->children_active() == true);
  REQUIRE(scene_1_root_element->base_data()->children() != nullptr);
  REQUIRE(scene_1_root_element->base_data()->children()->size() == 2);

  // Check Scene 1, Entity 1 (grimoire machina)
  const auto *scene_1_entity_1 = scene_1_entities->Get(1);
  REQUIRE(scene_1_entity_1 != nullptr);
  REQUIRE(scene_1_entity_1->c_grimoire_machina() != nullptr);
  REQUIRE(scene_1_entity_1->c_grimoire_machina()->fragments() != nullptr);
  REQUIRE(scene_1_entity_1->c_grimoire_machina()->fragments()->size() == 2);
  REQUIRE(scene_1_entity_1->c_grimoire_machina()->fragments()->Get(0)->str() ==
          "title_fragment_alpha");
  REQUIRE(scene_1_entity_1->c_grimoire_machina()->joints() != nullptr);
  REQUIRE(scene_1_entity_1->c_grimoire_machina()->joints()->size() == 1);

  // Verify Scene 2 (CRAFTING scene)
  const auto *scene_2 = scenes->Get(1);
  REQUIRE(scene_2 != nullptr);
  REQUIRE(scene_2->scene_info() != nullptr);
  REQUIRE(scene_2->scene_info()->scene_type() ==
          steamrot::SceneTypeFbs_CRAFTING);
  REQUIRE(scene_2->scene_resources_config() != nullptr);
  REQUIRE(scene_2->scene_resources_config()->texture_width() == 1024);
  REQUIRE(scene_2->scene_resources_config()->texture_height() == 768);
  REQUIRE(scene_2->entity_collection() != nullptr);
  REQUIRE(scene_2->entity_collection()->entity_memory_pool_size() == 50);

  // Check Scene 2 entities
  const auto *scene_2_entities = scene_2->entity_collection()->entities();
  REQUIRE(scene_2_entities != nullptr);
  REQUIRE(scene_2_entities->size() == 3);

  // Check Scene 2, Entity 0 (UI inventory)
  const auto *scene_2_entity_0 = scene_2_entities->Get(0);
  REQUIRE(scene_2_entity_0 != nullptr);
  REQUIRE(scene_2_entity_0->c_user_interface() != nullptr);
  REQUIRE(scene_2_entity_0->c_user_interface()->ui_name()->str() ==
          "crafting_ui_inventory");
  REQUIRE(scene_2_entity_0->c_user_interface()->is_visible() == true);

  // Check Scene 2, Entity 1 (UI workbench)
  const auto *scene_2_entity_1 = scene_2_entities->Get(1);
  REQUIRE(scene_2_entity_1 != nullptr);
  REQUIRE(scene_2_entity_1->c_user_interface() != nullptr);
  REQUIRE(scene_2_entity_1->c_user_interface()->ui_name()->str() ==
          "crafting_ui_workbench");
  REQUIRE(scene_2_entity_1->c_user_interface()->is_visible() == true);

  // Check Scene 2, Entity 2 (grimoire machina)
  const auto *scene_2_entity_2 = scene_2_entities->Get(2);
  REQUIRE(scene_2_entity_2 != nullptr);
  REQUIRE(scene_2_entity_2->c_grimoire_machina() != nullptr);
  REQUIRE(scene_2_entity_2->c_grimoire_machina()->fragments() != nullptr);
  REQUIRE(scene_2_entity_2->c_grimoire_machina()->fragments()->size() == 3);
  REQUIRE(scene_2_entity_2->c_grimoire_machina()->fragments()->Get(0)->str() ==
          "crafting_fragment_one");
  REQUIRE(scene_2_entity_2->c_grimoire_machina()->joints() != nullptr);
  REQUIRE(scene_2_entity_2->c_grimoire_machina()->joints()->size() == 2);

  // Verify Scene 3 (second TITLE scene)
  const auto *scene_3 = scenes->Get(2);
  REQUIRE(scene_3 != nullptr);
  REQUIRE(scene_3->scene_info() != nullptr);
  REQUIRE(scene_3->scene_info()->scene_type() == steamrot::SceneTypeFbs_TITLE);
  REQUIRE(scene_3->scene_resources_config() != nullptr);
  REQUIRE(scene_3->scene_resources_config()->texture_width() == 1920);
  REQUIRE(scene_3->scene_resources_config()->texture_height() == 1080);
  REQUIRE(scene_3->entity_collection() != nullptr);
  REQUIRE(scene_3->entity_collection()->entity_memory_pool_size() == 15);

  // Check Scene 3 entities
  const auto *scene_3_entities = scene_3->entity_collection()->entities();
  REQUIRE(scene_3_entities != nullptr);
  REQUIRE(scene_3_entities->size() == 2);

  // Check Scene 3, Entity 0 (UI element)
  const auto *scene_3_entity_0 = scene_3_entities->Get(0);
  REQUIRE(scene_3_entity_0 != nullptr);
  REQUIRE(scene_3_entity_0->c_user_interface() != nullptr);
  REQUIRE(scene_3_entity_0->c_user_interface()->ui_name()->str() ==
          "alternate_title_ui");
  REQUIRE(scene_3_entity_0->c_user_interface()->is_visible() == true);

  // Check Scene 3, Entity 1 (grimoire machina)
  const auto *scene_3_entity_1 = scene_3_entities->Get(1);
  REQUIRE(scene_3_entity_1 != nullptr);
  REQUIRE(scene_3_entity_1->c_grimoire_machina() != nullptr);
  REQUIRE(scene_3_entity_1->c_grimoire_machina()->fragments() != nullptr);
  REQUIRE(scene_3_entity_1->c_grimoire_machina()->fragments()->size() == 2);
  REQUIRE(scene_3_entity_1->c_grimoire_machina()->fragments()->Get(0)->str() ==
          "title_fragment_delta");
  REQUIRE(scene_3_entity_1->c_grimoire_machina()->joints() != nullptr);
  REQUIRE(scene_3_entity_1->c_grimoire_machina()->joints()->size() == 1);

  // Arrange SceneManager
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  steamrot::SceneManager scene_manager(fixture.GetGameContext());

  // Act

  // convert to SceneCollectionData
  steamrot::SceneCollectionData scene_collection_data;
  for (const auto *scene_fbs : *scenes) {

    // Create provider with SceneDataFbs and configure SceneData
    steamrot::FlatbuffersSceneDataProvider scene_data_provider(
        fixture.GetGameContext().event_handler, scene_fbs);

    steamrot::SceneData scene_data;
    auto configure_result = scene_data_provider.ConfigureSceneData(scene_data);
    if (!configure_result.has_value())
      FAIL(configure_result.error().message);

    // add to collection
    scene_collection_data.push_back(std::move(scene_data));
  }

  // pass to SceneManager
  auto scene_collection_add_result =
      scene_manager.AddScenesFromSceneCollectionData(scene_collection_data);
  if (!scene_collection_add_result.has_value()) {
    FAIL(scene_collection_add_result.error().message);
  }
  // Assert
  const auto &scene_map = scene_manager.GetScenes();
  REQUIRE(scene_map.size() == 3);
}
