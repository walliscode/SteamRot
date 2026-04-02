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

  /////////////////////////////////////////////////
  /// @brief Default constructor for InputPayload, sets action to NONE
  /////////////////////////////////////////////////
  InputPayload() = default;

  /////////////////////////////////////////////////
  /// @brief Constructor for InputPayload, takes an InputAction and sets the
  /// action member
  ///
  /// @param action InputAction enum value to set the action member to
  /////////////////////////////////////////////////
  InputPayload(const InputPayload::InputAction action) : action(action) {}
};

/////////////////////////////////////////////////
/// @class UIPayload
/// @brief Any event information concerned with user interfaces
/////////////////////////////////////////////////
struct UIPayload {

  /////////////////////////////////////////////////
  /// @brief The specific action being taken on the user interface
  /////////////////////////////////////////////////
  enum class UIAction { NONE, TOGGLE } action;

  /////////////////////////////////////////////////
  /// @brief Default constructor for UIPayload, sets action to TOGGLE
  /////////////////////////////////////////////////
  UIPayload() = default;

  /////////////////////////////////////////////////
  /// @brief Constructor for UIPayload, takes a UIAction and a string for the
  /// name of the UI state being targeted.
  ///
  /// @param action UIAction enum value to set the action member to
  /// @param c_ui_state_name name of the user interface state being targeted,
  /// used by the UI classes
  /////////////////////////////////////////////////
  UIPayload(const UIPayload::UIAction action, const std::string c_ui_state_name)
      : action(action), c_ui_state_name(c_ui_state_name) {}

  /////////////////////////////////////////////////
  /// @brief The name of the CUserInterface component being targeted
  /////////////////////////////////////////////////
  std::string c_user_interface_name{};

  /////////////////////////////////////////////////
  /// @brief The name of the user interface state being targeted
  /////////////////////////////////////////////////
  std::string c_ui_state_name;
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
    NONE,

    // GrimoireMachina toggles
    INITIATE_MACHINA_FORM_SCAFFOLD,
    CLEAR_MACHINA_FORM_SCAFFOLD

    // add other categories as needed

  } toggle_name;

  /////////////////////////////////////////////////
  /// @brief Default constructor for LogicPayload, sets toggle_name to
  /// INITIATE_MACHINA_FORM_SCAFFOLD
  /////////////////////////////////////////////////
  LogicPayload() = default;

  /////////////////////////////////////////////////
  /// @brief Constructor for LogicPayload, takes a LogicToggle and sets the
  /// toggle_name to NONE
  ///
  /// @param toggle_name Enum value to set the toggle_name member to
  /////////////////////////////////////////////////
  LogicPayload(const LogicPayload::LogicToggle toggle_name)
      : toggle_name(toggle_name) {}
};

/////////////////////////////////////////////////
/// @class ScenePayload
/// @brief Any event information concerned with processing whole Scenes
/////////////////////////////////////////////////
struct ScenePayload {

  /////////////////////////////////////////////////
  /// @brief Dictates what to do with the Scene being targeted
  /////////////////////////////////////////////////
  enum class SceneAction { NONE, CHANGE } action{SceneAction::NONE};

  /////////////////////////////////////////////////
  /// @brief Default constructor for ScenePayload, sets action to NONE
  /////////////////////////////////////////////////
  ScenePayload() = default;

  /////////////////////////////////////////////////
  /// @brief Constructor for ScenePayload, takes a SceneAction and a SceneType
  ///
  /// @param action SceneAction enum value to set the action member to
  /// @param scene_type SceneType enum value to set the scene_type member to
  /////////////////////////////////////////////////
  ScenePayload(const ScenePayload::SceneAction action,
               const SceneType scene_type)
      : action(action), optional_scene_type(scene_type) {}

  /////////////////////////////////////////////////
  /// @brief Constructor for ScenePayload, takes a SceneAction, a SceneType, and
  /// a scene_id
  ///
  /// @param action SceneAction enum value to set the action member to
  /// @param scene_type SceneType enum value to set the scene_type member to
  /// @param scene_id UUID value to set the scene_id member to, used for
  /// targeting specific scenes
  /////////////////////////////////////////////////
  ScenePayload(const ScenePayload::SceneAction action,
               const SceneType scene_type, const uuids::uuid scene_id)
      : action(action), optional_scene_type(scene_type),
        optional_scene_id(scene_id) {}

  /////////////////////////////////////////////////
  /// @brief SceneType enum provides the type of the Scene being targeted
  /////////////////////////////////////////////////
  std::optional<SceneType> optional_scene_type{SceneType::UNKNOWN};

  /////////////////////////////////////////////////
  /// @brief Provides the unique identifier for the Scene being targeted
  /////////////////////////////////////////////////
  std::optional<uuids::uuid> optional_scene_id;
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
  enum class SystemAction { NONE, QUIT } action;

  /////////////////////////////////////////////////
  /// @brief Default constructor for SystemPayload, sets action to NONE
  /////////////////////////////////////////////////
  SystemPayload() = default;

  /////////////////////////////////////////////////
  /// @brief Constructor for SystemPayload, takes a SystemAction and sets the
  /// action member
  ///
  /// @param action SystemAction enum value to set the action member to
  /////////////////////////////////////////////////
  SystemPayload(const SystemPayload::SystemAction action) : action(action) {}
};

/////////////////////////////////////////////////
/// @class SelectAndPlacePayload
/// @brief Event payload for the select-and-place system.
///
/// Carries the action being performed (selecting or placing an item),
/// the name of the item, and its type (e.g. "fragment", "joint").
/// This payload is used to communicate selection and placement intent
/// across Logic classes and scenes, enabling different placement
/// behaviours depending on context.
/////////////////////////////////////////////////
struct SelectAndPlacePayload {

  /////////////////////////////////////////////////
  /// @brief The action being performed in the select-and-place workflow
  /////////////////////////////////////////////////
  enum class Action {
    NONE,
    SELECT_ITEM, ///< An item has been selected for placement
    PLACE_ITEM   ///< The selected item should be placed at the current position
  } action{Action::NONE};

  /////////////////////////////////////////////////
  /// @brief Name of the item being selected or placed
  /////////////////////////////////////////////////
  std::string item_name;

  /////////////////////////////////////////////////
  /// @brief Category of the item (e.g. "fragment", "joint")
  /////////////////////////////////////////////////
  std::string item_type;

  /////////////////////////////////////////////////
  /// @brief Default constructor, sets action to NONE
  /////////////////////////////////////////////////
  SelectAndPlacePayload() = default;

  /////////////////////////////////////////////////
  /// @brief Constructor for SelectAndPlacePayload
  ///
  /// @param action  Action being performed
  /// @param item_name Name of the item
  /// @param item_type Category of the item
  /////////////////////////////////////////////////
  SelectAndPlacePayload(const Action action, std::string item_name,
                        std::string item_type)
      : action(action), item_name(std::move(item_name)),
        item_type(std::move(item_type)) {}
};

using EventPayload =
    std::variant<std::monostate, InputPayload, UIPayload, LogicPayload,
                 ScenePayload, SystemPayload, SelectAndPlacePayload>;

} // namespace steamrot
