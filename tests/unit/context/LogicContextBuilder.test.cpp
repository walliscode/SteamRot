/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for LogicContextBuilder class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "LogicContextBuilder.h"
#include "ArchetypeManager.h"
#include "AssetManager.h"
#include "EventHandler.h"
#include "PathProvider.h"
#include "containers.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>

TEST_CASE("LogicContextBuilder validates required fields",
          "[unit][logic][LogicContextBuilder]") {
  steamrot::LogicContextBuilder builder;
  auto result = builder.Build();

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingRequiredField);
}

TEST_CASE("LogicContextBuilder fails when scene entities is missing",
          "[unit][logic][LogicContextBuilder]") {
  steamrot::LogicContextBuilder builder;

  auto archetypes = std::make_shared<
      const std::unordered_map<steamrot::ArchetypeID, steamrot::Archetype>>();
  auto scene_texture = std::make_shared<sf::RenderTexture>();
  auto game_window = std::make_shared<sf::RenderWindow>();
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  auto asset_manager = std::make_shared<const steamrot::AssetManager>();
  auto event_handler = std::make_shared<steamrot::EventHandler>();
  auto mouse_position = std::make_shared<const sf::Vector2i>(0, 0);

  builder.SetArchetypes(archetypes)
      .SetSceneTexture(scene_texture)
      .SetGameWindow(game_window)
      .SetAssetManager(asset_manager)
      .SetEventHandler(event_handler)
      .SetMousePosition(mouse_position);

  auto result = builder.Build();

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingRequiredField);
  REQUIRE(result.error().message == "SceneEntities is required");
}

TEST_CASE("LogicContextBuilder fails when archetypes is missing",
          "[unit][logic][LogicContextBuilder]") {
  steamrot::LogicContextBuilder builder;

  auto scene_entities = std::make_shared<steamrot::EntityMemoryPool>();
  auto scene_texture = std::make_shared<sf::RenderTexture>();
  auto game_window = std::make_shared<sf::RenderWindow>();
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  auto asset_manager = std::make_shared<const steamrot::AssetManager>();
  auto event_handler = std::make_shared<steamrot::EventHandler>();
  auto mouse_position = std::make_shared<const sf::Vector2i>(0, 0);

  builder.SetSceneEntities(scene_entities)
      .SetSceneTexture(scene_texture)
      .SetGameWindow(game_window)
      .SetAssetManager(asset_manager)
      .SetEventHandler(event_handler)
      .SetMousePosition(mouse_position);

  auto result = builder.Build();

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingRequiredField);
  REQUIRE(result.error().message == "Archetypes is required");
}

TEST_CASE("LogicContextBuilder fails when scene texture is missing",
          "[unit][logic][LogicContextBuilder]") {
  steamrot::LogicContextBuilder builder;

  auto scene_entities = std::make_shared<steamrot::EntityMemoryPool>();
  auto archetypes = std::make_shared<
      const std::unordered_map<steamrot::ArchetypeID, steamrot::Archetype>>();
  auto game_window = std::make_shared<sf::RenderWindow>();
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  auto asset_manager = std::make_shared<const steamrot::AssetManager>();
  auto event_handler = std::make_shared<steamrot::EventHandler>();
  auto mouse_position = std::make_shared<const sf::Vector2i>(0, 0);

  builder.SetSceneEntities(scene_entities)
      .SetArchetypes(archetypes)
      .SetGameWindow(game_window)
      .SetAssetManager(asset_manager)
      .SetEventHandler(event_handler)
      .SetMousePosition(mouse_position);

  auto result = builder.Build();

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingRequiredField);
  REQUIRE(result.error().message == "SceneTexture is required");
}

TEST_CASE("LogicContextBuilder fails when game window is missing",
          "[unit][logic][LogicContextBuilder]") {
  steamrot::LogicContextBuilder builder;

  auto scene_entities = std::make_shared<steamrot::EntityMemoryPool>();
  auto archetypes = std::make_shared<
      const std::unordered_map<steamrot::ArchetypeID, steamrot::Archetype>>();
  auto scene_texture = std::make_shared<sf::RenderTexture>();
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  auto asset_manager = std::make_shared<const steamrot::AssetManager>();
  auto event_handler = std::make_shared<steamrot::EventHandler>();
  auto mouse_position = std::make_shared<const sf::Vector2i>(0, 0);

  builder.SetSceneEntities(scene_entities)
      .SetArchetypes(archetypes)
      .SetSceneTexture(scene_texture)
      .SetAssetManager(asset_manager)
      .SetEventHandler(event_handler)
      .SetMousePosition(mouse_position);

  auto result = builder.Build();

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingRequiredField);
  REQUIRE(result.error().message == "GameWindow is required");
}

TEST_CASE("LogicContextBuilder fails when asset manager is missing",
          "[unit][logic][LogicContextBuilder]") {
  steamrot::LogicContextBuilder builder;

  auto scene_entities = std::make_shared<steamrot::EntityMemoryPool>();
  auto archetypes = std::make_shared<
      const std::unordered_map<steamrot::ArchetypeID, steamrot::Archetype>>();
  auto scene_texture = std::make_shared<sf::RenderTexture>();
  auto game_window = std::make_shared<sf::RenderWindow>();
  auto event_handler = std::make_shared<steamrot::EventHandler>();
  auto mouse_position = std::make_shared<const sf::Vector2i>(0, 0);

  builder.SetSceneEntities(scene_entities)
      .SetArchetypes(archetypes)
      .SetSceneTexture(scene_texture)
      .SetGameWindow(game_window)
      .SetEventHandler(event_handler)
      .SetMousePosition(mouse_position);

  auto result = builder.Build();

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingRequiredField);
  REQUIRE(result.error().message == "AssetManager is required");
}

TEST_CASE("LogicContextBuilder fails when event handler is missing",
          "[unit][logic][LogicContextBuilder]") {
  steamrot::LogicContextBuilder builder;

  auto scene_entities = std::make_shared<steamrot::EntityMemoryPool>();
  auto archetypes = std::make_shared<
      const std::unordered_map<steamrot::ArchetypeID, steamrot::Archetype>>();
  auto scene_texture = std::make_shared<sf::RenderTexture>();
  auto game_window = std::make_shared<sf::RenderWindow>();
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  auto asset_manager = std::make_shared<const steamrot::AssetManager>();
  auto mouse_position = std::make_shared<const sf::Vector2i>(0, 0);

  builder.SetSceneEntities(scene_entities)
      .SetArchetypes(archetypes)
      .SetSceneTexture(scene_texture)
      .SetGameWindow(game_window)
      .SetAssetManager(asset_manager)
      .SetMousePosition(mouse_position);

  auto result = builder.Build();

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingRequiredField);
  REQUIRE(result.error().message == "EventHandler is required");
}

TEST_CASE("LogicContextBuilder fails when mouse position is missing",
          "[unit][logic][LogicContextBuilder]") {
  steamrot::LogicContextBuilder builder;

  auto scene_entities = std::make_shared<steamrot::EntityMemoryPool>();
  auto archetypes = std::make_shared<
      const std::unordered_map<steamrot::ArchetypeID, steamrot::Archetype>>();
  auto scene_texture = std::make_shared<sf::RenderTexture>();
  auto game_window = std::make_shared<sf::RenderWindow>();
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  auto asset_manager = std::make_shared<const steamrot::AssetManager>();
  auto event_handler = std::make_shared<steamrot::EventHandler>();

  builder.SetSceneEntities(scene_entities)
      .SetArchetypes(archetypes)
      .SetSceneTexture(scene_texture)
      .SetGameWindow(game_window)
      .SetAssetManager(asset_manager)
      .SetEventHandler(event_handler);

  auto result = builder.Build();

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingRequiredField);
  REQUIRE(result.error().message == "MousePosition is required");
}

TEST_CASE("LogicContextBuilder builds successfully with all fields",
          "[unit][logic][LogicContextBuilder]") {
  steamrot::LogicContextBuilder builder;

  auto scene_entities = std::make_shared<steamrot::EntityMemoryPool>();
  auto archetypes = std::make_shared<
      const std::unordered_map<steamrot::ArchetypeID, steamrot::Archetype>>();
  auto scene_texture = std::make_shared<sf::RenderTexture>();
  auto game_window = std::make_shared<sf::RenderWindow>();
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  auto asset_manager = std::make_shared<const steamrot::AssetManager>();
  auto event_handler = std::make_shared<steamrot::EventHandler>();
  auto mouse_position = std::make_shared<const sf::Vector2i>(100, 200);

  auto result = builder.SetSceneEntities(scene_entities)
                    .SetArchetypes(archetypes)
                    .SetSceneTexture(scene_texture)
                    .SetGameWindow(game_window)
                    .SetAssetManager(asset_manager)
                    .SetEventHandler(event_handler)
                    .SetMousePosition(mouse_position)
                    .Build();

  REQUIRE(result.has_value());
  REQUIRE(&result.value().scene_entities == scene_entities.get());
  REQUIRE(&result.value().archetypes == archetypes.get());
  REQUIRE(&result.value().scene_texture == scene_texture.get());
  REQUIRE(&result.value().game_window == game_window.get());
  REQUIRE(&result.value().asset_manager == asset_manager.get());
  REQUIRE(&result.value().event_handler == event_handler.get());
  REQUIRE(&result.value().mouse_position == mouse_position.get());
}

TEST_CASE("LogicContextBuilder supports method chaining",
          "[unit][logic][LogicContextBuilder]") {
  steamrot::LogicContextBuilder builder;

  auto scene_entities = std::make_shared<steamrot::EntityMemoryPool>();
  auto archetypes = std::make_shared<
      const std::unordered_map<steamrot::ArchetypeID, steamrot::Archetype>>();
  auto scene_texture = std::make_shared<sf::RenderTexture>();
  auto game_window = std::make_shared<sf::RenderWindow>();
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  auto asset_manager = std::make_shared<const steamrot::AssetManager>();
  auto event_handler = std::make_shared<steamrot::EventHandler>();
  auto mouse_position = std::make_shared<const sf::Vector2i>(0, 0);

  // Test that each setter returns a reference for chaining
  auto &builder_ref = builder.SetSceneEntities(scene_entities);
  REQUIRE(&builder_ref == &builder);

  auto &builder_ref2 = builder_ref.SetArchetypes(archetypes);
  REQUIRE(&builder_ref2 == &builder);

  auto &builder_ref3 = builder_ref2.SetSceneTexture(scene_texture);
  REQUIRE(&builder_ref3 == &builder);

  auto &builder_ref4 = builder_ref3.SetGameWindow(game_window);
  REQUIRE(&builder_ref4 == &builder);

  auto &builder_ref5 = builder_ref4.SetAssetManager(asset_manager);
  REQUIRE(&builder_ref5 == &builder);

  auto &builder_ref6 = builder_ref5.SetEventHandler(event_handler);
  REQUIRE(&builder_ref6 == &builder);

  auto &builder_ref7 = builder_ref6.SetMousePosition(mouse_position);
  REQUIRE(&builder_ref7 == &builder);
}
