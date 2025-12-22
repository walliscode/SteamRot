/////////////////////////////////////////////////
/// @file
/// @brief Declaration of SaveData struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EngineState.h"
#include "SaveMetadata.h"
#include "SceneData.h"
#include "SceneManagerState.h"
#include "SceneState.h"
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct SaveData
/// @brief Native C++ struct containing complete game save data.
///
/// SaveData is the native representation of a complete game save,
/// containing all state necessary to restore a game session:
/// - Metadata (display info, timestamps)
/// - Scene data (polymorphic via SceneData base class)
/// - Engine state (running flags, subscriptions)
/// - Scene manager state (scene subscriptions)
/// - Scene state (active status)
///
/// Design:
/// - Polymorphic types (SceneData) are stored as unique_ptr
/// - Simple copyable types (EngineState, etc.) are stored by value
/// - Easily extensible with new fields
///
/// Usage with Providers:
/// ```cpp
/// ISaveDataProvider& provider = GetSaveDataProvider();
/// auto result = provider.LoadSaveData(slot_index);
/// if (result.has_value()) {
///   const SaveData& save = result.value();
///   // Use save.scene_data, save.engine_state, etc.
/// }
/// ```
/////////////////////////////////////////////////
struct SaveData {
  /////////////////////////////////////////////////
  /// @brief Metadata about the save file
  ///
  /// Contains display name, timestamps, version info for UI and management.
  /////////////////////////////////////////////////
  SaveMetadata metadata;

  /////////////////////////////////////////////////
  /// @brief Scene data (polymorphic pointer)
  ///
  /// Points to concrete implementation (e.g., FbsSceneData for FlatBuffers).
  /// This allows different scene data formats without changing SaveData.
  /////////////////////////////////////////////////
  std::unique_ptr<SceneData> scene_data;

  /////////////////////////////////////////////////
  /// @brief Engine state at time of save
  ///
  /// Includes running/paused flags, subscriptions, and performance metrics.
  /////////////////////////////////////////////////
  EngineState engine_state;

  /////////////////////////////////////////////////
  /// @brief Scene manager state at time of save
  ///
  /// Includes scene manager subscriptions.
  /////////////////////////////////////////////////
  SceneManagerState scene_manager_state;

  /////////////////////////////////////////////////
  /// @brief Scene state at time of save
  ///
  /// Includes active status and other scene runtime flags.
  /////////////////////////////////////////////////
  SceneState scene_state;

  // Future extensions:
  // std::vector<InventoryItem> player_inventory;
  // QuestProgress quest_progress;
  // std::unordered_map<std::string, bool> game_flags;
};

} // namespace steamrot
