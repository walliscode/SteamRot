/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the CGrimoireMachina class.
/////////////////////////////////////////////////

#pragma once

#include "CMachinaForm.h"
#include "Component.h"
#include "Fragment.h"
#include "fragments_generated.h"
#include "grimoire_machina_generated.h"

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
  const size_t GetComponentRegisterIndex() const override;

  /////////////////////////////////////////////////
  /// @brief Creates and returns a string representation of the component name.
  /////////////////////////////////////////////////
  const std::string &Name() override;

  /////////////////////////////////////////////////
  /// @brief Configure the CGrimoireMachina component.
  /////////////////////////////////////////////////
  void Configure(const GrimoireMachina *grimoire_data);

private:
  /////////////////////////////////////////////////
  /// @brief Configure and load all fragments from the provided data.
  ///
  /// @param fragment_data Flatbuffers data containing all fragments from
  /// specified file.
  /////////////////////////////////////////////////
  void ConfigureFragment(const FragmentData *fragment_data);

  void ConfigureJoint(const JointData *joint_data);
};
} // namespace steamrot
