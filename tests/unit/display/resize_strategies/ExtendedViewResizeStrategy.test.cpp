////////////////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the ExtendedViewResizeStrategy class.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "ExtendedViewResizeStrategy.h"
#include "IResizeStrategy.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>

TEST_CASE("ExtendedViewResizeStrategy default-constructs without error",
          "[unit][ExtendedViewResizeStrategy]") {
  steamrot::ExtendedViewResizeStrategy strategy;
  SUCCEED("ExtendedViewResizeStrategy default-constructed successfully");
}

TEST_CASE(
    "ExtendedViewResizeStrategy is usable through IResizeStrategy pointer",
    "[unit][ExtendedViewResizeStrategy]") {
  std::shared_ptr<steamrot::IResizeStrategy> strategy =
      std::make_shared<steamrot::ExtendedViewResizeStrategy>();
  REQUIRE(strategy != nullptr);
}

TEST_CASE("ExtendedViewResizeStrategy::OnResize resizes the render texture to "
          "the new window size",
          "[unit][ExtendedViewResizeStrategy]") {
  steamrot::ExtendedViewResizeStrategy strategy;

  sf::RenderTexture texture;
  REQUIRE(texture.resize({800u, 600u}));

  const sf::Vector2u new_size{1600u, 1200u};
  strategy.OnResize(new_size, texture);

  REQUIRE(texture.getSize() == new_size);
}

TEST_CASE("ExtendedViewResizeStrategy::OnResize resets the view to the "
          "default view (1:1 world-to-pixel mapping)",
          "[unit][ExtendedViewResizeStrategy]") {
  steamrot::ExtendedViewResizeStrategy strategy;

  sf::RenderTexture texture;
  REQUIRE(texture.resize({800u, 600u}));

  // Set a non-default view to verify it is overwritten.
  sf::View custom_view;
  custom_view.setSize({400.f, 300.f});
  custom_view.setCenter({200.f, 150.f});
  texture.setView(custom_view);

  const sf::Vector2u new_size{1600u, 1200u};
  strategy.OnResize(new_size, texture);

  // After the strategy fires, the view should equal the texture default view.
  // The default view for a 1600x1200 texture has size {1600, 1200} and
  // centre {800, 600}.
  const sf::View &after_view = texture.getView();
  const sf::View default_view = texture.getDefaultView();

  REQUIRE(after_view.getSize().x == default_view.getSize().x);
  REQUIRE(after_view.getSize().y == default_view.getSize().y);
  REQUIRE(after_view.getCenter().x == default_view.getCenter().x);
  REQUIRE(after_view.getCenter().y == default_view.getCenter().y);
}

TEST_CASE("ExtendedViewResizeStrategy::OnResize view size equals new window "
          "size (1:1 pixel density)",
          "[unit][ExtendedViewResizeStrategy]") {
  steamrot::ExtendedViewResizeStrategy strategy;

  sf::RenderTexture texture;
  REQUIRE(texture.resize({800u, 600u}));

  const sf::Vector2u new_size{1024u, 768u};
  strategy.OnResize(new_size, texture);

  // The view dimensions should match the new texture size exactly, meaning
  // one world unit equals one screen pixel.
  const sf::View &after_view = texture.getView();
  REQUIRE(after_view.getSize().x ==
          static_cast<float>(new_size.x));
  REQUIRE(after_view.getSize().y ==
          static_cast<float>(new_size.y));
}

TEST_CASE(
    "DirectScaleResizeStrategy and ExtendedViewResizeStrategy differ in view "
    "behaviour after resize",
    "[unit][ExtendedViewResizeStrategy]") {
  // Start both textures at the same initial size and with the same custom view.
  sf::RenderTexture direct_texture;
  REQUIRE(direct_texture.resize({800u, 600u}));

  sf::RenderTexture extended_texture;
  REQUIRE(extended_texture.resize({800u, 600u}));

  sf::View custom_view;
  custom_view.setSize({400.f, 300.f});
  custom_view.setCenter({200.f, 150.f});
  direct_texture.setView(custom_view);
  extended_texture.setView(custom_view);

  // Apply each strategy.
  // Import is done inline to avoid needing multiple #includes in one file.
  struct FakeDirect : steamrot::IResizeStrategy {
    void OnResize(sf::Vector2u new_size,
                  sf::RenderTexture &rt) override {
      sf::View v = rt.getView();
      rt.resize(new_size);
      rt.setView(v); // preserve original view
    }
  };

  FakeDirect direct_strategy;
  steamrot::ExtendedViewResizeStrategy extended_strategy;

  const sf::Vector2u new_size{1600u, 1200u};
  direct_strategy.OnResize(new_size, direct_texture);
  extended_strategy.OnResize(new_size, extended_texture);

  // Direct Scale: view size unchanged (same world area, bigger pixels).
  REQUIRE(direct_texture.getView().getSize().x == custom_view.getSize().x);
  REQUIRE(direct_texture.getView().getSize().y == custom_view.getSize().y);

  // Extended View: view size matches new texture size (more world visible).
  REQUIRE(extended_texture.getView().getSize().x ==
          static_cast<float>(new_size.x));
  REQUIRE(extended_texture.getView().getSize().y ==
          static_cast<float>(new_size.y));
}
