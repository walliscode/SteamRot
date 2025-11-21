/////////////////////////////////////////////////
/// @file
/// @brief Isolated unit tests for CUserInterface matcher string output
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "cuser_interface_matchers.h"
#include "matcher_test_helpers.h"
#include "CUserInterface.h"
#include "PanelElement.h"
#include "ButtonElement.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CUserInterface matcher describe() output for matching components",
          "[unit][matchers][CUserInterface][isolated]") {
  
  steamrot::CUserInterface expected;
  expected.m_name = "TestUI";
  expected.m_UI_visible = true;
  expected.m_root_element = nullptr;
  
  steamrot::CUserInterface actual;
  actual.m_name = "TestUI";
  actual.m_UI_visible = true;
  actual.m_root_element = nullptr;
  
  steamrot::tests::CUserInterfaceEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match succeeds when components are equal") {
    REQUIRE(result.matched);
  }
  
  SECTION("Description contains PASSED indicator") {
    REQUIRE(result.description.find("PASSED") != std::string::npos);
  }
  
  SECTION("Description contains component name") {
    REQUIRE(result.description.find("CUserInterface") != std::string::npos);
  }
  
  SECTION("Description has proper structure") {
    REQUIRE(steamrot::tests::ValidateOutputStructure(
        result.description, true, true, false));
  }
}

TEST_CASE("CUserInterface matcher describe() output for m_name mismatch",
          "[unit][matchers][CUserInterface][isolated]") {
  
  steamrot::CUserInterface expected;
  expected.m_name = "ExpectedUI";
  expected.m_UI_visible = true;
  
  steamrot::CUserInterface actual;
  actual.m_name = "ActualUI";  // Mismatch
  actual.m_UI_visible = true;
  
  steamrot::tests::CUserInterfaceEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when m_name differs") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description contains FAILED indicator") {
    REQUIRE(result.description.find("FAILED") != std::string::npos);
  }
  
  SECTION("Description contains field name") {
    REQUIRE(result.description.find("m_name") != std::string::npos);
  }
  
  SECTION("Description shows both actual and expected names") {
    REQUIRE(result.description.find("ExpectedUI") != std::string::npos);
    REQUIRE(result.description.find("ActualUI") != std::string::npos);
  }
}

TEST_CASE("CUserInterface matcher describe() output for m_UI_visible mismatch",
          "[unit][matchers][CUserInterface][isolated]") {
  
  steamrot::CUserInterface expected;
  expected.m_name = "TestUI";
  expected.m_UI_visible = true;
  
  steamrot::CUserInterface actual;
  actual.m_name = "TestUI";
  actual.m_UI_visible = false;  // Mismatch
  
  steamrot::tests::CUserInterfaceEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when m_UI_visible differs") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description contains field name") {
    REQUIRE(result.description.find("m_UI_visible") != std::string::npos);
  }
  
  SECTION("Description contains actual and expected labels") {
    REQUIRE(result.description.find("actual") != std::string::npos);
    REQUIRE(result.description.find("expected") != std::string::npos);
  }
}

TEST_CASE("CUserInterface matcher describe() output for root_element null mismatch",
          "[unit][matchers][CUserInterface][isolated]") {
  
  SECTION("Expected has element, actual is null") {
    steamrot::CUserInterface expected;
    expected.m_name = "TestUI";
    expected.m_root_element = std::make_unique<steamrot::PanelElement>();
    
    steamrot::CUserInterface actual;
    actual.m_name = "TestUI";
    actual.m_root_element = nullptr;  // Mismatch
    
    steamrot::tests::CUserInterfaceEqualsMatcher matcher(expected);
    auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
    
    REQUIRE_FALSE(result.matched);
    REQUIRE(result.description.find("m_root_element") != std::string::npos);
    REQUIRE(result.description.find("nullptr") != std::string::npos);
  }
  
  SECTION("Expected is null, actual has element") {
    steamrot::CUserInterface expected;
    expected.m_name = "TestUI";
    expected.m_root_element = nullptr;
    
    steamrot::CUserInterface actual;
    actual.m_name = "TestUI";
    actual.m_root_element = std::make_unique<steamrot::PanelElement>();  // Mismatch
    
    steamrot::tests::CUserInterfaceEqualsMatcher matcher(expected);
    auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
    
    REQUIRE_FALSE(result.matched);
    REQUIRE(result.description.find("m_root_element") != std::string::npos);
  }
}

TEST_CASE("CUserInterface matcher describe() output for root_element field mismatch",
          "[unit][matchers][CUserInterface][isolated]") {
  
  auto expected_panel = std::make_unique<steamrot::PanelElement>();
  expected_panel->position = sf::Vector2f{100.f, 200.f};
  
  auto actual_panel = std::make_unique<steamrot::PanelElement>();
  actual_panel->position = sf::Vector2f{150.f, 200.f};  // Different position
  
  steamrot::CUserInterface expected;
  expected.m_name = "TestUI";
  expected.m_root_element = std::move(expected_panel);
  
  steamrot::CUserInterface actual;
  actual.m_name = "TestUI";
  actual.m_root_element = std::move(actual_panel);
  
  steamrot::tests::CUserInterfaceEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when root element fields differ") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description contains root element indicator") {
    REQUIRE(result.description.find("m_root_element") != std::string::npos);
  }
  
  SECTION("Description is hierarchical with UIElement details") {
    // Should contain both CUserInterface level and UIElement level info
    bool has_ui_info = result.description.find("CUserInterface") != std::string::npos ||
                       result.description.find("m_root_element") != std::string::npos;
    REQUIRE(has_ui_info);
  }
}

TEST_CASE("CUserInterface matcher describe() output for multiple field mismatches",
          "[unit][matchers][CUserInterface][isolated]") {
  
  steamrot::CUserInterface expected;
  expected.m_name = "ExpectedUI";
  expected.m_UI_visible = true;
  expected.m_root_element = nullptr;
  
  steamrot::CUserInterface actual;
  actual.m_name = "ActualUI";        // Mismatch
  actual.m_UI_visible = false;       // Mismatch
  actual.m_root_element = nullptr;
  
  steamrot::tests::CUserInterfaceEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when multiple fields differ") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description contains all mismatched field names") {
    REQUIRE(result.description.find("m_name") != std::string::npos);
    REQUIRE(result.description.find("m_UI_visible") != std::string::npos);
  }
  
  SECTION("Description contains multiple actual/expected pairs") {
    size_t actual_count = steamrot::tests::CountSubstring(result.description, "actual");
    size_t expected_count = steamrot::tests::CountSubstring(result.description, "expected");
    REQUIRE(actual_count >= 2);
    REQUIRE(expected_count >= 2);
  }
}

TEST_CASE("CUserInterface matcher with complex root elements",
          "[unit][matchers][CUserInterface][isolated]") {
  
  SECTION("Match succeeds with identical complex trees") {
    auto expected_panel = std::make_unique<steamrot::PanelElement>();
    expected_panel->position = sf::Vector2f{10.f, 10.f};
    auto expected_child = std::make_unique<steamrot::ButtonElement>();
    expected_child->label = "Button";
    expected_panel->child_elements.push_back(std::move(expected_child));
    
    auto actual_panel = std::make_unique<steamrot::PanelElement>();
    actual_panel->position = sf::Vector2f{10.f, 10.f};
    auto actual_child = std::make_unique<steamrot::ButtonElement>();
    actual_child->label = "Button";
    actual_panel->child_elements.push_back(std::move(actual_child));
    
    steamrot::CUserInterface expected;
    expected.m_name = "TestUI";
    expected.m_root_element = std::move(expected_panel);
    
    steamrot::CUserInterface actual;
    actual.m_name = "TestUI";
    actual.m_root_element = std::move(actual_panel);
    
    steamrot::tests::CUserInterfaceEqualsMatcher matcher(expected);
    auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
    
    REQUIRE(result.matched);
  }
  
  SECTION("Match fails with different child elements") {
    auto expected_panel = std::make_unique<steamrot::PanelElement>();
    auto expected_child = std::make_unique<steamrot::ButtonElement>();
    expected_child->label = "Expected Button";
    expected_panel->child_elements.push_back(std::move(expected_child));
    
    auto actual_panel = std::make_unique<steamrot::PanelElement>();
    auto actual_child = std::make_unique<steamrot::ButtonElement>();
    actual_child->label = "Actual Button";
    actual_panel->child_elements.push_back(std::move(actual_child));
    
    steamrot::CUserInterface expected;
    expected.m_name = "TestUI";
    expected.m_root_element = std::move(expected_panel);
    
    steamrot::CUserInterface actual;
    actual.m_name = "TestUI";
    actual.m_root_element = std::move(actual_panel);
    
    steamrot::tests::CUserInterfaceEqualsMatcher matcher(expected);
    auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
    
    REQUIRE_FALSE(result.matched);
    REQUIRE_FALSE(result.description.empty());
  }
}

TEST_CASE("CUserInterface matcher helper function",
          "[unit][matchers][CUserInterface][isolated]") {
  
  steamrot::CUserInterface expected;
  expected.m_name = "TestUI";
  
  steamrot::CUserInterface actual;
  actual.m_name = "TestUI";
  
  auto matcher = steamrot::tests::EqualsCUserInterface(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  REQUIRE(result.matched);
  REQUIRE_FALSE(result.description.empty());
}
