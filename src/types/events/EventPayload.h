/////////////////////////////////////////////////
/// @file
/// @brief Declaration of all payload types for Events
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "SceneType.h"
#include "uuid.h"
#include <optional>
#include <string>
#include <variant>
namespace steamrot {

/////////////////////////////////////////////////
/// @class InputPayload
/// @brief Any event information concenred with user input
///
/// For now, this will mainly revolve around converting the sfml keyboard/mouse
/// state to actions
/////////////////////////////////////////////////
struct InputPayload {

  /////////////////////////////////////////////////
  /// @brief The InputAction enum provides the action the user is taking
  ///
  /// Any user input events are converted to this format for ease of use in the
  /// EventHandler
  /////////////////////////////////////////////////
  enum class InputAction { NONE, SELECT } action;
};

/////////////////////////////////////////////////
/// @class UIPayload
/// @brief Any event information concerned with user interfaces
/////////////////////////////////////////////////
struct UIPayload {
  /////////////////////////////////////////////////
  /// @brief The name of single user interface being targeted
  /////////////////////////////////////////////////
  std::optional<std::string> c_user_interface_name;

  /////////////////////////////////////////////////
  /// @brief The name of the user interface state being targeted
  /////////////////////////////////////////////////
  std::optional<std::string> c_ui_state_name;

  /////////////////////////////////////////////////
  /// @brief The specific action being taken on the user interface
  /////////////////////////////////////////////////
  enum class UIAction { TOGGLE } action;
};

/////////////////////////////////////////////////
/// @class LogicPayload
/// @brief Any event information concerned with passing information to the logic
/// classes
/////////////////////////////////////////////////
struct LogicPayload {

  /////////////////////////////////////////////////
  /// @brief LogicToggle enum provides the name of the logic toggle being
  /// triggered
  ///
  /// This is to be used by the Logic classes to manage flow control without
  /// strings causing issues
  /////////////////////////////////////////////////
  enum class LogicToggle {
    INITIATE_MACHINA_FORM_SCAFFOLD,
    CLEAR_MACHINA_FORM_SCAFFOLD
  } toggle_name;
};

/////////////////////////////////////////////////
/// @class ScenePayload
/// @brief Any event information concerned with processing whole Scenes
/////////////////////////////////////////////////
struct ScenePayload {

  /////////////////////////////////////////////////
  /// @brief Dictates what to do with the Scene being targeted
  /////////////////////////////////////////////////
  enum class SceneAction { CHANGE } action;

  /////////////////////////////////////////////////
  /// @brief SceneType enum provides the type of the Scene being targeted
  /////////////////////////////////////////////////
  std::optional<SceneType> scene_type;

  /////////////////////////////////////////////////
  /// @brief Provides the unique identifier for the Scene being targeted
  /////////////////////////////////////////////////
  std::optional<uuids::uuid> scene_id;
};

/////////////////////////////////////////////////
/// @class SystemPayload
/// @brief Any event information concerned with processing system level events,
/// such as quitting the game
/////////////////////////////////////////////////
struct SystemPayload {
  /////////////////////////////////////////////////
  /// @brief SystemAction enum provides the name of the system action being
  /// triggered
  /////////////////////////////////////////////////
  enum class SystemAction { QUIT } action;
};

using EventPayload = std::variant<InputPayload, UIPayload, LogicPayload,
                                  ScenePayload, SystemPayload>;

} // namespace steamrot
