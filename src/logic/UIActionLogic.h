/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the UIEventLogic class.
/////////////////////////////////////////////////

#include "ButtonElement.h"
#include "EventHandler.h"
#include "Logic.h"

namespace steamrot {

class UIActionLogic : public Logic {

private:
  /////////////////////////////////////////////////
  /// @brief Override method to encapsulate all logic for the UIEventLogic
  /// class.
  /////////////////////////////////////////////////
  void ProcessLogic() override;

public:
  /////////////////////////////////////////////////
  /// @brief COnstructor for UIEventLogic.
  ///
  /// @param logic_context LogicContext object containing the context for the
  /// logic.
  /////////////////////////////////////////////////
  UIActionLogic(const LogicContext logic_context);
};

/////////////////////////////////////////////////
/// @brief Dispatches the variant to the correct action processing function.
///
/// @param ui_element Element to process.
/////////////////////////////////////////////////
void ProcessUIActionsAndEvents(UIElement &ui_element,
                               EventHandler &event_handler);

/////////////////////////////////////////////////
/// @brief Process actions for a UI element and its nested children recursively.
///
/// This function processes UI elements in a depth-first manner, ensuring that
/// child elements are processed before their parents. If a child element is
/// processed, the parent element will not be processed to avoid overlapping
/// element actions.
///
/// @param ui_element Element to process along with its children.
/// @param event_handler Event handler to process actions with.
/////////////////////////////////////////////////
void ProcessNestedUIActionsAndEvents(UIElement &ui_element,
                                     EventHandler &event_handler);

/////////////////////////////////////////////////
/// @brief Process actions for a ButtonElement
///
/// This function checks if the button is in a state to trigger its response
/// event. Thee subscriber will have already been checked before this function
/// is called.
///
/// @param button_element ButtonElement to process
/////////////////////////////////////////////////
void ProcessButtonElementActions(ButtonElement &button_element,
                                 EventHandler &event_handler);
} // namespace steamrot
