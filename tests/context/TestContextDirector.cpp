/////////////////////////////////////////////////
/// @file
/// @brief Implementation of TestContextDirector and TestResources
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestContextDirector.h"
#include "GameContext.h"
#include "LogicContextBuilder.h"
#include <memory>

namespace steamrot::tests {

/////////////////////////////////////////////////
// Initialize static members
/////////////////////////////////////////////////
std::unique_ptr<TestResources> TestContextDirector::s_resources = nullptr;

/////////////////////////////////////////////////
TestResources::TestResources()
    : render_window(sf::VideoMode({800, 600}), "Test Window"),
      archetype_manager(event_handler) {
  // Initialize render texture
  render_texture.create(800, 600);
}

/////////////////////////////////////////////////
void TestContextDirector::Initialize() {
  if (!s_resources) {
    s_resources = std::make_unique<TestResources>();
  }
}

/////////////////////////////////////////////////
GameContext &TestContextDirector::GetGameContext() {
  Initialize();

  static GameContext game_context(
      s_resources->render_window, s_resources->event_handler,
      s_resources->loop_number, s_resources->asset_manager,
      s_resources->env_type);

  return game_context;
}

/////////////////////////////////////////////////
LogicContextBuilder TestContextDirector::GetLogicContextBuilder() {
  Initialize();

  LogicContextBuilder builder;

  // Configure builder with shared pointers to test resources
  // Note: These use aliasing constructor to share ownership with s_resources
  builder
      .SetSceneEntities(std::shared_ptr<EntityMemoryPool>(
          s_resources, &s_resources->scene_entities))
      .SetArchetypes(std::shared_ptr<const std::unordered_map<ArchetypeID,
                                                                Archetype>>(
          s_resources, &s_resources->archetype_manager.GetArchetypes()))
      .SetSceneTexture(std::shared_ptr<sf::RenderTexture>(
          s_resources, &s_resources->render_texture))
      .SetGameWindow(std::shared_ptr<sf::RenderWindow>(
          s_resources, &s_resources->render_window))
      .SetAssetManager(std::shared_ptr<const AssetManager>(
          s_resources, &s_resources->asset_manager))
      .SetEventHandler(std::shared_ptr<EventHandler>(
          s_resources, &s_resources->event_handler))
      .SetMousePosition(std::shared_ptr<const sf::Vector2i>(
          s_resources, &s_resources->mouse_position));

  return builder;
}

/////////////////////////////////////////////////
void TestContextDirector::Reset() { s_resources.reset(); }

/////////////////////////////////////////////////
bool TestContextDirector::IsInitialized() { return s_resources != nullptr; }

} // namespace steamrot::tests
