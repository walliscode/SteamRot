/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the ContextDirector class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "LogicContext.h"
#include "LogicContextBuilder.h"
#include "scene_change_packet_generated.h"
#include <expected>
#include <unordered_map>

namespace steamrot {

/////////////////////////////////////////////////
/// @class ContextDirector
/// @brief Static registry for managing LogicContextBuilder instances by scene type.
///
/// Provides centralized context lifecycle management using a static registry pattern.
/// Not a singleton - uses static methods and data for simplicity and testability.
/////////////////////////////////////////////////
class ContextDirector {
private:
  /////////////////////////////////////////////////
  /// @brief Static map to register builders by scene type
  /////////////////////////////////////////////////
  static std::unordered_map<SceneType, LogicContextBuilder>
      s_logic_context_builders;

  /////////////////////////////////////////////////
  /// @brief Deleted constructor - this class only has static members
  /////////////////////////////////////////////////
  ContextDirector() = delete;

public:
  /////////////////////////////////////////////////
  /// @brief Register a builder for a specific scene type
  ///
  /// @param type Scene type to register the builder for
  /// @param builder LogicContextBuilder to register (will be copied)
  /////////////////////////////////////////////////
  static void RegisterLogicContextBuilder(SceneType type,
                                           LogicContextBuilder builder);

  /////////////////////////////////////////////////
  /// @brief Get builder for a scene type (returns copy for further configuration)
  ///
  /// @param scene_type Scene type to get the builder for
  /// @return Expected containing copy of the builder or FailInfo on error
  /////////////////////////////////////////////////
  static std::expected<LogicContextBuilder, FailInfo>
  GetLogicContextBuilder(SceneType scene_type);

  /////////////////////////////////////////////////
  /// @brief Build and return a LogicContext directly
  ///
  /// @param scene_type Scene type to build the context for
  /// @return Expected containing the built LogicContext or FailInfo on error
  /////////////////////////////////////////////////
  static std::expected<LogicContext, FailInfo>
  BuildLogicContext(SceneType scene_type);

  /////////////////////////////////////////////////
  /// @brief Clear all registered builders (useful for testing)
  /////////////////////////////////////////////////
  static void ClearBuilders();

  /////////////////////////////////////////////////
  /// @brief Check if a builder is registered for a scene type
  ///
  /// @param scene_type Scene type to check
  /// @return True if a builder is registered, false otherwise
  /////////////////////////////////////////////////
  static bool HasBuilder(SceneType scene_type);
};

} // namespace steamrot
