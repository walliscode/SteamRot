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

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
AddFragmentToScaffold(GrimoireMachina &grimoire_machina,
                      const std::string &fragment_name) {

  // ensure the scaffold exists
  if (!grimoire_machina.m_scaffold_form) {
    auto init_result = InitialiseActiveMachinaFormScaffold(grimoire_machina);
    if (!init_result.has_value()) {
      return std::unexpected(init_result.error());
    }
  }

  // find the fragment by name
  auto it = grimoire_machina.m_all_fragments.find(fragment_name);
  if (it == grimoire_machina.m_all_fragments.end()) {
    return std::unexpected(
        FailInfo{FailMode::MissingRequiredField,
                 "AddFragmentToScaffold: fragment not found: " + fragment_name});
  }

  Fragment &fragment = it->second;

  // build socket list from the fragment's local socket positions
  std::vector<Socket> sockets;
  sockets.reserve(fragment.sockets.size());
  for (const sf::Vector2f &socket_pos : fragment.sockets) {
    Socket socket;
    socket.circle.setPosition(socket_pos);
    sockets.push_back(socket);
  }

  // place the fragment at the growth point's current position
  sf::Transform transform = sf::Transform::Identity;
  transform.translate(
      grimoire_machina.m_scaffold_form->growth_point.origin.getPosition());

  grimoire_machina.m_scaffold_form->fragments.push_back(
      FragmentInstance{fragment, transform, std::move(sockets)});

  return std::monostate{};
}
} // namespace steamrot::logic::action::grimoire_machina
