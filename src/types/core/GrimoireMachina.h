/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the CGrimoireMachina class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Fragment.h"
#include "MachinaForm.h"
#include <map>
#include <memory>
namespace steamrot {

struct GrimoireMachina {

  GrimoireMachina() = default;

  /////////////////////////////////////////////////
  /// @brief Copy constructor for deep copying
  ///
  /// @param other The CGrimoireMachina to copy from
  /////////////////////////////////////////////////
  GrimoireMachina(const GrimoireMachina &other)
      : m_all_fragments(other.m_all_fragments),
        m_all_joints(other.m_all_joints),
        m_machina_forms(other.m_machina_forms),
        m_holding_form(other.m_holding_form ? std::make_unique<MachinaForm>(
                                                  *other.m_holding_form)
                                            : nullptr) {}

  /////////////////////////////////////////////////
  /// @brief Copy assignment operator for deep copying
  ///
  /// @param other The CGrimoireMachina to copy from
  /// @return Reference to this CGrimoireMachina
  /////////////////////////////////////////////////
  GrimoireMachina &operator=(const GrimoireMachina &other) {
    if (this != &other) {

      m_all_fragments = other.m_all_fragments;
      m_all_joints = other.m_all_joints;
      m_machina_forms = other.m_machina_forms;
      m_holding_form =
          other.m_holding_form
              ? std::make_unique<MachinaForm>(*other.m_holding_form)
              : nullptr;
    }
    return *this;
  }

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
  std::map<std::string, MachinaForm> m_machina_forms;

  /////////////////////////////////////////////////
  /// @brief A holding form used to build up a new structure
  /////////////////////////////////////////////////
  std::unique_ptr<MachinaForm> m_holding_form{nullptr};
};
} // namespace steamrot
