/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the UIEventLogic class.
/////////////////////////////////////////////////

#include "BaseLogic.h"
#include "CUserInterface.h"
#include "Logic.h"
namespace steamrot {

class UIEventLogic : public Logic<CUserInterface> {

private:
  /////////////////////////////////////////////////
  /// @brief Override method to encapsulate all logic for the UIEventLogic
  /// class.
  /////////////////////////////////////////////////
  void ProcessLogic() override;

  /////////////////////////////////////////////////
  /// @brief Encapsualtes the logic for processing all mouse events in the UI.
  /////////////////////////////////////////////////
  void ProcessMouseEvents(CUserInterface &ui_component);

  /////////////////////////////////////////////////
  /// @brief Applies the logic for processing mouse events recursively
  ///
  /// @param element UILement to process mouse events for.
  /////////////////////////////////////////////////
  void RecursiveProcessMouseEvents(UIElement &element);

public:
  /////////////////////////////////////////////////
  /// @brief COnstructor for UIEventLogic.
  ///
  /// @param logic_context LogicContext object containing the context for the
  /// logic.
  /////////////////////////////////////////////////
  UIEventLogic(const LogicContext logic_context);
};
} // namespace steamrot
