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
#include <string>
#include <vector>

namespace steamrot::logic::action::grimoire_machina {
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
void SetColor(GrowthPoint &growth_point) {
  if (growth_point.is_mouse_over) {

    growth_point.origin.setFillColor(growth_point.hover_color);
  } else {
    growth_point.origin.setFillColor(growth_point.base_color);
  }
}

/////////////////////////////////////////////////
void SetColor(Socket &socket) {
  if (socket.is_mouse_over) {
    socket.circle.setFillColor(socket.hover_color);
  } else {
    socket.circle.setFillColor(socket.base_color);
  }
}

/////////////////////////////////////////////////
void SetColor(FragmentInstance &fragment_instance) {
  for (auto &socket : fragment_instance.sockets) {
    SetColor(socket);
  }
}

/////////////////////////////////////////////////
void SetColor(JointInstance &joint_instance) {
  for (auto &socket : joint_instance.sockets) {
    SetColor(socket);
  }
}

/////////////////////////////////////////////////
std::vector<std::string>
GetAllFragmentNames(const GrimoireMachina &grimoire_machina) {
  std::vector<std::string> fragment_names;
  // cycle through all fragments in the GrimoireMachina and add their names to
  // the vector
  for (const auto &[name, fragment] : grimoire_machina.m_all_fragments) {
    fragment_names.push_back(name);
  }
  return fragment_names;
}
} // namespace steamrot::logic::action::grimoire_machina
