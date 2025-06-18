/////////////////////////////////////////////////
/// @file
/// @brief Declaration of UICollisionLogic class
/////////////////////////////////////////////////

#include "BaseLogic.h"
#include "CUserInterface.h"
#include "Logic.h"

namespace steamrot {
class UICollisionLogic : public Logic<CUserInterface> {

private:
  /////////////////////////////////////////////////
  /// @brief Check through the component and its elements
  ///
  /// @param ui_component CUserInterface component from entity memory pool
  /////////////////////////////////////////////////
  void CheckMouseCollision(CUserInterface &ui_component);

  bool RecursiveCheckMouseCollision(UIElement &element);

  void ProcessLogic() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for UICollisionLogic taking in a LogicContext
  ///
  /// @param logic_context LogicContext containing references to the scene
  /////////////////////////////////////////////////
  UICollisionLogic(const LogicContext logic_context);
};

} // namespace steamrot
