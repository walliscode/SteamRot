/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ContextDirector class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ContextDirector.h"

namespace steamrot {

/////////////////////////////////////////////////
// Initialize static member
/////////////////////////////////////////////////
std::unordered_map<SceneType, LogicContextBuilder>
    ContextDirector::s_logic_context_builders;

/////////////////////////////////////////////////
void ContextDirector::RegisterLogicContextBuilder(SceneType type,
                                                   LogicContextBuilder builder) {
  s_logic_context_builders[type] = std::move(builder);
}

/////////////////////////////////////////////////
std::expected<LogicContextBuilder, FailInfo>
ContextDirector::GetLogicContextBuilder(SceneType scene_type) {
  auto it = s_logic_context_builders.find(scene_type);

  if (it == s_logic_context_builders.end()) {
    return std::unexpected(FailInfo{
        FailMode::NonExistentEnumValue,
        "No LogicContextBuilder registered for scene type: " +
            std::to_string(static_cast<int>(scene_type))});
  }

  // Return a copy so caller can modify without affecting registry
  return it->second;
}

/////////////////////////////////////////////////
std::expected<LogicContext, FailInfo>
ContextDirector::BuildLogicContext(SceneType scene_type) {
  auto builder_result = GetLogicContextBuilder(scene_type);

  if (!builder_result.has_value()) {
    return std::unexpected(builder_result.error());
  }

  return builder_result.value().Build();
}

/////////////////////////////////////////////////
void ContextDirector::ClearBuilders() { s_logic_context_builders.clear(); }

/////////////////////////////////////////////////
bool ContextDirector::HasBuilder(SceneType scene_type) {
  return s_logic_context_builders.find(scene_type) !=
         s_logic_context_builders.end();
}

} // namespace steamrot
