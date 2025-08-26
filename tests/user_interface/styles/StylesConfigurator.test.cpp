/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the StylesConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "StylesConfigurator.h"
#include "PathProvider.h"
#include <catch2/catch_test_macros.hpp>
#include <vector>

TEST_CASE("StylesConfigurator can be instantiated", "[StylesConfigurator]") {
  // Arrange & Act
  steamrot::StylesConfigurator stylesConfigurator;

  SUCCEED("StylesConfigurator instantiated successfully");
}

TEST_CASE("StylesConfigurator GetAllStyleNames returns expected result",
          "[StylesConfigurator]") {
  // Arrange
  // preset PathProvider
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::StylesConfigurator stylesConfigurator;
  // Act
  auto result = stylesConfigurator.GetAllStyleNames();
  // Assert
  if (!result)
    FAIL("GetAllStyleNames failed: " + result.error().message);

  std::vector<std::string> style_names{"default"};
  REQUIRE(result->size() == style_names.size());
  for (const auto &name : style_names) {
    REQUIRE(std::find(result->begin(), result->end(), name) != result->end());
  }
}
