/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the TestFixture class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestFixture.h"
#include "GameContext.h"
#include "SceneContext.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/VideoMode.hpp>

namespace steamrot::tests {

/////////////////////////////////////////////////
TestFixture::TestFixture() : TestFixture(SceneType::TEST) {}

/////////////////////////////////////////////////
TestFixture::TestFixture(SceneType scene_type) : m_scene_type(scene_type) {
  CreateCores();
  CreateContexts();
}

/////////////////////////////////////////////////
TestFixture::~TestFixture() = default;

/////////////////////////////////////////////////
void TestFixture::CreateCores() {

  // Create EntityManager with default pool size for testing
  // The entity manager will create its own internal entity memory pool
  m_entity_manager = std::make_unique<EntityManager>();
}

/////////////////////////////////////////////////
void TestFixture::CreateContexts() {
  // Create GameContext from engine resources
  m_game_context = std::make_unique<GameContext>(m_engine_resources);

  // Create SceneContext from scene texture, engine resources, and entity
  // manager
  m_scene_context = std::make_unique<SceneContext>(
      m_scene_texture, m_engine_resources, *m_entity_manager,
      m_engine_resources.data_access_factory);
}

/////////////////////////////////////////////////
void TestFixture::Initialize() {
  if (m_initialized) {
    return;
  }

  // Create the window with a small size for testing
  // Window is created but not displayed in headless test environments
  constexpr unsigned int window_width = 800;
  constexpr unsigned int window_height = 600;

  // Initialize the game window in engine resources
  m_engine_resources.game_window.create(
      sf::VideoMode({window_width, window_height}), "Test Window");

  // Resize the render texture to match window size
  auto resize_result = m_scene_texture.resize({window_width, window_height});

  m_initialized = true;
}

/////////////////////////////////////////////////
GameContext &TestFixture::GetGameContext() { return *m_game_context; }

/////////////////////////////////////////////////
SceneContext &TestFixture::GetSceneContext() { return *m_scene_context; }

/////////////////////////////////////////////////
SceneType TestFixture::GetSceneType() const { return m_scene_type; }

} // namespace steamrot::tests
