/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the TestFixture class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestFixture.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/VideoMode.hpp>

namespace steamrot::tests {

/////////////////////////////////////////////////
TestFixture::TestFixture() : TestFixture(SceneType::SceneType_TEST) {}

/////////////////////////////////////////////////
TestFixture::TestFixture(SceneType scene_type) : m_scene_type(scene_type) {
  CreateCores();
  CreateContexts();
}

/////////////////////////////////////////////////
TestFixture::~TestFixture() = default;

/////////////////////////////////////////////////
void TestFixture::CreateCores() {
  // Create GameCore with default values
  m_game_core = std::make_unique<GameCore>();

  // Create SceneCore
  m_scene_core = std::make_unique<SceneCore>();

  // Create EntityManager with reference to event handler
  // Use a reasonable default pool size for testing
  constexpr size_t default_pool_size = 100;
  m_entity_manager = std::make_unique<EntityManager>(
      default_pool_size, m_game_core->event_handler);
}

/////////////////////////////////////////////////
void TestFixture::CreateContexts() {
  // Create GameContext from GameCore
  m_game_context = std::make_unique<GameContext>(*m_game_core);

  // Create SceneContext from cores and entity manager
  m_scene_context = std::make_unique<SceneContext>(
      *m_scene_core, *m_game_core, *m_entity_manager);
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
  m_game_core->game_window.create(
      sf::VideoMode({window_width, window_height}), "TestFixture Window",
      sf::Style::Default);

  // Create render texture for the scene
  if (!m_scene_core->scene_texture.resize({window_width, window_height})) {
    // Handle error - for now just continue
  }

  m_initialized = true;
}

/////////////////////////////////////////////////
GameContext &TestFixture::GetGameContext() { return *m_game_context; }

/////////////////////////////////////////////////
SceneContext &TestFixture::GetSceneContext() { return *m_scene_context; }

/////////////////////////////////////////////////
SceneType TestFixture::GetSceneType() const { return m_scene_type; }

} // namespace steamrot::tests
