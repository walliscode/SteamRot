/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EntityConfigurationFactory
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EntityConfigurationFactory.h"
#include "CGrimoireMachina.h"
#include "FlatbuffersDataLoader.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

steamrot::FlatbuffersDataLoader data_loader(steamrot::EnvironmentType::Test);
steamrot::EntityConfigurationFactory
    factory(std::make_shared<steamrot::FlatbuffersDataLoader>(data_loader));

TEST_CASE("EntityConfigurationFactory configures CGrimoireMachina",
          "[EntityConfigurationFactory], [CGrimoireMachina]") {

  steamrot::CGrimoireMachina grimoire;
  // Test pre-configuration state
  REQUIRE(grimoire.m_active == false);
  REQUIRE(grimoire.m_all_fragments.empty());
  REQUIRE(grimoire.m_all_joints.empty());
  REQUIRE(grimoire.m_machina_forms.empty());
  REQUIRE(grimoire.m_holding_form == nullptr);
  REQUIRE(grimoire.GetComponentRegisterIndex() == 3);
  REQUIRE(grimoire.Name() == "CGrimoireMachina");
  // Configure the component using the factory
  factory.ConfigureComponent(grimoire);
  // Test post-configuration state
  REQUIRE(grimoire.m_active == true);
  REQUIRE(grimoire.m_all_fragments.size() == 1);
}
