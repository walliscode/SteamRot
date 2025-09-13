/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the UIEventLogic class.
/////////////////////////////////////////////////

#include "Logic.h"

namespace steamrot {

class UIActionLogic : public Logic {

private:
  /////////////////////////////////////////////////
  /// @brief Override method to encapsulate all logic for the UIEventLogic
  /// class.
  /////////////////////////////////////////////////
  void ProcessLogic() override;

  /////////////////////////////////////////////////
  /// @brief Handle any actions associated with a drop down container.
  ///
  /// @param element UIElement containing the DropDownContainer variant.
  /////////////////////////////////////////////////
  void ToggleDropDown(UIElement &element);

  /////////////////////////////////////////////////
  /// @brief Returns a vector of the names of all available fragments in the
  /// CGrimoireMachina.
  /////////////////////////////////////////////////
  std::vector<std::string> GetAvailableFragments();

public:
  /////////////////////////////////////////////////
  /// @brief COnstructor for UIEventLogic.
  ///
  /// @param logic_context LogicContext object containing the context for the
  /// logic.
  /////////////////////////////////////////////////
  UIActionLogic(const LogicContext logic_context);
};
} // namespace steamrot
