/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the abstract Engine base class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Engine.h"

namespace steamrot {

/////////////////////////////////////////////////
Engine::Engine(EnvironmentType env_type)
    : m_game_context(m_game_resources), m_running(false) {
  m_game_resources.env_type = env_type;
  m_game_resources.loop_number = 1;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> Engine::StartUp() {
  // Derived classes implement ConfigureFromData()
  return ConfigureFromData();
}

/////////////////////////////////////////////////
void Engine::Run(size_t num_ticks) {
  m_running = true;
  size_t ticks_executed = 0;

  while (m_running && ShouldContinueRunning()) {
    ExecuteTick();
    OnPostTick();

    ticks_executed++;

    // If num_ticks is specified, stop after that many ticks
    if (num_ticks > 0 && ticks_executed >= num_ticks) {
      break;
    }
  }
}

/////////////////////////////////////////////////
bool Engine::ShouldContinueRunning() const {
  // Default implementation checks if the window is open
  return m_game_resources.game_window.isOpen();
}

/////////////////////////////////////////////////
void Engine::OnPostTick() { m_game_resources.loop_number++; }

/////////////////////////////////////////////////
void Engine::Stop() { m_running = false; }

/////////////////////////////////////////////////
size_t Engine::GetLoopNumber() const { return m_game_resources.loop_number; }

/////////////////////////////////////////////////
GameResources &Engine::GetGameResources() { return m_game_resources; }

/////////////////////////////////////////////////
const GameResources &Engine::GetGameResources() const {
  return m_game_resources;
}

/////////////////////////////////////////////////
GameContext &Engine::GetGameContext() { return m_game_context; }

/////////////////////////////////////////////////
const GameContext &Engine::GetGameContext() const { return m_game_context; }

} // namespace steamrot
