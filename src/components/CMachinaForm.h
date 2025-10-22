/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the CMachinaForm component
/////////////////////////////////////////////////

#pragma once

#include "Component.h"
#include "Fragment.h"
#include "Joint.h"

namespace steamrot {
/////////////////////////////////////////////////
/// @class CMachinaForm
/// @brief CMachinaForm is a data store of a combination of Fragments, Joints
/// and any asscociated data
///
/// Each CMachinaForm contains all the Fragments and Joints that define an
/// Entity. Including how they link together and limitiations e.t.c.
/////////////////////////////////////////////////
struct CMachinaForm : public Component {

  CMachinaForm() = default;

  /////////////////////////////////////////////////
  /// @brief Contains all Fragments for this Entity/MachinaForm
  ///
  /// Not designed to be modified once created, game mechanics is about making
  /// new designs. Modificaion would be copy then modify.
  /////////////////////////////////////////////////
  std::vector<Fragment> m_fragments;

  std::vector<Joint> m_joints;

  size_t GetComponentRegisterIndex() const override;

  ////////////////////////////////////////////////////////////
  /// @brief Equality operator for CMachinaForm
  ///
  /// @param other The CMachinaForm to compare with
  /// @return true if components are equal, false otherwise
  ////////////////////////////////////////////////////////////
  bool operator==(const CMachinaForm &other) const = default;
};
} // namespace steamrot
