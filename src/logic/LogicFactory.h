/**
 * @file
 * @brief LogicFactory class declaration along with any helper classes.
 */

#include "BaseLogic.h"
#include "logics_generated.h"
#include <memory>
#include <unordered_map>
namespace steamrot {

/**
 * @class LogicFactory
 * @brief Provides vectors of logic objects for the game.
 *
 */
class LogicFactory {
private:
  /////////////////////////////////////////////////
  /// @brief Create a vector of logic objects specifically for collision
  ///
  /// @param logic_context  LogicContext containing references to the scene
  /////////////////////////////////////////////////
  std::vector<std::unique_ptr<BaseLogic>>
  CreateCollisionLogics(const LogicContext logic_context);

  /**
   * @brief Create a vector of logic objects specifically for rendering.
   */
  std::vector<std::unique_ptr<BaseLogic>>
  CreateRenderLogics(const LogicContext logic_context);

public:
  /**
   * @brief Default constructor
   */
  LogicFactory() = default;

  /**
   * @brief Return a map of different logic types to their corresponding logic
   * objects.
   */
  std::unordered_map<LogicType, std::vector<std::unique_ptr<BaseLogic>>>
  CreateLogicMap(const LogicCollection &logic_collection,
                 const LogicContext logic_context);
};
} // namespace steamrot
