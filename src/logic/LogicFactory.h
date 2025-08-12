/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the LogicFactory class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "BaseLogic.h"
#include "logics_generated.h"
#include <memory>
#include <unordered_map>
namespace steamrot {

/////////////////////////////////////////////////
/// @class LogicFactory
/// @brief Provides Logic objects for Scenes to store and use.
///
/////////////////////////////////////////////////
class LogicFactory {

private:
  /////////////////////////////////////////////////
  /// @brief Create a vector of logic objects specifically for collision
  ///
  /// @param logic_context  LogicContext containing references to the scene
  /////////////////////////////////////////////////
  std::vector<std::unique_ptr<BaseLogic>>
  CreateCollisionLogics(const LogicContext logic_context);

  /////////////////////////////////////////////////
  /// @brief Create a vector of logic objects specifically for rendering
  ///
  /// @param logic_context [TODO:parameter]
  /////////////////////////////////////////////////
  std::vector<std::unique_ptr<BaseLogic>>
  CreateRenderLogics(const LogicContext logic_context);

  /////////////////////////////////////////////////
  /// @brief Create a vector of logic objects specifically for actions
  ///
  /// @param logic_context [TODO:parameter]
  /////////////////////////////////////////////////
  std::vector<std::unique_ptr<BaseLogic>>
  CreateActionLogics(const LogicContext logic_context);

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor for LogicFactory.
  /////////////////////////////////////////////////
  LogicFactory() = default;

  /////////////////////////////////////////////////
  /// @brief Create and return a map of logic objects.
  ///
  /// @param logic_collection [TODO:parameter]
  /// @param logic_context [TODO:parameter]
  /////////////////////////////////////////////////
  std::unordered_map<LogicType, std::vector<std::unique_ptr<BaseLogic>>>
  CreateLogicMap(const LogicCollection &logic_collection,
                 const LogicContext logic_context);
};
} // namespace steamrot
