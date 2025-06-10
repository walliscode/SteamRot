/**
 * @file
 * @brief LogicFactory class declaration along with any helper classes.
 */
#pragma once
#include "BaseLogic.h"
#include "logics_generated.h"
#include <memory>
#include <unordered_map>
#include <vector>
namespace steamrot {

/**
 * @class LogicFactory
 * @brief Provide a map of containers for logic objects.
 *
 */
class LogicFactory {
private:
  /**
   * @brief Default constructor.
   */
  LogicFactory() = default;

  /**
   * @brief Return a vector of Logic objects for Rendering.
   */
  std::vector<std::unique_ptr<BaseLogic>> CreateRenderLogics();

public:
  /**
   * @brief Returns a map of logic objects for the Scene to use.
   */
  std::unordered_map<const LogicType, std::vector<std::unique_ptr<BaseLogic>>>
  CreateLogicMap(const std::vector<LogicType> &logic_types);
};
} // namespace steamrot
