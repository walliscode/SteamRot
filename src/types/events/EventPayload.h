/////////////////////////////////////////////////
/// @file
/// @brief Declaration of all payload types for Events
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "MrGhost.h"
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
  enum class InputAction { NONE, SELECT, TOGGLE_SOCKET_VISIBILITY } action;

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
/// @class GhostPayload
/// @brief Event payload for MrGhost selection changes.
///
/// Emitted by Logic classes that write to MrGhost (e.g. a UI action logic
/// that detects a menu item click). Logic classes that need to react to
/// selection changes subscribe to EventType::GHOST and inspect this payload.
/////////////////////////////////////////////////
struct GhostPayload {

  /////////////////////////////////////////////////
  /// @brief Discriminates between selecting and clearing the ghost state.
  /////////////////////////////////////////////////
  enum class GhostAction { NONE, SELECT, CLEAR } action{GhostAction::NONE};

  /////////////////////////////////////////////////
  /// @brief Default constructor for GhostPayload, sets action to NONE.
  /////////////////////////////////////////////////
  GhostPayload() = default;

  /////////////////////////////////////////////////
  /// @brief Constructor for a SELECT action carrying the chosen item.
  ///
  /// @param instance The GhostInstance variant describing what was selected.
  /////////////////////////////////////////////////
  GhostPayload(const GhostInstance &instance)
      : action(GhostAction::SELECT), m_instance(instance) {}

  /////////////////////////////////////////////////
  /// @brief Constructor taking an explicit action and optional instance.
  ///
  /// @param ghost_action GhostAction enum value to set the action member to.
  /// @param instance The GhostInstance variant (relevant for SELECT action).
  /////////////////////////////////////////////////
  GhostPayload(const GhostAction ghost_action, const GhostInstance &instance)
      : action(ghost_action), m_instance(instance) {}

  /////////////////////////////////////////////////
  /// @brief The typed ghost instance carried by this payload.
  ///
  /// std::monostate when action is CLEAR or NONE.
  /////////////////////////////////////////////////
  GhostInstance m_instance{std::monostate{}};
};

/////////////////////////////////////////////////
/// @class CameraPayload
/// @brief Event payload carrying a mouse-scroll delta for camera zoom.
///
/// Emitted each tick that a mouse-wheel event is detected by
/// SFMLEventConverter and consumed by CameraMovementLogic to update the
/// scene CameraState.
/////////////////////////////////////////////////
struct CameraPayload {

  /////////////////////////////////////////////////
  /// @brief Raw scroll delta from SFML this tick.
  ///
  /// Positive = scroll up (zoom in); negative = scroll down (zoom out).
  /////////////////////////////////////////////////
  float scroll_delta{0.0f};

  /////////////////////////////////////////////////
  /// @brief Default constructor.
  /////////////////////////////////////////////////
  CameraPayload() = default;

  /////////////////////////////////////////////////
  /// @brief Constructor taking a scroll delta.
  ///
  /// @param delta Raw scroll delta from the SFML mouse-wheel event.
  /////////////////////////////////////////////////
  explicit CameraPayload(float delta) : scroll_delta(delta) {}
};

using EventPayload = std::variant<std::monostate, InputPayload, UIPayload,
                                  LogicPayload, ScenePayload, SystemPayload,
                                  GhostPayload, CameraPayload>;

} // namespace steamrot
