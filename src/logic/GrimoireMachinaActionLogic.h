/////////////////////////////////////////////////
/// @file
/// @brief Declaration of GrimoireMachinaActionLogic class
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
#include "Subscriber.h"
#include <memory>

namespace steamrot::logic {

/////////////////////////////////////////////////
/// @class GrimoireMachinaActionLogic
/// @brief Deals with flow control for the Grimoire Machina action sequence
/////////////////////////////////////////////////
class GrimoireMachinaActionLogic : public Logic {

  /////////////////////////////////////////////////
  /// @brief Subscriber for USER_INPUT SELECT events, used to detect
  /// pick-and-place clicks on the crafting canvas.
  /////////////////////////////////////////////////
  std::shared_ptr<Subscriber> m_place_subscriber;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for the GrimoireMachinaActionLogic class.
  ///
  /// @param scene_context SceneContext struct instance to be used by the logic
  /// class.
  /////////////////////////////////////////////////
  GrimoireMachinaActionLogic(const SceneContext scene_context);

  /////////////////////////////////////////////////
  /// @brief Wrapper function for the logic processing of the
  /// GrimoireMachinaActionLogic class.
  /////////////////////////////////////////////////
  void ProcessLogic() override;

  /////////////////////////////////////////////////
  /// @brief Returns the LogicType enum value corresponding to the
  ///
  /// @return LogicType::GrimoireMachinaAction
  /////////////////////////////////////////////////
  LogicType GetLogicType() const override {
    return LogicType::GrimoireMachinaAction;
  }
};
} // namespace steamrot::logic
