/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for indentation helper functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "indentation_helpers.h"
#include <catch2/catch_test_macros.hpp>

using namespace steamrot::tests::formatting;

TEST_CASE("conmat::Indent generates correct number of spaces",
          "[unit][formatting]") {
  SECTION("Level 0 produces no spaces") {
    REQUIRE(conmat::Indent(0) == "");
    REQUIRE(conmat::Indent(0).length() == 0);
  }

  SECTION("Level 1 produces 2 spaces by default") {
    REQUIRE(conmat::Indent(1) == "  ");
    REQUIRE(conmat::Indent(1).length() == 2);
  }

  SECTION("Level 2 produces 4 spaces by default") {
    REQUIRE(conmat::Indent(2) == "    ");
    REQUIRE(conmat::Indent(2).length() == 4);
  }

  SECTION("Level 3 produces 6 spaces by default") {
    REQUIRE(conmat::Indent(3) == "      ");
    REQUIRE(conmat::Indent(3).length() == 6);
  }

  SECTION("Custom spaces_per_level works") {
    REQUIRE(conmat::Indent(1, 4) == "    ");
    REQUIRE(conmat::Indent(2, 4) == "        ");
    REQUIRE(conmat::Indent(1, 3) == "   ");
  }

  SECTION("Large indentation levels work") {
    REQUIRE(conmat::Indent(10).length() == 20);
    REQUIRE(conmat::Indent(100).length() == 200);
  }
}

TEST_CASE("IndentedLine formats text with indentation",
          "[unit][formatting]") {
  SECTION("Level 0 adds no indentation") {
    REQUIRE(IndentedLine("Root level", 0) == "Root level");
  }

  SECTION("Level 1 adds 2 spaces") {
    REQUIRE(IndentedLine("First level", 1) == "  First level");
  }

  SECTION("Level 2 adds 4 spaces") {
    REQUIRE(IndentedLine("Second level", 2) == "    Second level");
  }

  SECTION("Level 3 adds 6 spaces") {
    REQUIRE(IndentedLine("Third level", 3) == "      Third level");
  }

  SECTION("Empty text is handled correctly") {
    REQUIRE(IndentedLine("", 0) == "");
    REQUIRE(IndentedLine("", 1) == "  ");
    REQUIRE(IndentedLine("", 2) == "    ");
  }

  SECTION("Custom spaces_per_level works") {
    REQUIRE(IndentedLine("Text", 1, 4) == "    Text");
    REQUIRE(IndentedLine("Text", 2, 3) == "      Text");
  }

  SECTION("Text with special characters is preserved") {
    REQUIRE(IndentedLine("Test: value", 1) == "  Test: value");
    REQUIRE(IndentedLine("m_active = true", 1) == "  m_active = true");
  }
}

TEST_CASE("IndentedKeyValue formats key-value pairs", "[unit][formatting]") {
  SECTION("Boolean values are formatted correctly") {
    REQUIRE(IndentedKeyValue("actual", false, 0) == "actual:   false");
    REQUIRE(IndentedKeyValue("expected", true, 0) == "expected:   true");
    REQUIRE(IndentedKeyValue("actual", false, 1) == "  actual:   false");
    REQUIRE(IndentedKeyValue("expected", true, 2) == "    expected:   true");
  }

  SECTION("Integer values are formatted correctly") {
    REQUIRE(IndentedKeyValue("count", 0, 0) == "count:   0");
    REQUIRE(IndentedKeyValue("count", 42, 1) == "  count:   42");
    REQUIRE(IndentedKeyValue("count", -5, 2) == "    count:   -5");
  }

  SECTION("String values are formatted correctly") {
    REQUIRE(IndentedKeyValue("name", "test", 0) == "name:   test");
    REQUIRE(IndentedKeyValue("name", "test_ui", 1) == "  name:   test_ui");
  }

  SECTION("Floating point values are formatted correctly") {
    REQUIRE(IndentedKeyValue("value", 3.14, 0) == "value:   3.14");
    REQUIRE(IndentedKeyValue("value", 2.5, 1) == "  value:   2.5");
  }

  SECTION("Custom spaces_per_level works") {
    REQUIRE(IndentedKeyValue("key", "value", 1, 4) == "    key:   value");
    REQUIRE(IndentedKeyValue("key", "value", 2, 3) == "      key:   value");
  }

  SECTION("Empty keys and values are handled") {
    REQUIRE(IndentedKeyValue("", "value", 0) == ":   value");
    REQUIRE(IndentedKeyValue("key", "", 0) == "key:   ");
  }

  SECTION("Alignment is consistent") {
    std::string line1 = IndentedKeyValue("actual", true, 2);
    std::string line2 = IndentedKeyValue("expected", false, 2);

    // Both should start with same indentation
    REQUIRE(line1.substr(0, 4) == "    ");
    REQUIRE(line2.substr(0, 4) == "    ");

    // Both should have ":   " after the key
    REQUIRE(line1.find(":   ") != std::string::npos);
    REQUIRE(line2.find(":   ") != std::string::npos);
  }
}

TEST_CASE("Indentation helpers work together", "[unit][formatting]") {
  SECTION("Creating hierarchical output") {
    std::ostringstream oss;

    oss << IndentedLine("Component: CUserInterface", 0) << "\n";
    oss << IndentedKeyValue("m_active", true, 1) << "\n";
    oss << IndentedKeyValue("m_ui_name", "test_ui", 1) << "\n";
    oss << IndentedLine("root_ui_element:", 1) << "\n";
    oss << IndentedKeyValue("position_x", 100, 2) << "\n";
    oss << IndentedKeyValue("position_y", 200, 2) << "\n";

    std::string expected =
        "Component: CUserInterface\n"
        "  m_active:   true\n"
        "  m_ui_name:   test_ui\n"
        "  root_ui_element:\n"
        "    position_x:   100\n"
        "    position_y:   200\n";

    REQUIRE(oss.str() == expected);
  }

  SECTION("Creating deeply nested output") {
    std::ostringstream oss;

    oss << IndentedLine("Level 0", 0) << "\n";
    oss << IndentedLine("Level 1", 1) << "\n";
    oss << IndentedLine("Level 2", 2) << "\n";
    oss << IndentedLine("Level 3", 3) << "\n";
    oss << IndentedLine("Level 4", 4) << "\n";

    std::string expected = "Level 0\n"
                           "  Level 1\n"
                           "    Level 2\n"
                           "      Level 3\n"
                           "        Level 4\n";

    REQUIRE(oss.str() == expected);
  }
}

TEST_CASE("Indentation helpers handle edge cases", "[unit][formatting]") {
  SECTION("Zero spaces_per_level results in no indentation") {
    REQUIRE(conmat::Indent(5, 0) == "");
    REQUIRE(IndentedLine("Text", 5, 0) == "Text");
    REQUIRE(IndentedKeyValue("key", "value", 5, 0) == "key:   value");
  }

  SECTION("Single space per level works") {
    REQUIRE(conmat::Indent(3, 1) == "   ");
    REQUIRE(IndentedLine("Text", 3, 1) == "   Text");
  }
}
