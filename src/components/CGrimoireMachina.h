/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the CGrimoireMachina class.
/////////////////////////////////////////////////

#pragma once

#include "CMachinaForm.h"
#include "Component.h"
#include "Fragment.h"
#include <map>
#include <memory>
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
  std::unique_ptr<CMachinaForm> m_holding_form{nullptr};

  /////////////////////////////////////////////////
  /// @brief Get the position of the Component in the Component Register.
  ///
  /// @return index of the component in the component register
  /////////////////////////////////////////////////
  size_t GetComponentRegisterIndex() const override;
};
} // namespace steamrot
