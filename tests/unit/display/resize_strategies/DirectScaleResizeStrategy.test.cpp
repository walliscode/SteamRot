////////////////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the DirectScaleResizeStrategy class.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "DirectScaleResizeStrategy.h"
#include "IResizeStrategy.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>

TEST_CASE("DirectScaleResizeStrategy default-constructs without error",
          "[unit][DirectScaleResizeStrategy]") {
  steamrot::DirectScaleResizeStrategy strategy;
  SUCCEED("DirectScaleResizeStrategy default-constructed successfully");
}

TEST_CASE("DirectScaleResizeStrategy is usable through IResizeStrategy pointer",
          "[unit][DirectScaleResizeStrategy]") {
  std::shared_ptr<steamrot::IResizeStrategy> strategy =
      std::make_shared<steamrot::DirectScaleResizeStrategy>();
  REQUIRE(strategy != nullptr);
}

TEST_CASE("DirectScaleResizeStrategy::OnResize resizes the render texture to "
          "the new window size",
          "[unit][DirectScaleResizeStrategy]") {
  steamrot::DirectScaleResizeStrategy strategy;

  sf::RenderTexture texture;
  REQUIRE(texture.resize({800u, 600u}));

  const sf::Vector2u new_size{1024u, 768u};
  strategy.OnResize(new_size, texture);

  // The texture should now match the requested new size.
  REQUIRE(texture.getSize() == new_size);
}

TEST_CASE("DirectScaleResizeStrategy::OnResize preserves the existing view",
          "[unit][DirectScaleResizeStrategy]") {
  steamrot::DirectScaleResizeStrategy strategy;

  sf::RenderTexture texture;
  REQUIRE(texture.resize({800u, 600u}));

  // Set a custom view so we can verify it is preserved after resize.
  sf::View custom_view;
  custom_view.setSize({400.f, 300.f});
  custom_view.setCenter({200.f, 150.f});
  texture.setView(custom_view);

  strategy.OnResize({1600u, 1200u}, texture);

  // The custom view should be restored (same world area, now in more pixels).
  const sf::View &after_view = texture.getView();
  REQUIRE(after_view.getSize().x == custom_view.getSize().x);
  REQUIRE(after_view.getSize().y == custom_view.getSize().y);
  REQUIRE(after_view.getCenter().x == custom_view.getCenter().x);
  REQUIRE(after_view.getCenter().y == custom_view.getCenter().y);
}

TEST_CASE("DirectScaleResizeStrategy::OnResize handles shrinking the window",
          "[unit][DirectScaleResizeStrategy]") {
  steamrot::DirectScaleResizeStrategy strategy;

  sf::RenderTexture texture;
  REQUIRE(texture.resize({1920u, 1080u}));

  const sf::Vector2u smaller_size{640u, 480u};
  strategy.OnResize(smaller_size, texture);

  REQUIRE(texture.getSize() == smaller_size);
}
