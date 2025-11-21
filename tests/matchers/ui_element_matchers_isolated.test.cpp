/////////////////////////////////////////////////
/// @file
/// @brief Isolated unit tests for UIElement matcher string output
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ui_element_matchers.h"
#include "matcher_test_helpers.h"
#include "PanelElement.h"
#include "ButtonElement.h"
#include "DropDownButtonElement.h"
#include "DropDownListElement.h"
#include "DropDownItemElement.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UIElement matcher describe() output for matching PanelElements",
          "[unit][matchers][UIElement][isolated]") {
  
  steamrot::PanelElement expected;
  expected.position = sf::Vector2f{100.f, 200.f};
  expected.size = sf::Vector2f{50.f, 30.f};
  
  steamrot::PanelElement actual;
  actual.position = sf::Vector2f{100.f, 200.f};
  actual.size = sf::Vector2f{50.f, 30.f};
  
  steamrot::tests::UIElementEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match succeeds when elements are equal") {
    REQUIRE(result.matched);
  }
  
  SECTION("Description contains PASSED indicator") {
    REQUIRE(result.description.find("PASSED") != std::string::npos);
  }
  
  SECTION("Description has proper structure") {
    REQUIRE(steamrot::tests::ValidateOutputStructure(
        result.description, true, true, false));
  }
}

TEST_CASE("UIElement matcher describe() output for position mismatch",
          "[unit][matchers][UIElement][isolated]") {
  
  steamrot::PanelElement expected;
  expected.position = sf::Vector2f{100.f, 200.f};
  
  steamrot::PanelElement actual;
  actual.position = sf::Vector2f{150.f, 200.f};  // Mismatch
  
  steamrot::tests::UIElementEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when position differs") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description contains FAILED indicator") {
    REQUIRE(result.description.find("FAILED") != std::string::npos);
  }
  
  SECTION("Description mentions position") {
    bool has_position = result.description.find("position") != std::string::npos ||
                        result.description.find("100") != std::string::npos ||
                        result.description.find("150") != std::string::npos;
    REQUIRE(has_position);
  }
}

TEST_CASE("UIElement matcher describe() output for size mismatch",
          "[unit][matchers][UIElement][isolated]") {
  
  steamrot::PanelElement expected;
  expected.size = sf::Vector2f{50.f, 30.f};
  
  steamrot::PanelElement actual;
  actual.size = sf::Vector2f{50.f, 40.f};  // Mismatch
  
  steamrot::tests::UIElementEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when size differs") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description mentions size") {
    bool has_size = result.description.find("size") != std::string::npos ||
                    result.description.find("30") != std::string::npos ||
                    result.description.find("40") != std::string::npos;
    REQUIRE(has_size);
  }
}

TEST_CASE("UIElement matcher describe() output for is_mouse_over mismatch",
          "[unit][matchers][UIElement][isolated]") {
  
  steamrot::PanelElement expected;
  expected.is_mouse_over = true;
  
  steamrot::PanelElement actual;
  actual.is_mouse_over = false;  // Mismatch
  
  steamrot::tests::UIElementEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when is_mouse_over differs") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description mentions mouse over state") {
    bool has_mouse_over = result.description.find("mouse") != std::string::npos;
    REQUIRE(has_mouse_over);
  }
}

TEST_CASE("UIElement matcher describe() output for ButtonElement label mismatch",
          "[unit][matchers][UIElement][isolated]") {
  
  steamrot::ButtonElement expected;
  expected.label = "Expected Label";
  
  steamrot::ButtonElement actual;
  actual.label = "Actual Label";  // Mismatch
  
  steamrot::tests::UIElementEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when label differs") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description mentions ButtonElement or label") {
    bool has_button_info = result.description.find("Button") != std::string::npos ||
                           result.description.find("label") != std::string::npos;
    REQUIRE(has_button_info);
  }
  
  SECTION("Description shows both labels") {
    REQUIRE(result.description.find("Expected Label") != std::string::npos);
    REQUIRE(result.description.find("Actual Label") != std::string::npos);
  }
}

TEST_CASE("UIElement matcher describe() output for DropDownButton is_expanded mismatch",
          "[unit][matchers][UIElement][isolated]") {
  
  steamrot::DropDownButtonElement expected;
  expected.is_expanded = true;
  
  steamrot::DropDownButtonElement actual;
  actual.is_expanded = false;  // Mismatch
  
  steamrot::tests::UIElementEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when is_expanded differs") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description mentions expanded state") {
    bool has_expanded = result.description.find("expand") != std::string::npos;
    REQUIRE(has_expanded);
  }
}

TEST_CASE("UIElement matcher describe() output for type mismatch",
          "[unit][matchers][UIElement][isolated]") {
  
  steamrot::ButtonElement expected;
  steamrot::PanelElement actual;
  
  steamrot::tests::UIElementEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when types differ") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description mentions type difference") {
    bool has_type_info = result.description.find("type") != std::string::npos ||
                         result.description.find("Button") != std::string::npos ||
                         result.description.find("Panel") != std::string::npos;
    REQUIRE(has_type_info);
  }
}

TEST_CASE("UIElement matcher describe() output for child count mismatch",
          "[unit][matchers][UIElement][isolated]") {
  
  steamrot::PanelElement expected;
  expected.child_elements.push_back(std::make_unique<steamrot::ButtonElement>());
  
  steamrot::PanelElement actual;
  actual.child_elements.push_back(std::make_unique<steamrot::ButtonElement>());
  actual.child_elements.push_back(std::make_unique<steamrot::ButtonElement>());
  
  steamrot::tests::UIElementEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when child counts differ") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description mentions child elements or count") {
    bool has_child_info = result.description.find("child") != std::string::npos ||
                          result.description.find("1") != std::string::npos ||
                          result.description.find("2") != std::string::npos;
    REQUIRE(has_child_info);
  }
}

TEST_CASE("UIElement matcher describe() output for child element differences",
          "[unit][matchers][UIElement][isolated]") {
  
  auto expected_child = std::make_unique<steamrot::ButtonElement>();
  expected_child->label = "Expected Child";
  
  auto actual_child = std::make_unique<steamrot::ButtonElement>();
  actual_child->label = "Actual Child";
  
  steamrot::PanelElement expected;
  expected.child_elements.push_back(std::move(expected_child));
  
  steamrot::PanelElement actual;
  actual.child_elements.push_back(std::move(actual_child));
  
  steamrot::tests::UIElementEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when child elements differ") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description is hierarchical with child details") {
    // Should have both parent and child information
    bool has_child_details = result.description.find("Child") != std::string::npos;
    REQUIRE(has_child_details);
  }
}

TEST_CASE("UIElement matcher describe() output for nested children",
          "[unit][matchers][UIElement][isolated]") {
  
  SECTION("Match succeeds with identical nested structure") {
    auto expected_grandchild = std::make_unique<steamrot::ButtonElement>();
    expected_grandchild->label = "Grandchild";
    
    auto expected_child = std::make_unique<steamrot::PanelElement>();
    expected_child->child_elements.push_back(std::move(expected_grandchild));
    
    auto actual_grandchild = std::make_unique<steamrot::ButtonElement>();
    actual_grandchild->label = "Grandchild";
    
    auto actual_child = std::make_unique<steamrot::PanelElement>();
    actual_child->child_elements.push_back(std::move(actual_grandchild));
    
    steamrot::PanelElement expected;
    expected.child_elements.push_back(std::move(expected_child));
    
    steamrot::PanelElement actual;
    actual.child_elements.push_back(std::move(actual_child));
    
    steamrot::tests::UIElementEqualsMatcher matcher(expected);
    auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
    
    REQUIRE(result.matched);
  }
  
  SECTION("Match fails with different nested structure") {
    auto expected_grandchild = std::make_unique<steamrot::ButtonElement>();
    expected_grandchild->label = "Expected Grandchild";
    
    auto expected_child = std::make_unique<steamrot::PanelElement>();
    expected_child->child_elements.push_back(std::move(expected_grandchild));
    
    auto actual_grandchild = std::make_unique<steamrot::ButtonElement>();
    actual_grandchild->label = "Actual Grandchild";
    
    auto actual_child = std::make_unique<steamrot::PanelElement>();
    actual_child->child_elements.push_back(std::move(actual_grandchild));
    
    steamrot::PanelElement expected;
    expected.child_elements.push_back(std::move(expected_child));
    
    steamrot::PanelElement actual;
    actual.child_elements.push_back(std::move(actual_child));
    
    steamrot::tests::UIElementEqualsMatcher matcher(expected);
    auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
    
    REQUIRE_FALSE(result.matched);
    REQUIRE_FALSE(result.description.empty());
  }
}

TEST_CASE("UIElement matcher describe() output for multiple field mismatches",
          "[unit][matchers][UIElement][isolated]") {
  
  steamrot::ButtonElement expected;
  expected.position = sf::Vector2f{100.f, 200.f};
  expected.size = sf::Vector2f{50.f, 30.f};
  expected.label = "Expected";
  
  steamrot::ButtonElement actual;
  actual.position = sf::Vector2f{150.f, 250.f};  // Mismatch
  actual.size = sf::Vector2f{60.f, 40.f};        // Mismatch
  actual.label = "Actual";                       // Mismatch
  
  steamrot::tests::UIElementEqualsMatcher matcher(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  SECTION("Match fails when multiple fields differ") {
    REQUIRE_FALSE(result.matched);
  }
  
  SECTION("Description is not empty and contains failure info") {
    REQUIRE_FALSE(result.description.empty());
    REQUIRE(result.description.find("FAILED") != std::string::npos);
  }
}

TEST_CASE("UIElement matcher helper function",
          "[unit][matchers][UIElement][isolated]") {
  
  steamrot::ButtonElement expected;
  expected.label = "Test";
  
  steamrot::ButtonElement actual;
  actual.label = "Test";
  
  auto matcher = steamrot::tests::EqualsUIElement(expected);
  auto result = steamrot::tests::TestMatcherOutput(actual, matcher);
  
  REQUIRE(result.matched);
  REQUIRE_FALSE(result.description.empty());
}
