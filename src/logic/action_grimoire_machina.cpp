/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for process actions related to the
/// GrimoireMachina.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_grimoire_machina.h"
#include "EventPayload.h"
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
GetAllFragmentNames(GrimoireMachina &grimoire_machina) {

  std::vector<std::string> fragment_names;
  // cycle through all fragments in the GrimoireMachina and add their names to
  // the vector
  for (const auto &[name, fragment] : grimoire_machina.m_all_fragments) {
    fragment_names.push_back(name);
  }
  return fragment_names;
}

/////////////////////////////////////////////////
std::vector<std::string>
GetAllJointNames(GrimoireMachina &grimoire_machina) {

  std::vector<std::string> joint_names;
  // cycle through all joints in the GrimoireMachina and add their names to
  // the vector
  for (const auto &[name, joint] : grimoire_machina.m_all_joints) {
    joint_names.push_back(name);
  }
  return joint_names;
}

/////////////////////////////////////////////////
void ProcessSubscriber(Subscriber &subscriber,
                       GrimoireMachina &grimoire_machina) {
  if (!subscriber.captured_payload.has_value())
    return;

  const LogicPayload *logic_payload =
      std::get_if<LogicPayload>(&subscriber.captured_payload.value());
  if (!logic_payload)
    return;

  if (logic_payload->toggle_name ==
      LogicPayload::LogicToggle::INITIATE_MACHINA_FORM_SCAFFOLD) {
    InitialiseActiveMachinaFormScaffold(grimoire_machina);
  } else if (logic_payload->toggle_name ==
             LogicPayload::LogicToggle::CLEAR_MACHINA_FORM_SCAFFOLD) {
    ClearActiveMachinaFormScaffold(grimoire_machina);
  }
}

} // namespace steamrot::logic::action::grimoire_machina
