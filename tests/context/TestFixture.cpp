////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of TestFixture class.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "TestFixture.h"
#include "FlatbuffersConfigurator.h"
#include "PathProvider.h"
#include <iostream>

namespace steamrot::tests {

////////////////////////////////////////////////////////////
TestFixture::TestFixture(const SceneType &scene_type)
    : m_entity_manager(m_game_resources.event_handler),
      m_scene_type(scene_type) {}

void TestFixture::Intialize() {

  PathProvider path_provider{EnvironmentType::Test};
  ConfigureGameResources();
  ConfigureSceneResources(m_scene_type);
}
////////////////////////////////////////////////////////////
void TestFixture::ConfigureGameResources() {
  // Initialize GameResources
  m_game_resources.game_window.create(sf::VideoMode({900, 600}),
                                      "SteamRot Test Window");
  m_game_resources.env_type = EnvironmentType::Test;

  // Load default assets into the asset manager
  auto load_result = m_game_resources.asset_manager.LoadDefaultAssets();
  if (!load_result.has_value()) {
    const FailInfo &error = load_result.error();
    std::cerr << "Error loading default assets: " << error.message << std::endl;
  }
}

////////////////////////////////////////////////////////////
void TestFixture::ConfigureSceneResources(const SceneType &scene_type) {
  // Configure the EntityMemoryPool for the scene
  FlatbuffersConfigurator configurator(m_game_resources.event_handler);

  auto configure_result = configurator.ConfigureEntitiesFromDefaultData(
      m_entity_manager.GetEntityMemoryPool(), scene_type);

  // Check the configuration was successful
  if (!configure_result.has_value()) {
    const FailInfo &error = configure_result.error();
    std::cerr << "Error configuring entities: " << error.message << std::endl;
  }

  // Generate all archetypes for the scene
  auto archetype_result = m_entity_manager.GenerateAllArchetypes();
  if (!archetype_result.has_value()) {
    const FailInfo &error = archetype_result.error();
    std::cerr << "Error generating archetypes: " << error.message << std::endl;
  }
}

////////////////////////////////////////////////////////////
GameResources &TestFixture::GetGameResources() { return m_game_resources; }

////////////////////////////////////////////////////////////
SceneResources &TestFixture::GetSceneResources() { return m_scene_resources; }

////////////////////////////////////////////////////////////
EntityManager &TestFixture::GetEntityManager() { return m_entity_manager; }

////////////////////////////////////////////////////////////
GameContext &TestFixture::GetGameContext() {
  if (!m_game_context) {
    m_game_context = std::make_unique<GameContext>(m_game_resources);
  }
  return *m_game_context;
}

////////////////////////////////////////////////////////////
SceneContext &TestFixture::GetSceneContext() {
  if (!m_scene_context) {
    m_scene_context = std::make_unique<SceneContext>(
        m_scene_resources, m_game_resources, m_entity_manager);
  }
  return *m_scene_context;
}

} // namespace steamrot::tests
