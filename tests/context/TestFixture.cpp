////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of TestFixture class.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "TestFixture.h"
#include "FlatbuffersDataLoader.h"
#include "PathProvider.h"
#include "ResourceConfigurator.h"

namespace steamrot::tests {

////////////////////////////////////////////////////////////
TestFixture::TestFixture(const SceneType &scene_type)
    : m_entity_manager(m_game_resources.event_handler),
      m_scene_type(scene_type) {
  ConfigureGameResources();
  ConfigureSceneResources(scene_type);
}

////////////////////////////////////////////////////////////
void TestFixture::ConfigureGameResources() {
  // Use ResourceConfigurator to configure from test data
  PathProvider path_provider(EnvironmentType::Test);
  FlatbuffersDataLoader loader;
  
  auto context_data_result = loader.ProvideContextData();
  if (context_data_result.has_value()) {
    ResourceConfigurator configurator(context_data_result.value());
    auto config_result = configurator.ConfigureGameResources(m_game_resources);
    
    if (config_result.has_value()) {
      // Configuration successful - resources configured from test data
      return;
    }
  }
  
  // Fallback: If loading/configuration fails, use hardcoded defaults
  m_game_resources.env_type = EnvironmentType::Test;
  sf::Vector2u window_size(800, 600);
  m_game_resources.game_window.create(sf::VideoMode(window_size), "SteamRot Test");
  m_game_resources.game_window.setFramerateLimit(60);
}

////////////////////////////////////////////////////////////
void TestFixture::ConfigureSceneResources(const SceneType &scene_type) {
  // Use ResourceConfigurator to configure from test data
  PathProvider path_provider(EnvironmentType::Test);
  FlatbuffersDataLoader loader;
  
  auto context_data_result = loader.ProvideContextData();
  if (context_data_result.has_value()) {
    ResourceConfigurator configurator(context_data_result.value());
    auto config_result = configurator.ConfigureSceneResources(m_scene_resources, scene_type);
    
    if (config_result.has_value()) {
      // Configuration successful - resources configured from test data
      return;
    }
  }
  
  // Fallback: If loading/configuration fails, use hardcoded defaults
  sf::Vector2u texture_size(800, 600);
  m_scene_resources.scene_texture = sf::RenderTexture(texture_size);
  
  if (!m_scene_resources.scene_texture) {
    throw std::runtime_error("Failed to create test render texture");
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
    m_game_context =
        std::make_unique<GameContext>(m_game_resources.game_window,
                                      m_game_resources.event_handler,
                                      m_game_resources.loop_number,
                                      m_game_resources.asset_manager,
                                      m_game_resources.env_type);
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
