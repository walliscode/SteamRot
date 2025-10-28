////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of TestFixture class.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "TestFixture.h"
#include "PathProvider.h"

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
  // Set environment type to Test
  m_game_resources.env_type = EnvironmentType::Test;

  // Create window with test settings
  m_game_resources.game_window.create(sf::VideoMode(800, 600),
                                      "SteamRot Test");
  m_game_resources.game_window.setFramerateLimit(60);

  // Initialize other resources (mouse_position and loop_number already have
  // default values)
}

////////////////////////////////////////////////////////////
void TestFixture::ConfigureSceneResources(const SceneType &scene_type) {
  // Create render texture with test dimensions
  if (!m_scene_resources.scene_texture.create(800, 600)) {
    // Handle error - for tests we can throw since this indicates a serious
    // problem
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
