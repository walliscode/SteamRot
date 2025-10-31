/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for test data harness utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "test_data_harness.h"
#include "CGrimoireMachina.h"
#include "CMachinaForm.h"
#include "CMeta.h"
#include "CUIState.h"
#include "CUserInterface.h"
#include "entity_memory.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("load_test_data_configs loads from adjacent data directory",
          "[unit][harness]") {

  auto result = steamrot::tests::load_test_data_configs();

  REQUIRE(result.has_value());
  const auto &configs = result.value();

  // Should have at least our sample test data files
  REQUIRE(configs.size() >= 3);

  // Verify each config has valid metadata
  for (const auto *config : configs) {
    REQUIRE(config != nullptr);
    REQUIRE(config->metadata() != nullptr);
    REQUIRE(config->metadata()->test_name() != nullptr);
  }
}

TEST_CASE("load_test_data_configs works with Catch2 generators",
          "[unit][harness]") {

  // Load all test data configs
  auto configs_result = steamrot::tests::load_test_data_configs();
  REQUIRE(configs_result.has_value());

  // Use Catch2 generator to iterate through configs
  const auto *config = GENERATE_COPY(from_range(configs_result.value()));

  REQUIRE(config != nullptr);
  REQUIRE(config->metadata() != nullptr);
  REQUIRE(config->metadata()->test_name() != nullptr);

  // Verify expected_to_pass is set
  REQUIRE(config->metadata()->expected_to_pass() == true);

  // Verify at least some entity data is present
  bool has_entity_data = (config->start_entity_collection() != nullptr ||
                          config->expected_entity_collection() != nullptr);
  REQUIRE(has_entity_data);
}

TEST_CASE("load_test_data_configs with subdirectory parameter",
          "[unit][harness]") {

  SECTION("Valid subdirectory returns results") {
    auto result = steamrot::tests::load_test_data_configs("harness");
    REQUIRE(result.has_value());
    REQUIRE(result.value().size() >= 3);
  }

  SECTION("Non-existent subdirectory returns error") {
    auto result =
        steamrot::tests::load_test_data_configs("non_existent_subdir_xyz");
    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().mode == steamrot::FailMode::FileNotFound);
  }
}

TEST_CASE("Test data harness demonstrates simple workflow", "[unit][harness]") {

  // Simple one-line call to load all test data
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  // Use with Catch2 generator for parameterized testing
  const auto *config = GENERATE_COPY(from_range(configs.value()));

  // Process the test data
  INFO("Test name: " << config->metadata()->test_name()->str());

  if (config->metadata()->description()) {
    INFO("Description: " << config->metadata()->description()->str());
  }

  // Verify the config has expected structure
  REQUIRE(config->metadata()->version() >= 1);
}

TEST_CASE("create_fixture_from_test_data creates initialized fixture",
          "[unit][harness]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  REQUIRE(configs.value().size() >= 1);

  const auto *config = configs.value()[0];

  // Create fixture from test data
  auto fixture_result = steamrot::tests::create_fixture_from_test_data(config);

  REQUIRE(fixture_result.has_value());

  auto &fixture = fixture_result.value();
}

TEST_CASE("create_fixture_from_test_data rejects null config",
          "[unit][harness]") {

  auto result = steamrot::tests::create_fixture_from_test_data(nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("create_fixture_from_test_data does not load default scene entities",
          "[unit][harness]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  REQUIRE(configs.value().size() >= 1);

  // Find sample_test_1 config (only has c_user_interface, no
  // c_grimoire_machina)
  const steamrot::TestDataConfig *test_config = nullptr;
  for (const auto *config : configs.value()) {
    if (config->metadata()->test_name()->str() == "sample_test_1") {
      test_config = config;
      break;
    }
  }
  REQUIRE(test_config != nullptr);

  // Create fixture from test data
  auto fixture_result =
      steamrot::tests::create_fixture_from_test_data(test_config);
  REQUIRE(fixture_result.has_value());

  auto &fixture = fixture_result.value();
  const auto &pool = fixture.GetEntityManager().GetEntityMemoryPool();

  // Get component vectors
  const auto &grimoire_vec =
      steamrot::entity::memory::GetComponentVector<steamrot::CGrimoireMachina>(
          pool);

  // Verify that CGrimoireMachina components are NOT active
  // (i.e., they weren't loaded from default scene data)
  for (size_t i = 0; i < grimoire_vec.size(); ++i) {
    // All CGrimoireMachina components should be inactive since sample_test_1
    // doesn't define any c_grimoire_machina
    REQUIRE_FALSE(grimoire_vec[i].m_active);
    REQUIRE(grimoire_vec[i].m_all_fragments.empty());
  }
}

TEST_CASE("run_fixture_test executes comparison test", "[unit][harness]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  REQUIRE(configs.value().size() >= 1);

  const auto *config = configs.value()[0];

  // Run fixture test with comparison
  auto result = steamrot::tests::run_fixture_test(config);

  REQUIRE(result.has_value());
}

TEST_CASE("run_fixture_test works with Catch2 generators", "[unit][harness]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  const auto *config = GENERATE_COPY(from_range(configs.value()));

  // This is the main wrapper function for data-driven testing
  auto result = steamrot::tests::run_fixture_test(config);

  INFO("Test name: " << config->metadata()->test_name()->str());
  REQUIRE(result.has_value());
}

TEST_CASE("run_entity_memory_pool_comparison_test with metadata",
          "[unit][harness]") {

  steamrot::EntityMemoryPool pool1;
  steamrot::EntityMemoryPool pool2;

  // Create pools with same structure
  const size_t num_entities = 3;

  auto &cmeta_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool1);
  cmeta_vec1.resize(num_entities);

  auto &cui_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUserInterface>(
          pool1);
  cui_vec1.resize(num_entities);

  auto &cform_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMachinaForm>(
          pool1);
  cform_vec1.resize(num_entities);

  auto &cgrim_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CGrimoireMachina>(
          pool1);
  cgrim_vec1.resize(num_entities);

  auto &cstate_vec1 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUIState>(pool1);
  cstate_vec1.resize(num_entities);

  auto &cmeta_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool2);
  cmeta_vec2.resize(num_entities);

  auto &cui_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUserInterface>(
          pool2);
  cui_vec2.resize(num_entities);

  auto &cform_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CMachinaForm>(
          pool2);
  cform_vec2.resize(num_entities);

  auto &cgrim_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CGrimoireMachina>(
          pool2);
  cgrim_vec2.resize(num_entities);

  auto &cstate_vec2 =
      steamrot::entity::memory::GetComponentVector<steamrot::CUIState>(pool2);
  cstate_vec2.resize(num_entities);

  SECTION("Comparison succeeds with equal pools") {
    // Pools are equal, test should pass
    steamrot::tests::run_entity_memory_pool_comparison_test(
        pool1, pool2, "Test: metadata_test");
    SUCCEED("Comparison passed with metadata");
  }

  SECTION("Comparison can be done without metadata") {
    // Test backwards compatibility - should still work without metadata
    steamrot::tests::run_entity_memory_pool_comparison_test(pool1, pool2);
    SUCCEED("Comparison passed without metadata");
  }
}
