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
/// Provides centralized management of context builders, allowing scenes to
/// register, retrieve, and build LogicContext instances. Uses static methods
/// and data (not singleton pattern) for simplicity and testability.
/////////////////////////////////////////////////
class ContextDirector {
private:
  /////////////////////////////////////////////////
  /// @brief Static map to register builders by scene type.
  /////////////////////////////////////////////////
  static std::unordered_map<SceneType, LogicContextBuilder>
      s_logic_context_builders;

  /////////////////////////////////////////////////
  /// @brief Deleted constructor - static class only.
  /////////////////////////////////////////////////
  ContextDirector() = delete;

public:
  /////////////////////////////////////////////////
  /// @brief Register a builder for a specific scene type.
  ///
  /// Stores a copy of the builder in the static registry. If a builder
  /// already exists for the given scene type, it will be replaced.
  ///
  /// @param type Scene type to register the builder for
  /// @param builder LogicContextBuilder to register (will be copied)
  /////////////////////////////////////////////////
  static void RegisterLogicContextBuilder(SceneType type,
                                           LogicContextBuilder builder);

  /////////////////////////////////////////////////
  /// @brief Get builder for a scene type.
  ///
  /// Returns a copy of the registered builder for further configuration.
  /// This allows callers to modify the builder without affecting the
  /// registered version.
  ///
  /// @param scene_type Scene type to get builder for
  /// @return Expected containing builder copy or FailInfo if not registered
  /////////////////////////////////////////////////
  static std::expected<LogicContextBuilder, FailInfo>
  GetLogicContextBuilder(SceneType scene_type);

  /////////////////////////////////////////////////
  /// @brief Build and return a LogicContext directly.
  ///
  /// Retrieves the registered builder for the scene type and calls Build()
  /// on it. This is a convenience method for one-step context creation.
  ///
  /// @param scene_type Scene type to build context for
  /// @return Expected containing LogicContext or FailInfo on error
  /////////////////////////////////////////////////
  static std::expected<LogicContext, FailInfo>
  BuildLogicContext(SceneType scene_type);

  /////////////////////////////////////////////////
  /// @brief Clear all registered builders.
  ///
  /// Removes all entries from the static registry. Useful for testing
  /// and cleanup between test cases.
  /////////////////////////////////////////////////
  static void ClearBuilders();

  /////////////////////////////////////////////////
  /// @brief Check if a builder is registered for a scene type.
  ///
  /// @param scene_type Scene type to check
  /// @return True if a builder is registered, false otherwise
  /////////////////////////////////////////////////
  static bool HasBuilder(SceneType scene_type);
};

} // namespace steamrot
