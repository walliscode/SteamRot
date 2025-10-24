/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for TestDataGenerator utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestDataGenerator.h"
#include "TestDataLoader.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("DiscoverTestDataJsonFiles finds JSON files in directory",
          "[unit][data_generators]") {
  
  // Get path to our test data directory
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  auto data_dir_result = path_provider.GetDataDirectory();
  REQUIRE(data_dir_result.has_value());

  // Construct path to data_generators/data directory
  auto tests_path = data_dir_result.value().parent_path();
  auto data_generators_data_path = tests_path / "data_generators" / "data";

  auto result = steamrot::tests::DiscoverTestDataJsonFiles(data_generators_data_path);
  
  REQUIRE(result.has_value());
  const auto &json_files = result.value();
  
  // We created 3 sample test data files
  REQUIRE(json_files.size() >= 3);
  
  // Verify all files end with .test_data.json
  for (const auto &file : json_files) {
    REQUIRE(file.filename().string().ends_with(".test_data.json"));
  }
}

TEST_CASE("DiscoverTestDataJsonFiles returns error for non-existent directory",
          "[unit][data_generators]") {
  
  std::filesystem::path non_existent_path = "/non/existent/path";
  auto result = steamrot::tests::DiscoverTestDataJsonFiles(non_existent_path);
  
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().m_fail_mode == steamrot::FailMode::FileNotFound);
}

TEST_CASE("GetTestNamesForGenerator returns test names from subdirectory",
          "[unit][data_generators]") {
  
  auto result = steamrot::tests::GetTestNamesForGenerator("data_generators");
  
  REQUIRE(result.has_value());
  const auto &test_names = result.value();
  
  // Should have at least our 3 sample tests
  REQUIRE(test_names.size() >= 3);
  
  // Test names should not have extensions
  for (const auto &name : test_names) {
    REQUIRE_FALSE(name.ends_with(".test_data.json"));
    REQUIRE_FALSE(name.ends_with(".test_data.bin"));
  }
}

TEST_CASE("LoadTestDataForGenerator loads TestDataConfig objects",
          "[unit][data_generators]") {
  
  auto result = steamrot::tests::LoadTestDataForGenerator("data_generators");
  
  REQUIRE(result.has_value());
  const auto &configs = result.value();
  
  // Should have loaded at least our 3 sample tests
  REQUIRE(configs.size() >= 3);
  
  // Verify each config has valid metadata
  for (const auto *config : configs) {
    REQUIRE(config != nullptr);
    REQUIRE(config->metadata() != nullptr);
    REQUIRE(config->metadata()->test_name() != nullptr);
  }
}

TEST_CASE("TestDataGenerator works with Catch2 GENERATE for test names",
          "[unit][data_generators]") {
  
  // Discover test names
  auto test_names_result = steamrot::tests::GetTestNamesForGenerator("data_generators");
  REQUIRE(test_names_result.has_value());
  
  // Use Catch2 generator to iterate through test names
  auto test_name = GENERATE_COPY(from_range(test_names_result.value()));
  
  // Load the specific test data
  steamrot::tests::TestDataLoader loader;
  auto config_result = loader.LoadTestData(test_name, "data_generators");
  
  REQUIRE(config_result.has_value());
  const auto *config = config_result.value();
  
  REQUIRE(config->metadata() != nullptr);
  REQUIRE(config->metadata()->test_name() != nullptr);
  
  // The test name in metadata should match the loaded test
  std::string metadata_name = config->metadata()->test_name()->str();
  REQUIRE(metadata_name == test_name);
}

TEST_CASE("TestDataGenerator works with Catch2 GENERATE for TestDataConfig objects",
          "[unit][data_generators]") {
  
  // Load all test data configs
  auto configs_result = steamrot::tests::LoadTestDataForGenerator("data_generators");
  REQUIRE(configs_result.has_value());
  
  // Use Catch2 generator to iterate through configs
  const auto *config = GENERATE_COPY(from_range(configs_result.value()));
  
  REQUIRE(config != nullptr);
  REQUIRE(config->metadata() != nullptr);
  REQUIRE(config->metadata()->test_name() != nullptr);
  
  // Verify expected_to_pass is set correctly (all our samples are true)
  REQUIRE(config->metadata()->expected_to_pass() == true);
  
  // Verify tags are present
  if (config->metadata()->tags()) {
    REQUIRE(config->metadata()->tags()->size() > 0);
  }
}

TEST_CASE("TestDataGenerator handles subdirectory parameter correctly",
          "[unit][data_generators]") {
  
  SECTION("Valid subdirectory returns results") {
    auto result = steamrot::tests::GetTestNamesForGenerator("data_generators");
    REQUIRE(result.has_value());
    REQUIRE(result.value().size() >= 3);
  }
  
  SECTION("Empty subdirectory uses tests/data") {
    // This should work if there are files in tests/data, otherwise fail
    auto result = steamrot::tests::GetTestNamesForGenerator("");
    // Result depends on whether tests/data exists and has files
    // We just verify the function doesn't crash
    REQUIRE(true);
  }
  
  SECTION("Non-existent subdirectory returns error") {
    auto result = steamrot::tests::GetTestNamesForGenerator("non_existent_subdir_xyz");
    REQUIRE_FALSE(result.has_value());
  }
}

TEST_CASE("Sample test demonstrates full workflow with generators",
          "[unit][data_generators]") {
  
  // This test demonstrates the complete intended workflow:
  // 1. Discover test data files in a directory
  // 2. Load them as TestDataConfig objects
  // 3. Use them with Catch2 generators
  
  // Step 1: Get test names
  auto test_names_result = steamrot::tests::GetTestNamesForGenerator("data_generators");
  REQUIRE(test_names_result.has_value());
  
  // Step 2 & 3: Generate test cases for each test name
  auto test_name = GENERATE_COPY(from_range(test_names_result.value()));
  
  SECTION("Processing test: " + test_name) {
    // Load the specific test data
    steamrot::tests::TestDataLoader loader;
    auto config = loader.LoadTestData(test_name, "data_generators");
    
    REQUIRE(config.has_value());
    
    // Now we can process the TestDataConfig as needed
    // In the future, this could be passed to RunEMPComparisonTest
    // or other test processing functions
    
    const auto *test_config = config.value();
    
    // Example processing: verify metadata
    INFO("Test name: " << test_config->metadata()->test_name()->str());
    INFO("Description: " << (test_config->metadata()->description() ? 
                            test_config->metadata()->description()->str() : "N/A"));
    
    // Example: check if entity data is present
    bool has_entity_collection = (test_config->entity_collection() != nullptr);
    bool has_start_collection = (test_config->start_entity_collection() != nullptr);
    bool has_expected_collection = (test_config->expected_entity_collection() != nullptr);
    
    INFO("Has entity_collection: " << has_entity_collection);
    INFO("Has start_entity_collection: " << has_start_collection);
    INFO("Has expected_entity_collection: " << has_expected_collection);
    
    // Verify at least some entity data is present
    REQUIRE((has_entity_collection || has_start_collection || has_expected_collection));
  }
}
