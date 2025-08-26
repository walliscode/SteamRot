/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the StylesConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "StylesConfigurator.h"
#include "FlatbuffersDataLoader.h"
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

TEST_CASE("StylesConfigurator ConfigureStyle returns expected result for "
          "default style",
          "[StylesConfigurator]") {

  // set up objects
  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
  steamrot::StylesConfigurator styles_configurator;

  // Load style data
  auto style_data_result = data_loader.ProvideUIStylesData("default");
  if (!style_data_result)
    FAIL(style_data_result.error().message);

  // Get UIStyle object
  auto ui_style_result =
      styles_configurator.ConfigureStyle(*style_data_result.value());
  if (!ui_style_result)
    FAIL(ui_style_result.error().message);

  // compare values between flatbuffer and UIStyle object
  const steamrot::UIStyleData &style_data = *style_data_result.value();
  const steamrot::UIStyle &ui_style = ui_style_result.value();
  REQUIRE(ui_style.panel_style.background_color.r ==
          style_data.panel_style()->style()->background_color()->r());
}
