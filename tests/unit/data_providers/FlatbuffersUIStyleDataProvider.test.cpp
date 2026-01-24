/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersUIStyleDataProvider
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersUIStyleDataProvider.h"
#include <SFML/Graphics/Font.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <unordered_map>

/////////////////////////////////////////////////
// Constructor and Basic tests
/////////////////////////////////////////////////

TEST_CASE("FlatbuffersUIStyleDataProvider is constructed correctly",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);
  REQUIRE_NOTHROW(provider);
}

TEST_CASE("FlatbuffersUIStyleDataProvider::CreateUIStyles loads correctly",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  // Load fonts that are expected in the data
  auto font1 = std::make_shared<sf::Font>();
  auto font2 = std::make_shared<sf::Font>();
  fonts_map["DaddyTimeMonoNerdFont-Regular"] = font1;
  fonts_map["Roboto-Regular"] = font2;

  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);
  auto result = provider.CreateUIStyles();

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &styles = result.value();
  REQUIRE(styles.size() == 1);
  REQUIRE(styles[0].name == "default");
}

TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigureUIStyles configures "
          "correctly",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  // Load fonts that are expected in the data
  auto font1 = std::make_shared<sf::Font>();
  auto font2 = std::make_shared<sf::Font>();
  fonts_map["DaddyTimeMonoNerdFont-Regular"] = font1;
  fonts_map["Roboto-Regular"] = font2;

  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);
  std::vector<steamrot::UIStyle> styles;

  auto result = provider.ConfigureUIStyles(styles);

  REQUIRE(result.has_value());
  REQUIRE(styles.size() == 1);
  REQUIRE(styles[0].name == "default");
}

TEST_CASE("FlatbuffersUIStyleDataProvider::ProvideUIStyles loads correctly "
          "(backward compatibility)",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  // Load fonts that are expected in the data
  auto font1 = std::make_shared<sf::Font>();
  auto font2 = std::make_shared<sf::Font>();
  fonts_map["DaddyTimeMonoNerdFont-Regular"] = font1;
  fonts_map["Roboto-Regular"] = font2;

  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);
  auto result = provider.ProvideUIStyles();

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &styles = result.value();
  REQUIRE(styles.size() == 1);
  REQUIRE(styles[0].name == "default");
}
