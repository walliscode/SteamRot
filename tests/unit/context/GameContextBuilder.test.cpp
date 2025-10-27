/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for GameContextBuilder class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GameContextBuilder.h"
#include "AssetManager.h"
#include "EventHandler.h"
#include "PathProvider.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>

TEST_CASE("GameContextBuilder validates required fields",
          "[unit][context][GameContextBuilder]") {
  steamrot::GameContextBuilder builder;
  auto result = builder.Build();

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingRequiredField);
}

TEST_CASE("GameContextBuilder fails when window is missing",
          "[unit][context][GameContextBuilder]") {
  steamrot::GameContextBuilder builder;

  auto event_handler = std::make_shared<steamrot::EventHandler>();
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  auto asset_manager = std::make_shared<steamrot::AssetManager>();
  auto loop_num = std::make_shared<const size_t>(0);

  builder.SetEventHandler(event_handler)
      .SetAssetManager(asset_manager)
      .SetLoopNumber(loop_num)
      .SetEnvironmentType(steamrot::EnvironmentType::Test);

  auto result = builder.Build();

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingRequiredField);
  REQUIRE(result.error().message == "Window is required");
}

TEST_CASE("GameContextBuilder fails when event handler is missing",
          "[unit][context][GameContextBuilder]") {
  steamrot::GameContextBuilder builder;

  auto window = std::make_shared<sf::RenderWindow>();
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  auto asset_manager = std::make_shared<steamrot::AssetManager>();
  auto loop_num = std::make_shared<const size_t>(0);

  builder.SetWindow(window)
      .SetAssetManager(asset_manager)
      .SetLoopNumber(loop_num)
      .SetEnvironmentType(steamrot::EnvironmentType::Test);

  auto result = builder.Build();

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingRequiredField);
  REQUIRE(result.error().message == "EventHandler is required");
}

TEST_CASE("GameContextBuilder fails when asset manager is missing",
          "[unit][context][GameContextBuilder]") {
  steamrot::GameContextBuilder builder;

  auto window = std::make_shared<sf::RenderWindow>();
  auto event_handler = std::make_shared<steamrot::EventHandler>();
  auto loop_num = std::make_shared<const size_t>(0);

  builder.SetWindow(window)
      .SetEventHandler(event_handler)
      .SetLoopNumber(loop_num)
      .SetEnvironmentType(steamrot::EnvironmentType::Test);

  auto result = builder.Build();

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingRequiredField);
  REQUIRE(result.error().message == "AssetManager is required");
}

TEST_CASE("GameContextBuilder fails when loop number is missing",
          "[unit][context][GameContextBuilder]") {
  steamrot::GameContextBuilder builder;

  auto window = std::make_shared<sf::RenderWindow>();
  auto event_handler = std::make_shared<steamrot::EventHandler>();
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  auto asset_manager = std::make_shared<steamrot::AssetManager>();

  builder.SetWindow(window)
      .SetEventHandler(event_handler)
      .SetAssetManager(asset_manager)
      .SetEnvironmentType(steamrot::EnvironmentType::Test);

  auto result = builder.Build();

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingRequiredField);
  REQUIRE(result.error().message == "LoopNumber is required");
}

TEST_CASE("GameContextBuilder fails when environment type is missing",
          "[unit][context][GameContextBuilder]") {
  steamrot::GameContextBuilder builder;

  auto window = std::make_shared<sf::RenderWindow>();
  auto event_handler = std::make_shared<steamrot::EventHandler>();
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  auto asset_manager = std::make_shared<steamrot::AssetManager>();
  auto loop_num = std::make_shared<const size_t>(0);

  builder.SetWindow(window)
      .SetEventHandler(event_handler)
      .SetAssetManager(asset_manager)
      .SetLoopNumber(loop_num);

  auto result = builder.Build();

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingRequiredField);
  REQUIRE(result.error().message == "EnvironmentType is required");
}

TEST_CASE("GameContextBuilder builds successfully with all fields",
          "[unit][context][GameContextBuilder]") {
  steamrot::GameContextBuilder builder;

  auto window = std::make_shared<sf::RenderWindow>();
  auto event_handler = std::make_shared<steamrot::EventHandler>();
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  auto asset_manager = std::make_shared<steamrot::AssetManager>();
  auto loop_num = std::make_shared<const size_t>(0);

  auto result = builder.SetWindow(window)
                     .SetEventHandler(event_handler)
                     .SetAssetManager(asset_manager)
                     .SetLoopNumber(loop_num)
                     .SetEnvironmentType(steamrot::EnvironmentType::Test)
                     .Build();

  REQUIRE(result.has_value());
  REQUIRE(&result.value().game_window == window.get());
  REQUIRE(&result.value().event_handler == event_handler.get());
  REQUIRE(&result.value().asset_manager == asset_manager.get());
  REQUIRE(&result.value().loop_number == loop_num.get());
  REQUIRE(result.value().env_type == steamrot::EnvironmentType::Test);
}

TEST_CASE("GameContextBuilder supports method chaining",
          "[unit][context][GameContextBuilder]") {
  steamrot::GameContextBuilder builder;

  auto window = std::make_shared<sf::RenderWindow>();
  auto event_handler = std::make_shared<steamrot::EventHandler>();
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  auto asset_manager = std::make_shared<steamrot::AssetManager>();
  auto loop_num = std::make_shared<const size_t>(42);

  // Test that each setter returns a reference for chaining
  auto &builder_ref = builder.SetWindow(window);
  REQUIRE(&builder_ref == &builder);

  auto &builder_ref2 = builder_ref.SetEventHandler(event_handler);
  REQUIRE(&builder_ref2 == &builder);

  auto &builder_ref3 = builder_ref2.SetAssetManager(asset_manager);
  REQUIRE(&builder_ref3 == &builder);

  auto &builder_ref4 = builder_ref3.SetLoopNumber(loop_num);
  REQUIRE(&builder_ref4 == &builder);

  auto &builder_ref5 = builder_ref4.SetEnvironmentType(steamrot::EnvironmentType::Test);
  REQUIRE(&builder_ref5 == &builder);
}
