/////////////////////////////////////////////////
/// @file
/// @brief An Enum class representing the different types of Logic class in the
/// game
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

namespace steamrot {

enum class LogicType {
  Test = 0,
  UIRender,
  UIState,
  UIAction,
  UICollision,
  CraftingRender
};

enum class LogicGrouping {
  Collision,
  Render,
  Action,
  Movement,
};
} // namespace steamrot
