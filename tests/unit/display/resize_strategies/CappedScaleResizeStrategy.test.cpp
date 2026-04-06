////////////////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the CappedScaleResizeStrategy class.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "CappedScaleResizeStrategy.h"
#include "IResizeStrategy.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>

TEST_CASE("CappedScaleResizeStrategy constructs with a max size",
          "[unit][CappedScaleResizeStrategy]") {
  const sf::Vector2u cap{1920u, 1080u};
  steamrot::CappedScaleResizeStrategy strategy(cap);
  REQUIRE(strategy.GetMaxSize() == cap);
}

TEST_CASE("CappedScaleResizeStrategy is usable through IResizeStrategy pointer",
          "[unit][CappedScaleResizeStrategy]") {
  std::shared_ptr<steamrot::IResizeStrategy> strategy =
      std::make_shared<steamrot::CappedScaleResizeStrategy>(
          sf::Vector2u{1920u, 1080u});
  REQUIRE(strategy != nullptr);
}

TEST_CASE("CappedScaleResizeStrategy::OnResize resizes to new size when below "
          "the cap",
          "[unit][CappedScaleResizeStrategy]") {
  const sf::Vector2u cap{1920u, 1080u};
  steamrot::CappedScaleResizeStrategy strategy(cap);

  sf::RenderTexture texture;
  REQUIRE(texture.resize({800u, 600u}));

  // New size is within the cap on both axes.
  const sf::Vector2u new_size{1024u, 768u};
  strategy.OnResize(new_size, texture);

  REQUIRE(texture.getSize() == new_size);
}

TEST_CASE(
    "CappedScaleResizeStrategy::OnResize clamps to cap when window exceeds it",
    "[unit][CappedScaleResizeStrategy]") {
  const sf::Vector2u cap{1280u, 720u};
  steamrot::CappedScaleResizeStrategy strategy(cap);

  sf::RenderTexture texture;
  REQUIRE(texture.resize({800u, 600u}));

  // New size exceeds the cap on both axes.
  strategy.OnResize({3840u, 2160u}, texture);

  // Texture must not exceed the cap.
  REQUIRE(texture.getSize().x == cap.x);
  REQUIRE(texture.getSize().y == cap.y);
}

TEST_CASE("CappedScaleResizeStrategy::OnResize clamps only the axis that "
          "exceeds its cap",
          "[unit][CappedScaleResizeStrategy]") {
  // Cap only the height, allow width to grow freely (up to a large value).
  const sf::Vector2u cap{4096u, 720u};
  steamrot::CappedScaleResizeStrategy strategy(cap);

  sf::RenderTexture texture;
  REQUIRE(texture.resize({800u, 600u}));

  // Width is within cap, height exceeds cap.
  strategy.OnResize({1600u, 1080u}, texture);

  REQUIRE(texture.getSize().x == 1600u); // width uncapped
  REQUIRE(texture.getSize().y == 720u);  // height clamped
}

TEST_CASE("CappedScaleResizeStrategy::OnResize preserves the existing view",
          "[unit][CappedScaleResizeStrategy]") {
  const sf::Vector2u cap{1920u, 1080u};
  steamrot::CappedScaleResizeStrategy strategy(cap);

  sf::RenderTexture texture;
  REQUIRE(texture.resize({800u, 600u}));

  sf::View custom_view;
  custom_view.setSize({400.f, 300.f});
  custom_view.setCenter({200.f, 150.f});
  texture.setView(custom_view);

  strategy.OnResize({1600u, 900u}, texture);

  const sf::View &after_view = texture.getView();
  REQUIRE(after_view.getSize().x == custom_view.getSize().x);
  REQUIRE(after_view.getSize().y == custom_view.getSize().y);
}
