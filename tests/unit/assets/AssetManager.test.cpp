/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for AssetManager class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AssetManager.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("AssetManager can be constructed with a DataAccessFactory reference",
          "[AssetManager]") {

  steamrot::tests::TestFixture test_fixture;

  // Construct AssetManager with the mock DataAccessFactory
  REQUIRE_NOTHROW(steamrot::AssetManager(
      test_fixture.GetGameContext().data_access_factory));
}

TEST_CASE("AssetManager Startup returns success", "[AssetManager]") {

  steamrot::tests::TestFixture test_fixture;
  steamrot::AssetManager asset_manager(
      test_fixture.GetGameContext().data_access_factory);

  auto result = asset_manager.Startup();
  REQUIRE(result.has_value());
}

TEST_CASE(
    "AssetManager::StartUp creates and configures GrimoireMachina instance",
    "[AssetManager]") {

  steamrot::tests::TestFixture test_fixture;
  steamrot::AssetManager asset_manager(
      test_fixture.GetGameContext().data_access_factory);

  auto result = asset_manager.Startup();
  if (!result.has_value()) {
    FAIL("AssetManager::Startup failed with error: " + result.error().message);
  }

  auto get_grimoire_result = asset_manager.GetGrimoireMachina();
  REQUIRE(get_grimoire_result.has_value());

  steamrot::GrimoireMachina *grimoire_machina = get_grimoire_result.value();
  REQUIRE(grimoire_machina != nullptr);

  // check Fragments are initialized
  std::vector<std::string> expected_fragments = {"rectangle"};

  REQUIRE(grimoire_machina->m_all_fragments.size() ==
          expected_fragments.size());
  for (const auto &fragment_name : expected_fragments) {
    REQUIRE(grimoire_machina->m_all_fragments.contains(fragment_name));
  }

  // check Joints are initialized
  std::vector<std::string> expected_joints = {"square"};
  REQUIRE(grimoire_machina->m_all_joints.size() == expected_joints.size());
  for (const auto &joint_name : expected_joints) {
    REQUIRE(grimoire_machina->m_all_joints.contains(joint_name));
  }
}
