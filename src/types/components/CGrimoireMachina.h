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
#include "CMachinaForm.h"
#include "Component.h"
#include "Fragment.h"
#include <map>
#include <memory>
namespace steamrot {

struct CGrimoireMachina : public Component {
  CGrimoireMachina() = default;

  /////////////////////////////////////////////////
  /// @brief Copy constructor for deep copying
  ///
  /// @param other The CGrimoireMachina to copy from
  /////////////////////////////////////////////////
  CGrimoireMachina(const CGrimoireMachina &other)
      : Component(other), m_all_fragments(other.m_all_fragments),
        m_all_joints(other.m_all_joints),
        m_machina_forms(other.m_machina_forms),
        m_holding_form(other.m_holding_form ? std::make_unique<CMachinaForm>(
                                                  *other.m_holding_form)
                                            : nullptr) {}

  /////////////////////////////////////////////////
  /// @brief Copy assignment operator for deep copying
  ///
  /// @param other The CGrimoireMachina to copy from
  /// @return Reference to this CGrimoireMachina
  /////////////////////////////////////////////////
  CGrimoireMachina &operator=(const CGrimoireMachina &other) {
    if (this != &other) {
      Component::operator=(other);
      m_all_fragments = other.m_all_fragments;
      m_all_joints = other.m_all_joints;
      m_machina_forms = other.m_machina_forms;
      m_holding_form =
          other.m_holding_form
              ? std::make_unique<CMachinaForm>(*other.m_holding_form)
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
  std::map<std::string, CMachinaForm> m_machina_forms;

  /////////////////////////////////////////////////
  /// @brief A holding form used to build up a new structure
  /////////////////////////////////////////////////
  std::unique_ptr<CMachinaForm> m_holding_form{nullptr};
};
} // namespace steamrot
