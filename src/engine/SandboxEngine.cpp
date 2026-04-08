/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the SandboxEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SandboxEngine.h"
#include "Engine.h"
#include "FailInfo.h"
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> SandboxEngine::StartUp() {
  // Run base Engine startup (window, event handler, asset manager, etc.)
  auto base_startup_result = Engine::StartUp();
  if (!base_startup_result.has_value()) {
    return std::unexpected(base_startup_result.error());
  }

  // modify the size of the window for the sandbox
  // m_engine_resources.game_window.setSize({1270, 800});
  // Load the UIExplorer scene instead of the Title scene
  auto load_result = m_scene_manager.LoadUIExplorerScene();
  if (!load_result.has_value()) {
    return std::unexpected(load_result.error());
  }

  return std::monostate{};
}

} // namespace steamrot
