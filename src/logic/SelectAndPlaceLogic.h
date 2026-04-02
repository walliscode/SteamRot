/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SelectAndPlaceLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Logic.h"
#include "LogicType.h"
#include "SceneContext.h"
#include "SelectAndPlaceState.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
/// @class SelectAndPlaceLogic
/// @brief Manages the select-and-place workflow.
///
/// This Logic class is responsible for:
/// - Listening for SELECT_ITEM events (e.g. from a dropdown) and updating
///   internal selection state.
/// - Listening for USER_INPUT SELECT events and, when an item is already
///   selected, firing a PLACE_ITEM event for context-specific handlers.
///
/// The actual placement behaviour is implemented by other Logic classes
/// (e.g. GrimoireMachinaActionLogic in the crafting scene), which subscribe
/// to SELECT_AND_PLACE PLACE_ITEM events. This design allows the same
/// SelectAndPlaceLogic to be reused across different scenes with different
/// placement outcomes.
///
/// Subscribers are registered in the constructor rather than through the
/// FlatBuffers logic config, as the payloads are created programmatically.
/////////////////////////////////////////////////
class SelectAndPlaceLogic : public Logic {

private:
  /////////////////////////////////////////////////
  /// @brief Current item selection state
  /////////////////////////////////////////////////
  SelectAndPlaceState m_select_and_place_state;

  /////////////////////////////////////////////////
  /// @brief Process selection and placement per frame
  /////////////////////////////////////////////////
  void ProcessLogic() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for SelectAndPlaceLogic.
  ///
  /// Creates and registers the necessary Subscribers for SELECT_ITEM and
  /// USER_INPUT SELECT events.
  ///
  /// @param scene_context SceneContext containing dependencies
  /////////////////////////////////////////////////
  SelectAndPlaceLogic(const SceneContext scene_context);

  /////////////////////////////////////////////////
  /// @brief Returns the LogicType of this Logic instance.
  ///
  /// @return LogicType::SelectAndPlace
  /////////////////////////////////////////////////
  LogicType GetLogicType() const override { return LogicType::SelectAndPlace; }
};

} // namespace steamrot::logic
