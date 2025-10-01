/////////////////////////////////////////////////
/// @file
/// @brief Example Logic class header demonstrating best practices
///
/// This file shows the recommended structure for Logic class headers:
/// - Proper inheritance from Logic base class
/// - Private ProcessLogic() method
/// - Public constructor with LogicContext parameter
/// - Doxygen documentation
/// - Visual dividers between sections
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Logic.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @class ExampleLogic
/// @brief Example Logic class that demonstrates standard patterns.
///
/// This Logic class shows common patterns used in the game engine:
/// - Processing entities with specific components
/// - Interacting with game systems (events, rendering)
/// - Handling edge cases gracefully
///
/// ExampleLogic would typically belong to a LogicType category:
/// - Collision: For spatial interactions
/// - Render: For drawing operations
/// - Action: For input processing and event triggering
/// - Movement: For position updates
/////////////////////////////////////////////////
class ExampleLogic : public Logic {

private:
  /////////////////////////////////////////////////
  /// @brief Override method to encapsulate all logic operations.
  ///
  /// This method is called by RunLogic() in the base class.
  /// It should contain all the logic processing for this system.
  /// 
  /// Common operations include:
  /// - Finding archetypes with required components
  /// - Iterating through entities in archetypes
  /// - Reading/modifying component state
  /// - Interacting with game systems via m_logic_context
  /////////////////////////////////////////////////
  void ProcessLogic() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for ExampleLogic.
  ///
  /// @param logic_context LogicContext object containing references to
  ///                      game systems and scene data needed for logic
  ///                      processing.
  ///
  /// The LogicContext provides access to:
  /// - scene_entities: EntityMemoryPool for the scene
  /// - archetypes: Map of available component combinations
  /// - scene_texture: RenderTexture for drawing
  /// - game_window: Main game window
  /// - asset_manager: Game assets (textures, fonts, etc.)
  /// - event_handler: Event system for game events
  /// - mouse_position: Current mouse position in window
  /////////////////////////////////////////////////
  ExampleLogic(const LogicContext logic_context);
};

} // namespace steamrot
