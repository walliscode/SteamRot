/////////////////////////////////////////////////
/// @file
/// @brief Free functions for logic execution.
///
/// These functions extract the core logic execution patterns from Scene
/// classes, enabling reuse by both the game engine and test harness.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Logic.h"
#include "LogicFactory.h"
#include <memory>
#include <vector>

namespace steamrot::logic::execution {

/////////////////////////////////////////////////
/// @brief Execute all logics in a LogicVector
///
/// Iterates through the vector and calls RunLogic() on each Logic instance.
///
/// @param logics Vector of Logic unique pointers to execute
/////////////////////////////////////////////////
void ExecuteLogicVector(const LogicVector &logics);

/////////////////////////////////////////////////
/// @brief Execute all logics of a specific type from a LogicCollection
///
/// Looks up the LogicType in the collection and executes all logics
/// if found. Silently returns if the type is not in the collection.
///
/// @param logic_map The LogicCollection to search
/// @param type The LogicType to execute
/////////////////////////////////////////////////
void ExecuteLogicsByType(const LogicCollection &logic_map, LogicType type);

/////////////////////////////////////////////////
/// @brief Execute a complete scene tick (all systems in order)
///
/// Executes all logic types in the standard scene update order:
/// Action -> Movement -> Collision -> Render
///
/// @param logic_map The LogicCollection containing all scene logics
/////////////////////////////////////////////////
void ExecuteSceneTick(const LogicCollection &logic_map);

/////////////////////////////////////////////////
/// @brief Execute action logics
///
/// Convenience function to execute only Action type logics.
///
/// @param logic_map The LogicCollection to search
/////////////////////////////////////////////////
void ExecuteActionLogics(const LogicCollection &logic_map);

/////////////////////////////////////////////////
/// @brief Execute movement logics
///
/// Convenience function to execute only Movement type logics.
///
/// @param logic_map The LogicCollection to search
/////////////////////////////////////////////////
void ExecuteMovementLogics(const LogicCollection &logic_map);

/////////////////////////////////////////////////
/// @brief Execute collision logics
///
/// Convenience function to execute only Collision type logics.
///
/// @param logic_map The LogicCollection to search
/////////////////////////////////////////////////
void ExecuteCollisionLogics(const LogicCollection &logic_map);

/////////////////////////////////////////////////
/// @brief Execute render logics
///
/// Convenience function to execute only Render type logics.
///
/// @param logic_map The LogicCollection to search
/////////////////////////////////////////////////
void ExecuteRenderLogics(const LogicCollection &logic_map);

} // namespace steamrot::logic::execution
