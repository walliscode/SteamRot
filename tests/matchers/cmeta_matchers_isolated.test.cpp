/////////////////////////////////////////////////
/// @file
/// @brief Isolated unit tests for CMeta matcher string output
///
/// These tests validate matcher string generation without requiring
/// failing test assertions. They test the describe() method output
/// directly using matcher_test_helpers utilities.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "cmeta_matchers.h"
#include "matcher_test_helpers.h"
#include "CMeta.h"
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>

TEST_CASE("CMeta matcher describe() output for matching components",
          "[unit][matchers][CMeta][isolated]") {
  
  steamrot::CMeta expected;
  expected.m_active = true;
  expected.m_entity_active = true;
  
  steamrot::CMeta actual;
  actual.m_active = true;
  actual.m_entity_active = true;
  
  steamrot::tests::CMetaEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match succeeds when components are equal") {
    REQUIRE(result.matched);
  }
  
  SECTION("Description contains PASSED indicator") {
    REQUIRE(result.description.find("PASSED") != std::string::npos);
  }
  
  SECTION("Description contains component name") {
    REQUIRE(result.description.find("CMeta") != std::string::npos);
  }
  
  SECTION("Description has proper structure with dividers") {
    REQUIRE(steamrot::tests::ValidateOutputStructure(
        result.description, true, true, false));
  }
  
  SECTION("Description does not contain field mismatch details") {
    REQUIRE(result.description.find("m_active:") == std::string::npos);
    REQUIRE(result.description.find("m_entity_active:") == std::string::npos);
  }
}

TEST_CASE("CMeta matcher describe() output for m_active mismatch",
          "[unit][matchers][CMeta][isolated]") {
  
  steamrot::CMeta expected;
  expected.m_active = true;
  expected.m_entity_active = true;
  
  steamrot::CMeta actual;
  actual.m_active = false;  // Mismatch
  actual.m_entity_active = true;
  
  steamrot::tests::CMetaEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when m_active differs") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description contains FAILED indicator") {
    REQUIRE(result.description.find("FAILED") != std::string::npos);
  }
  
  SECTION("Description contains m_active field name") {
    REQUIRE(result.description.find("m_active") != std::string::npos);
  }
  
  SECTION("Description contains actual and expected labels") {
    REQUIRE(result.description.find("actual") != std::string::npos);
    REQUIRE(result.description.find("expected") != std::string::npos);
  }
  
  SECTION("Description shows actual value (false)") {
    // Looking for some representation of false
    bool has_false = result.description.find("false") != std::string::npos ||
                     result.description.find("0") != std::string::npos;
    REQUIRE(has_false);
  }
  
  SECTION("Description shows expected value (true)") {
    // Looking for some representation of true
    bool has_true = result.description.find("true") != std::string::npos ||
                    result.description.find("1") != std::string::npos;
    REQUIRE(has_true);
  }
  
  SECTION("Description does not contain m_entity_active (no mismatch)") {
    REQUIRE(result.description.find("m_entity_active") == std::string::npos);
  }
}

TEST_CASE("CMeta matcher describe() output for m_entity_active mismatch",
          "[unit][matchers][CMeta][isolated]") {
  
  steamrot::CMeta expected;
  expected.m_active = true;
  expected.m_entity_active = true;
  
  steamrot::CMeta actual;
  actual.m_active = true;
  actual.m_entity_active = false;  // Mismatch
  
  steamrot::tests::CMetaEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when m_entity_active differs") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description contains m_entity_active field name") {
    REQUIRE(result.description.find("m_entity_active") != std::string::npos);
  }
  
  SECTION("Description does not contain m_active (no mismatch)") {
    // The field name should not appear in the mismatch details
    // Note: it might appear in a header, but not as a mismatch detail
    size_t m_active_count = steamrot::tests::CountSubstring(result.description, "m_active");
    // If m_active appears, it should be minimal (e.g., in the overall matcher name)
    REQUIRE(m_active_count <= 1);
  }
}

TEST_CASE("CMeta matcher describe() output for multiple field mismatches",
          "[unit][matchers][CMeta][isolated]") {
  
  steamrot::CMeta expected;
  expected.m_active = true;
  expected.m_entity_active = true;
  
  steamrot::CMeta actual;
  actual.m_active = false;        // Mismatch
  actual.m_entity_active = false;  // Mismatch
  
  steamrot::tests::CMetaEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when both fields differ") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description contains both field names") {
    std::vector<std::string> expected_fields = {"m_active", "m_entity_active"};
    REQUIRE(steamrot::tests::ContainsAllSubstrings(result.description, expected_fields));
  }
  
  SECTION("Description contains multiple actual/expected pairs") {
    // Should have at least 2 occurrences of "actual" and "expected"
    REQUIRE(steamrot::tests::CountSubstring(result.description, "actual") >= 2);
    REQUIRE(steamrot::tests::CountSubstring(result.description, "expected") >= 2);
  }
  
  SECTION("Description has proper failure structure") {
    REQUIRE(steamrot::tests::ValidateOutputStructure(
        result.description, true, false, true));
  }
}

TEST_CASE("CMeta matcher output format consistency",
          "[unit][matchers][CMeta][isolated]") {
  
  SECTION("Passed output always contains Match keyword") {
    steamrot::CMeta expected;
    expected.m_active = false;
    steamrot::CMeta actual = expected;
    
    steamrot::tests::CMetaEqualsMatcher matcher(expected);
    auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
    
    REQUIRE(result.description.find("Match") != std::string::npos);
  }
  
  SECTION("Failed output always contains Match keyword") {
    steamrot::CMeta expected;
    expected.m_active = true;
    steamrot::CMeta actual;
    actual.m_active = false;
    
    steamrot::tests::CMetaEqualsMatcher matcher(expected);
    auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
    
    REQUIRE(result.description.find("Match") != std::string::npos);
  }
  
  SECTION("Output is non-empty for both passing and failing cases") {
    steamrot::CMeta expected;
    expected.m_active = true;
    
    // Passing case
    steamrot::CMeta actual_pass = expected;
    steamrot::tests::CMetaEqualsMatcher matcher_pass(expected);
    auto result_pass = steamrot::tests::TestMatcherOutput(actual_pass, matcher_pass);
    REQUIRE_FALSE(result_pass.description.empty());
    
    // Failing case
    steamrot::CMeta actual_fail;
    actual_fail.m_active = false;
    steamrot::tests::CMetaEqualsMatcher matcher_fail(expected);
    auto result_fail = steamrot::tests::TestMatcherOutput(actual_fail, matcher_fail);
    REQUIRE_FALSE(result_fail.description.empty());
  }
}
