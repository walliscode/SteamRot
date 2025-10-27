/////////////////////////////////////////////////
/// @file
/// @brief Declaration of UICollisionLogic class
/////////////////////////////////////////////////

#include "Logic.h"

namespace steamrot {
class UICollisionLogic : public Logic {

private:
  /////////////////////////////////////////////////
  /// @brief Applies all UICollision logic to correct entities
  /////////////////////////////////////////////////
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
