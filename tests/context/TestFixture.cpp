/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the TestFixture class
/////////////////////////////////////////////////

#include "TestFixture.h"
#include "FlatbuffersConfigurator.h"
#include "PathProvider.h"
#include <SFML/Graphics/VideoMode.hpp>
#include <iostream>

namespace steamrot::tests {

/////////////////////////////////////////////////
TestFixture::TestFixture(const SceneType &scene_type) {
  // Set the path provider for test environment
  PathProvider path_provider{EnvironmentType::Test};

  // Configure resources
  ConfigureGameResources();
  ConfigureSceneResources(scene_type);

  // Create contexts
  m_game_context = std::make_unique<GameContext>(m_game_resources);
  m_scene_context =
      std::make_unique<SceneContext>(m_scene_resources, m_game_resources);
}

/////////////////////////////////////////////////
void TestFixture::ConfigureGameResources() {
  // Configure game window
  m_game_resources.game_window.create(sf::VideoMode({900, 600}),
                                      "SteamRot Test Window");

  // Set environment type
  m_game_resources.env_type = EnvironmentType::Test;

  // Load default assets
  auto load_result = m_game_resources.asset_manager.LoadDefaultAssets();
  if (!load_result.has_value()) {
    const FailInfo &error = load_result.error();
    std::cerr << "Error loading default assets: " << error.message
              << std::endl;
  }

  std::cout << "TestFixture: GameResources configured" << std::endl;
}

/////////////////////////////////////////////////
void TestFixture::ConfigureSceneResources(const SceneType &scene_type) {
  // Configure entities for the scene
  FlatbuffersConfigurator configurator(m_game_resources.event_handler);

  std::cout << "TestFixture: Configuring entities for scene type "
            << static_cast<int>(scene_type) << std::endl;

  auto configure_result = configurator.ConfigureEntitiesFromDefaultData(
      m_scene_resources.scene_entities, scene_type);

  if (!configure_result.has_value()) {
    const FailInfo &error = configure_result.error();
    std::cerr << "Error configuring entities: " << error.message << std::endl;
  }

  // Generate archetypes
  auto archetype_result =
      m_scene_resources.archetype_manager.GenerateAllArchetypes();
  if (!archetype_result.has_value()) {
    const FailInfo &error = archetype_result.error();
    std::cerr << "Error generating archetypes: " << error.message << std::endl;
  }

  std::cout << "TestFixture: SceneResources configured" << std::endl;
}

/////////////////////////////////////////////////
GameResources &TestFixture::GetGameResources() { return m_game_resources; }

/////////////////////////////////////////////////
SceneResources &TestFixture::GetSceneResources() { return m_scene_resources; }

/////////////////////////////////////////////////
GameContext &TestFixture::GetGameContext() {
  if (!m_game_context) {
    throw std::runtime_error("GameContext not initialized in TestFixture");
  }
  return *m_game_context;
}

/////////////////////////////////////////////////
SceneContext &TestFixture::GetSceneContext() {
  if (!m_scene_context) {
    throw std::runtime_error("SceneContext not initialized in TestFixture");
  }
  return *m_scene_context;
}

} // namespace steamrot::tests
