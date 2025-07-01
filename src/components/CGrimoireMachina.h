/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the CGrimoireMachina class.
/////////////////////////////////////////////////

#pragma once

#include "CMachinaForm.h"
#include "Component.h"
#include "Fragment.h"
#include <map>
namespace steamrot {

struct CGrimoireMachina : public Component {
  CGrimoireMachina() = default;

  /////////////////////////////////////////////////
  /// @brief All available fragments in the game.
  /////////////////////////////////////////////////
  std::map<std::string, Fragment> m_all_fragments;

  /////////////////////////////////////////////////
  /// @brief All available joints in the game.
  /////////////////////////////////////////////////
  std::map<std::string, Joint> m_all_joints;

  /////////////////////////////////////////////////
  /// @brief Collection of all available MachinaForms. These are designed to be
  /// copied and not used directly.
  /////////////////////////////////////////////////
  std::map<std::string, CMachinaForm> m_machina_forms;

  /////////////////////////////////////////////////
  /// @brief A holding form used to build up a new structure
  /////////////////////////////////////////////////
  CMachinaForm m_holding_form;

  const size_t GetComponentRegisterIndex() const override;

  const std::string &Name() override;
};
} // namespace steamrot
