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
    grimoire_machina.m_scaffold_form = nullptr;

  return std::monostate{};
}

/////////////////////////////////////////////////
void SetGrowthPointColor(GrowthPoint &growth_point) {
  if (growth_point.is_mouse_over) {

    growth_point.origin.setFillColor(growth_point.hover_color);
  } else {
    growth_point.origin.setFillColor(growth_point.base_color);
  }
}
} // namespace steamrot::logic::actions::grimoire_machina
