/////////////////////////////////////////////////
/// @file
/// @brief Isolated unit tests for EntityMemoryPool matcher string output
///
/// These tests validate EntityMemoryPool matcher string generation
/// without requiring failing test assertions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "entity_memory_pool_matchers.h"
#include "matcher_test_helpers.h"
#include "CMeta.h"
#include "CUserInterface.h"
#include "entity_memory.h"
#include "containers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("EntityMemoryPool matcher describe() output for matching pools",
          "[unit][matchers][EntityMemoryPool][isolated]") {
  
  steamrot::EntityMemoryPool pool1;
  steamrot::EntityMemoryPool pool2;
  
  const size_t num_entities = 3;
  
  // Initialize both pools identically
  auto &cmeta_vec1 = steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool1);
  cmeta_vec1.resize(num_entities);
  auto &cmeta_vec2 = steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool2);
  cmeta_vec2.resize(num_entities);
  
  steamrot::tests::EntityMemoryPoolEqualsMatcher matcher(pool2);
  auto result = steamrot::tests::TestMatcherOutput(pool1, matcher);
  
  SECTION("Match succeeds when pools are identical") {
    REQUIRE(result.matched);
  }
  
  SECTION("Description contains PASSED indicator") {
    REQUIRE(result.description.find("PASSED") != std::string::npos);
  }
  
  SECTION("Description contains EntityMemoryPool identifier") {
    REQUIRE(result.description.find("EntityMemoryPool") != std::string::npos);
  }
  
  SECTION("Description has proper structure") {
    REQUIRE(steamrot::tests::ValidateOutputStructure(
        result.description, true, true, false));
  }
}

TEST_CASE("EntityMemoryPool matcher describe() output for size mismatch",
          "[unit][matchers][EntityMemoryPool][isolated]") {
  
  steamrot::EntityMemoryPool pool1;
  steamrot::EntityMemoryPool pool2;
  
  // Different sizes
  auto &cmeta_vec1 = steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool1);
  cmeta_vec1.resize(5);
  auto &cmeta_vec2 = steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool2);
  cmeta_vec2.resize(3);
  
  steamrot::tests::EntityMemoryPoolEqualsMatcher matcher(pool2);
  auto result = steamrot::tests::TestMatcherOutput(pool1, matcher);
  
  SECTION("Match fails when pool sizes differ") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description contains FAILED indicator") {
    REQUIRE(result.description.find("FAILED") != std::string::npos);
  }
  
  SECTION("Description mentions size mismatch") {
    bool has_size_info = result.description.find("Size") != std::string::npos ||
                         result.description.find("size") != std::string::npos;
    REQUIRE(has_size_info);
  }
  
  SECTION("Description contains actual size value") {
    REQUIRE(result.description.find("5") != std::string::npos);
  }
  
  SECTION("Description contains expected size value") {
    REQUIRE(result.description.find("3") != std::string::npos);
  }
  
  SECTION("Description has actual and expected labels") {
    REQUIRE(result.description.find("actual") != std::string::npos);
    REQUIRE(result.description.find("expected") != std::string::npos);
  }
}

TEST_CASE("EntityMemoryPool matcher describe() output for component value mismatch",
          "[unit][matchers][EntityMemoryPool][isolated]") {
  
  steamrot::EntityMemoryPool pool1;
  steamrot::EntityMemoryPool pool2;
  
  const size_t num_entities = 3;
  
  // Same size but different component values
  auto &cmeta_vec1 = steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool1);
  cmeta_vec1.resize(num_entities);
  cmeta_vec1[0].m_active = true;  // Difference
  
  auto &cmeta_vec2 = steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool2);
  cmeta_vec2.resize(num_entities);
  cmeta_vec2[0].m_active = false;  // Difference
  
  steamrot::tests::EntityMemoryPoolEqualsMatcher matcher(pool2);
  auto result = steamrot::tests::TestMatcherOutput(pool1, matcher);
  
  SECTION("Match fails when component values differ") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description contains component type") {
    REQUIRE(result.description.find("CMeta") != std::string::npos);
  }
  
  SECTION("Description contains field name") {
    REQUIRE(result.description.find("m_active") != std::string::npos);
  }
  
  SECTION("Description is hierarchical (contains both pool and component info)") {
    // Should have EntityMemoryPool info and component-level details
    bool has_pool_info = result.description.find("EntityMemoryPool") != std::string::npos;
    bool has_component_info = result.description.find("CMeta") != std::string::npos;
    REQUIRE(has_pool_info);
    REQUIRE(has_component_info);
  }
}

TEST_CASE("EntityMemoryPool matcher with test metadata",
          "[unit][matchers][EntityMemoryPool][isolated]") {
  
  steamrot::EntityMemoryPool pool1;
  steamrot::EntityMemoryPool pool2;
  
  // Different sizes to trigger failure
  auto &cmeta_vec1 = steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool1);
  cmeta_vec1.resize(5);
  auto &cmeta_vec2 = steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool2);
  cmeta_vec2.resize(3);
  
  std::string test_metadata = "Test: my_test_name";
  steamrot::tests::EntityMemoryPoolEqualsMatcher matcher(pool2, test_metadata);
  auto result = steamrot::tests::TestMatcherOutput(pool1, matcher);
  
  SECTION("Metadata constructor creates valid matcher") {
    // The matcher should still work with metadata
    REQUIRE_FALSE(result.matched);
    REQUIRE_FALSE(result.description.empty());
  }
  
  SECTION("Output still contains standard elements") {
    REQUIRE(result.description.find("Size") != std::string::npos);
    REQUIRE(result.description.find("actual") != std::string::npos);
    REQUIRE(result.description.find("expected") != std::string::npos);
  }
}

TEST_CASE("EntityMemoryPool matcher output format consistency",
          "[unit][matchers][EntityMemoryPool][isolated]") {
  
  SECTION("All outputs contain dividers") {
    steamrot::EntityMemoryPool pool1;
    steamrot::EntityMemoryPool pool2;
    
    auto &cmeta_vec1 = steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool1);
    cmeta_vec1.resize(2);
    auto &cmeta_vec2 = steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool2);
    cmeta_vec2.resize(2);
    
    // Passing case
    steamrot::tests::EntityMemoryPoolEqualsMatcher matcher_pass(pool2);
    auto result_pass = steamrot::tests::TestMatcherOutput(pool1, matcher_pass);
    bool has_dividers_pass = result_pass.description.find("===") != std::string::npos ||
                             result_pass.description.find("---") != std::string::npos;
    REQUIRE(has_dividers_pass);
    
    // Failing case
    cmeta_vec1.resize(5);  // Change size
    steamrot::tests::EntityMemoryPoolEqualsMatcher matcher_fail(pool2);
    auto result_fail = steamrot::tests::TestMatcherOutput(pool1, matcher_fail);
    bool has_dividers_fail = result_fail.description.find("===") != std::string::npos ||
                             result_fail.description.find("---") != std::string::npos;
    REQUIRE(has_dividers_fail);
  }
  
  SECTION("Output is always non-empty") {
    steamrot::EntityMemoryPool pool1;
    steamrot::EntityMemoryPool pool2;
    
    auto &cmeta_vec1 = steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool1);
    cmeta_vec1.resize(2);
    auto &cmeta_vec2 = steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool2);
    cmeta_vec2.resize(2);
    
    steamrot::tests::EntityMemoryPoolEqualsMatcher matcher(pool2);
    auto result = steamrot::tests::TestMatcherOutput(pool1, matcher);
    
    REQUIRE_FALSE(result.description.empty());
    REQUIRE(result.description.length() > 10);  // Should have meaningful content
  }
}

TEST_CASE("EntityMemoryPool matcher helper functions",
          "[unit][matchers][EntityMemoryPool][isolated]") {
  
  SECTION("EqualsEntityMemoryPool helper creates valid matcher") {
    steamrot::EntityMemoryPool pool1;
    steamrot::EntityMemoryPool pool2;
    
    auto &cmeta_vec1 = steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool1);
    cmeta_vec1.resize(2);
    auto &cmeta_vec2 = steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool2);
    cmeta_vec2.resize(2);
    
    auto matcher = steamrot::tests::EqualsEntityMemoryPool(pool2);
    auto result = steamrot::tests::TestMatcherOutput(pool1, matcher);
    
    REQUIRE(result.matched);
    REQUIRE_FALSE(result.description.empty());
  }
  
  SECTION("EqualsEntityMemoryPool with metadata helper creates valid matcher") {
    steamrot::EntityMemoryPool pool1;
    steamrot::EntityMemoryPool pool2;
    
    auto &cmeta_vec1 = steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool1);
    cmeta_vec1.resize(2);
    auto &cmeta_vec2 = steamrot::entity::memory::GetComponentVector<steamrot::CMeta>(pool2);
    cmeta_vec2.resize(2);
    
    auto matcher = steamrot::tests::EqualsEntityMemoryPool(pool2, "Test metadata");
    auto result = steamrot::tests::TestMatcherOutput(pool1, matcher);
    
    REQUIRE(result.matched);
    REQUIRE_FALSE(result.description.empty());
  }
}
