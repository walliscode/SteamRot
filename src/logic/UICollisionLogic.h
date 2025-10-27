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
  /// @brief Constructor for UICollisionLogic taking in a SceneContext
  ///
  /// @param logic_context SceneContext containing references to the scene
  /////////////////////////////////////////////////
  UICollisionLogic(const SceneContext logic_context);
};

} // namespace steamrot
