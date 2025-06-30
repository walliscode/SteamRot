/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the UIEventLogic class.
/////////////////////////////////////////////////

#include "BaseLogic.h"
#include "CUserInterface.h"
#include "Logic.h"
namespace steamrot {

class UIActionLogic : public Logic<CUserInterface> {

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

  /////////////////////////////////////////////////
  /// @brief Process actions that just affect the local UI without passing to
  /// the LogicBus
  ///
  /// @param element UIElement being evaluated
  /// @return True if the action was processed, false otherwise.
  /////////////////////////////////////////////////
  bool LocalUIActions(UIElement &element);

  /////////////////////////////////////////////////
  /// @brief Handle any actions associated with a drop down container.
  ///
  /// @param element UIElement containing the DropDownContainer variant.
  /////////////////////////////////////////////////
  void HandleDropDownContainerActions(UIElement &element);

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
