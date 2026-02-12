/////////////////////////////////////////////////
/// @file
/// @brief An Enum class representing the different types of Logic class in the
/// game
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include <string>
namespace steamrot {

enum class LogicType {
  None,
  Test,
  Dummy,
  UIRender,
  UIState,
  UIAction,
  UICollision,
  CraftingRender,
  GrimoireMachinaAction,
  GrimoireMachinaPositioning,
  GrimoireMachinaRender,
  GrimoireMachinaCollision,
};

inline std::string EnumNameLogicType(LogicType logic_type) {
  switch (logic_type) {
  case LogicType::None:
    return "None";
  case LogicType::Test:
    return "Test";
  case LogicType::UIRender:
    return "UIRender";
  case LogicType::UIState:
    return "UIState";
  case LogicType::UIAction:
    return "UIAction";
  case LogicType::UICollision:
    return "UICollision";
  case LogicType::CraftingRender:
    return "CraftingRender";
  case LogicType::GrimoireMachinaAction:
    return "GrimoireMachinaAction";
  case LogicType::GrimoireMachinaPositioning:
    return "GrimoireMachinaPositioning";
  case LogicType::GrimoireMachinaRender:
    return "GrimoireMachinaRender";
  case LogicType::GrimoireMachinaCollision:
    return "GrimoireMachinaCollision";
  default:
    return "INVALID_LOGIC_TYPE";
  }
}

enum class LogicGrouping {
  Collision,
  Render,
  Action,
  Movement,
};

inline std::string EnumNameLogicGrouping(LogicGrouping logic_grouping) {
  switch (logic_grouping) {
  case LogicGrouping::Collision:
    return "Collision";
  case LogicGrouping::Render:
    return "Render";
  case LogicGrouping::Action:
    return "Action";
  case LogicGrouping::Movement:
    return "Movement";
  default:
    return "INVALID_LOGIC_GROUPING";
  }
}
} // namespace steamrot
