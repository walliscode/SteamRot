/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the UIEventLogic class.
/////////////////////////////////////////////////

#include "BaseLogic.h"
#include "CUserInterface.h"
#include "event_helpers.h"

namespace steamrot {

class UIActionLogic : public BaseLogic {

private:
  /////////////////////////////////////////////////
  /// @brief Override method to encapsulate all logic for the UIEventLogic
  /// class.
  /////////////////////////////////////////////////
  void ProcessLogic() override;

  /////////////////////////////////////////////////
  /// @brief Cycles through the EventBus and processes relevant events.
  /////////////////////////////////////////////////
  void ProcessEvents(CUserInterface &ui_component);

  void RecursiveProcessEvents(UIElement &ui_element, const EventPacket &event);

  /////////////////////////////////////////////////
  /// @brief Applies the logic for processing mouse events recursively
  ///
  /// @param element UILement to process mouse events for.
  /////////////////////////////////////////////////
  void ProcessMouseEvents(UIElement &element, UserInputBitset &user_input);

  /////////////////////////////////////////////////
  /// @brief Process actions that just affect the local UI without passing to
  /// the LogicBus
  ///
  /// @param element UIElement being evaluated
  /// @return True
  /////////////////////////////////////////////////
  bool LocalUIActions(UIElement &element);

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
