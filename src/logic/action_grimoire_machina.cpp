/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for process actions related to the
/// GrimoireMachina.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_grimoire_machina.h"
#include "MachinaFormScaffold.h"

namespace steamrot::logic::actions::grimoire_machina {
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
InitialiseActiveMachinaFormScaffold(GrimoireMachina &grimoire_machina) {

  // clear the active form if it exists
  if (grimoire_machina.m_scaffold_form)
    grimoire_machina.m_scaffold_form.reset();
  // add a new MachinaForm to the active form
  grimoire_machina.m_scaffold_form = std::make_unique<MachinaFormScaffold>();

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ClearActiveMachinaFormScaffold(GrimoireMachina &grimoire_machina) {
  // clear the active form if it exists
  if (grimoire_machina.m_scaffold_form)
    grimoire_machina.m_scaffold_form.reset();

  return std::monostate{};
}
} // namespace steamrot::logic::actions::grimoire_machina
