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
  auto fixture_result = steamrot::tests::CreateFixtureFromTestData(config);

  REQUIRE(fixture_result.has_value());

  auto &fixture = fixture_result.value();
}

TEST_CASE("create_fixture_from_test_data rejects null config",
          "[unit][harness]") {

  auto result = steamrot::tests::CreateFixtureFromTestData(nullptr);

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
  auto fixture_result = steamrot::tests::CreateFixtureFromTestData(test_config);
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
  auto result = steamrot::tests::RunFixtureTest(config);

  REQUIRE(result.has_value());
}

TEST_CASE("run_fixture_test works with Catch2 generators", "[unit][harness]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  // Removed console output - let Catch2 control verbosity
  const auto *config = GENERATE_COPY(from_range(configs.value()));

  // This is the main wrapper function for data-driven testing
  auto result = steamrot::tests::RunFixtureTest(config);

  INFO("Test name: " << config->metadata()->test_name()->str());
  if (!result.has_value()) {
    FAIL("Fixture test failed: " << result.error().message);
  }
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
    steamrot::tests::RunEntityMemoryPoolComparisonTest(pool1, pool2,
                                                       "Test: metadata_test");
    SUCCEED("Comparison passed with metadata");
  }

  SECTION("Comparison can be done without metadata") {
    // Test backwards compatibility - should still work without metadata
    steamrot::tests::RunEntityMemoryPoolComparisonTest(pool1, pool2);
    SUCCEED("Comparison passed without metadata");
  }
}

TEST_CASE("run_entity_memory_pool_comparison_test respects expected_to_pass",
          "[unit][harness]") {

  steamrot::EntityMemoryPool pool1;
  steamrot::EntityMemoryPool pool2;

  const size_t num_entities = 3;

  // Setup pool1
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

  // Setup pool2
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

  SECTION("expected_to_pass=true succeeds when pools match") {
    // Pools are equal, test should pass
    steamrot::tests::RunEntityMemoryPoolComparisonTest(pool1, pool2, true);
    SUCCEED("Comparison passed with expected_to_pass=true");
  }

  SECTION("expected_to_pass=false succeeds when pools differ") {
    // Modify pool1 to make it different from pool2
    cmeta_vec1[0].m_active = true;
    cmeta_vec2[0].m_active = false;

    // Pools are different, expected_to_pass=false should succeed
    steamrot::tests::RunEntityMemoryPoolComparisonTest(pool1, pool2, false);
    SUCCEED("Comparison passed with expected_to_pass=false");
  }

  SECTION("expected_to_pass=false with metadata succeeds when pools differ") {
    // Modify pool1 to make it different from pool2
    cui_vec1[1].m_name = "different_name";
    cui_vec2[1].m_name = "original_name";

    // Pools are different, expected_to_pass=false should succeed
    steamrot::tests::RunEntityMemoryPoolComparisonTest(
        pool1, pool2, "Test: mismatch_test", false);
    SUCCEED("Comparison passed with expected_to_pass=false and metadata");
  }
}

TEST_CASE("run_fixture_test handles expected_to_pass from config metadata",
          "[unit][harness]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  SECTION("Test with expected_to_pass=true passes when pools match") {
    // Find a test with expected_to_pass=true
    const steamrot::TestDataConfig *pass_config = nullptr;
    for (const auto *config : configs.value()) {
      if (config->metadata()->expected_to_pass() == true &&
          config->metadata()->test_name()->str() == "sample_test_1") {
        pass_config = config;
        break;
      }
    }
    REQUIRE(pass_config != nullptr);

    auto result = steamrot::tests::RunFixtureTest(pass_config);
    REQUIRE(result.has_value());
  }

  SECTION("Test with expected_to_pass=false passes when pools mismatch") {
    // Find a test with expected_to_pass=false
    const steamrot::TestDataConfig *fail_config = nullptr;
    for (const auto *config : configs.value()) {
      if (config->metadata()->expected_to_pass() == false &&
          config->metadata()->test_name()->str() == "sample_mismatch_test") {
        fail_config = config;
        break;
      }
    }
    REQUIRE(fail_config != nullptr);

    auto result = steamrot::tests::RunFixtureTest(fail_config);
    REQUIRE(result.has_value());
  }
}

TEST_CASE("Metadata-only test data loads successfully", "[unit][harness]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  // Find the metadata_validation test config
  const steamrot::TestDataConfig *metadata_config = nullptr;
  for (const auto *config : configs.value()) {
    if (config->metadata()->test_name()->str() == "metadata_validation") {
      metadata_config = config;
      break;
    }
  }

  REQUIRE(metadata_config != nullptr);

  // Verify metadata fields are correctly loaded
  REQUIRE(metadata_config->metadata()->test_name()->str() ==
          "metadata_validation");
  REQUIRE(metadata_config->metadata()->description() != nullptr);
  REQUIRE(metadata_config->metadata()->description()->str() ==
          "Validates test harness metadata loading without entity collections");
  REQUIRE(metadata_config->metadata()->expected_to_pass() == true);
  REQUIRE(metadata_config->metadata()->version() == 1);

  // Verify optional author field is present
  REQUIRE(metadata_config->metadata()->author() != nullptr);
  REQUIRE(metadata_config->metadata()->author()->str() ==
          "Test Infrastructure Team");

  // Verify tags are correctly loaded
  REQUIRE(metadata_config->metadata()->tags() != nullptr);
  REQUIRE(metadata_config->metadata()->tags()->size() == 3);
  REQUIRE(metadata_config->metadata()->tags()->Get(0)->str() == "unit");
  REQUIRE(metadata_config->metadata()->tags()->Get(1)->str() == "harness");
  REQUIRE(metadata_config->metadata()->tags()->Get(2)->str() ==
          "infrastructure");
}

TEST_CASE("Metadata-only test data has no entity collections",
          "[unit][harness]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  // Find the metadata_validation test config
  const steamrot::TestDataConfig *metadata_config = nullptr;
  for (const auto *config : configs.value()) {
    if (config->metadata()->test_name()->str() == "metadata_validation") {
      metadata_config = config;
      break;
    }
  }

  REQUIRE(metadata_config != nullptr);

  // Verify that entity collections are not present (null)
  REQUIRE(metadata_config->start_entity_collection() == nullptr);
  REQUIRE(metadata_config->expected_entity_collection() == nullptr);

  // Verify other optional fields are also not present
  REQUIRE(metadata_config->game_resources() == nullptr);
  REQUIRE(metadata_config->scene_resources() == nullptr);
  REQUIRE(metadata_config->simulation_data() == nullptr);
  REQUIRE(metadata_config->event_sequence() == nullptr);
  REQUIRE(metadata_config->input_sequence() == nullptr);
}

TEST_CASE("Metadata-only test data can be used with generators",
          "[unit][harness]") {

  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());

  // Filter to only metadata-only tests
  std::vector<const steamrot::TestDataConfig *> metadata_only_configs;
  for (const auto *config : configs.value()) {
    if (config->start_entity_collection() == nullptr &&
        config->expected_entity_collection() == nullptr) {
      metadata_only_configs.push_back(config);
    }
  }

  REQUIRE(metadata_only_configs.size() >= 1);

  // Use generator with metadata-only configs
  const auto *config = GENERATE_COPY(from_range(metadata_only_configs));

  INFO("Test name: " << config->metadata()->test_name()->str());

  // Verify metadata structure
  REQUIRE(config->metadata() != nullptr);
  REQUIRE(config->metadata()->test_name() != nullptr);
  REQUIRE(config->metadata()->version() >= 1);

  // Verify no entity data is present
  REQUIRE(config->start_entity_collection() == nullptr);
  REQUIRE(config->expected_entity_collection() == nullptr);
}

TEST_CASE("RunDataStructComparisonTest returns headers in error messages",
          "[unit][harness]") {}
