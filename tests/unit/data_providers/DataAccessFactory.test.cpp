/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the DataAccessFactory class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DataAccessFactory.h"
#include "FlatbuffersEngineDataProvider.h"
#include "FlatbuffersSceneDataProvider.h"
#include "FlatbuffersSceneManagerDataProvider.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("DataAccessFactory Initialization", "[DataAccessFactory]") {

  SECTION("Initialize with default data type(Flatbuffers)") {
    // Expect no exceptions to be thrown
    steamrot::tests::TestFixture test_fixture;
    test_fixture.Initialize();

    // create the factory
    steamrot::DataAccessFactory factory(
        test_fixture.GetGameContext().event_handler);

    // attempt to access the engine data provider
    auto get_engine_provider_result = factory.GetEngineDataProvider();
    if (!get_engine_provider_result.has_value()) {
      FAIL("Failed to get Engine Data Provider: " +
           get_engine_provider_result.error().message);
    }
    auto engine_provider = get_engine_provider_result.value();
    REQUIRE(engine_provider != nullptr);

    // cast to flatbuffers concrete type and check not null
    auto flatbuffers_engine_provider =
        dynamic_cast<steamrot::FlatbuffersEngineDataProvider *>(
            engine_provider);
    REQUIRE(flatbuffers_engine_provider != nullptr);

    // attempt to access the scene manager data provider
    auto get_scene_manager_provider_result =
        factory.GetSceneManagerDataProvider();
    if (!get_scene_manager_provider_result.has_value()) {
      FAIL("Failed to get Scene Manager Data Provider: " +
           get_scene_manager_provider_result.error().message);
    }
    auto scene_manager_provider = get_scene_manager_provider_result.value();
    REQUIRE(scene_manager_provider != nullptr);

    // cast to flatbuffers concrete type and check not null
    auto flatbuffers_scene_manager_provider =
        dynamic_cast<steamrot::FlatbuffersSceneManagerDataProvider *>(
            scene_manager_provider);
    REQUIRE(flatbuffers_scene_manager_provider != nullptr);

    // attempt to access the scene data provider
    auto get_scene_provider_result = factory.GetSceneDataProvider();
    if (!get_scene_provider_result.has_value()) {
      FAIL("Failed to get Scene Data Provider: " +
           get_scene_provider_result.error().message);
    }
    auto scene_provider = get_scene_provider_result.value();
    REQUIRE(scene_provider != nullptr);

    // cast to flatbuffers concrete type and check not null
    auto flatbuffers_scene_provider =
        dynamic_cast<steamrot::FlatbuffersSceneDataProvider *>(
            scene_provider);
    REQUIRE(flatbuffers_scene_provider != nullptr);
  }
}
