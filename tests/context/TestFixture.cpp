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

  // Create EntityManager with reference to event handler
  // Use a reasonable default pool size for testing
  constexpr size_t default_pool_size = 100;
}

/////////////////////////////////////////////////
void TestFixture::CreateContexts() {}

/////////////////////////////////////////////////
void TestFixture::Initialize() {
  if (m_initialized) {
    return;
  }

  // Create the window with a small size for testing
  // Window is created but not displayed in headless test environments
  constexpr unsigned int window_width = 800;
  constexpr unsigned int window_height = 600;

  m_initialized = true;
}

/////////////////////////////////////////////////
GameContext &TestFixture::GetGameContext() { return *m_game_context; }

/////////////////////////////////////////////////
SceneContext &TestFixture::GetSceneContext() { return *m_scene_context; }

/////////////////////////////////////////////////
SceneType TestFixture::GetSceneType() const { return m_scene_type; }

} // namespace steamrot::tests
