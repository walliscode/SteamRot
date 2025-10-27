/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the UIStateLogic class.
/////////////////////////////////////////////////

#pragma once

#include "Logic.h"

namespace steamrot {

class UIStateLogic : public Logic {

private:
  /////////////////////////////////////////////////
  /// @brief Override method to encapsulate all logic for UIStateLogic
  /////////////////////////////////////////////////
  void ProcessLogic() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for UIStateLogic.
  ///
  /// @param logic_context LogicContext object containing the context for the logic.
  /////////////////////////////////////////////////
  UIStateLogic(const LogicContext logic_context);
};

} // namespace steamrot
