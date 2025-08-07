/////////////////////////////////////////////////
/// @file
/// @brief unit tests for FlatbuffersConfigurator
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "FlatbuffersConfigurator.h"
#include "Button.h"
#include "CUserInterface.h"
#include "EntityManager.h"
#include "emp_helpers.h"
#include "user_interface_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Fails if memory pool is not big enough",
          "[FlatbuffersConfigurator]") {

  // create EntityManager with a small memory pool
  steamrot::EntityManager entity_manager{0};

  // create configurator with test environment
  steamrot::FlatbuffersConfigurator configurator{
      steamrot::EnvironmentType::Test};

  auto result = configurator.ConfigureEntitiesFromDefaultData(
      entity_manager.GetEntityMemoryPool(),
      steamrot::SceneType::SceneType_TEST);

  REQUIRE(result.has_value() == false);
  REQUIRE(result.error().mode == steamrot::FailMode::ParameterOutOfBounds);

  // due to changing test data, we will not check the exact size needed
  std::string partial_message = "Entity memory pool size: 0, required size: ";
  REQUIRE(result.error().message.find(partial_message) != std::string::npos);
}

TEST_CASE("Succeeds if memory pool is big enough",
          "[FlatbuffersConfigurator]") {
  // create EntityManager with a big memory pool
  steamrot::EntityManager entity_manager{1000};
  // create configurator with test environment
  steamrot::FlatbuffersConfigurator configurator{
      steamrot::EnvironmentType::Test};
  auto result = configurator.ConfigureEntitiesFromDefaultData(
      entity_manager.GetEntityMemoryPool(),
      steamrot::SceneType::SceneType_TEST);
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
}

TEST_CASE("Data is configured correctly", "[FlatbuffersConfigurator]") {
  // create EntityManager with a big memory pool
  steamrot::EntityManager entity_manager{100};
  // create configurator with test environment
  steamrot::FlatbuffersConfigurator configurator{
      steamrot::EnvironmentType::Test};

  // check Components have default values
  steamrot::EntityMemoryPool &entity_memory_pool_before =
      entity_manager.GetEntityMemoryPool();

  // user interface values
  auto &user_interface_component_vector =
      steamrot::emp_helpers::GetComponentVector<steamrot::CUserInterface>(
          entity_memory_pool_before);

  for (auto &component : user_interface_component_vector) {
    REQUIRE(component.m_active == false);
    REQUIRE(component.UIName == "Default UI");
    REQUIRE(component.m_UI_visible == false);
    REQUIRE(component.m_root_element.name == "UIElement");
    REQUIRE(component.m_root_element.position == sf::Vector2f(0.f, 0.f));
    REQUIRE(component.m_root_element.size == sf::Vector2f(0.f, 0.f));
    REQUIRE(component.m_root_element.spacing_strategy ==
            steamrot::SpacingAndSizingType::SpacingAndSizingType_Even);
    REQUIRE(component.m_root_element.layout ==
            steamrot::LayoutType::LayoutType_Vertical);
    REQUIRE(component.m_root_element.child_elements.empty());
    REQUIRE(component.m_root_element.element_type.index() == 0); // Panel
    REQUIRE(component.m_root_element.mouse_over_child == false);
    REQUIRE(component.m_root_element.mouse_over == false);
  }
  auto result = configurator.ConfigureEntitiesFromDefaultData(
      entity_manager.GetEntityMemoryPool(),
      steamrot::SceneType::SceneType_TEST);
  REQUIRE(result.has_value() == true);
  // Check if entities are configured correctly
  steamrot::EntityMemoryPool &entity_memory_pool_after =
      entity_manager.GetEntityMemoryPool();

  // check CUserInterface component(s)
  auto &user_interface_component_vector_after =
      steamrot::emp_helpers::GetComponentVector<steamrot::CUserInterface>(
          entity_memory_pool_after);

  steamrot::CUserInterface &ui_component_one =
      user_interface_component_vector_after[0];
  REQUIRE(ui_component_one.m_active == true);
  REQUIRE(ui_component_one.UIName == "test_menu");
  REQUIRE(ui_component_one.m_UI_visible == true);

  // check root element
  auto &root_element = ui_component_one.m_root_element;
  REQUIRE(root_element.name == "UIElement");
  REQUIRE(root_element.position.x == 20.f);
  REQUIRE(root_element.position.y == 25.f);
  REQUIRE(root_element.size.x == 220.f);
  REQUIRE(root_element.size.y == 600.f);

  REQUIRE(root_element.spacing_strategy ==
          steamrot::SpacingAndSizingType::SpacingAndSizingType_Ratioed);
  REQUIRE(root_element.layout == steamrot::LayoutType::LayoutType_Horizontal);
  REQUIRE(std::holds_alternative<steamrot::Button>(root_element.element_type));
}
