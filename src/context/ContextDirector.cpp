/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ContextDirector class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ContextDirector.h"
#include "FailInfo.h"
#include "LogicContextBuilder.h"

namespace steamrot {

/////////////////////////////////////////////////
// Initialize static member
/////////////////////////////////////////////////
std::unordered_map<SceneType, LogicContextBuilder>
    ContextDirector::s_logic_context_builders;

/////////////////////////////////////////////////
void ContextDirector::RegisterLogicContextBuilder(SceneType type,
                                                   LogicContextBuilder builder) {
  s_logic_context_builders[type] = builder;
}

/////////////////////////////////////////////////
std::expected<LogicContextBuilder, FailInfo>
ContextDirector::GetLogicContextBuilder(SceneType scene_type) {
  auto it = s_logic_context_builders.find(scene_type);
  if (it == s_logic_context_builders.end()) {
    return std::unexpected(FailInfo{
        FailMode::NotFound,
        "No LogicContextBuilder registered for the requested scene type"});
  }
  // Return a copy of the builder for further configuration
  return it->second;
}

/////////////////////////////////////////////////
std::expected<LogicContext, FailInfo>
ContextDirector::BuildLogicContext(SceneType scene_type) {
  // Get the builder for this scene type
  auto builder_result = GetLogicContextBuilder(scene_type);
  if (!builder_result.has_value()) {
    return std::unexpected(builder_result.error());
  }

  // Build and return the context
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
