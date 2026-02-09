/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for process actions related to the
/// GrimoireMachina.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_grimoire_machina.h"
#include "MachinaForm.h"

namespace steamrot::logic::actions::grimoire_machina {
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
InitialiseActiveMachinaForm(GrimoireMachina &grimoire_machina) {

  // clear the active form if it exists
  if (grimoire_machina.m_active_form)
    grimoire_machina.m_active_form.reset();
  // add a new MachinaForm to the active form
  grimoire_machina.m_active_form = std::make_unique<MachinaForm>();

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ClearActiveMachinaForm(GrimoireMachina &grimoire_machina) {
  // clear the active form if it exists
  if (grimoire_machina.m_active_form)
    grimoire_machina.m_active_form.reset();

  return std::monostate{};
}
} // namespace steamrot::logic::actions::grimoire_machina
