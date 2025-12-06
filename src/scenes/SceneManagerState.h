/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SceneManagerState struct.
/////////////////////////////////////////////////

#pragma once

#include "Subscriber.h"
#include "events_generated.h"
#include <memory>
#include <unordered_map>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct SceneManagerState
/// @brief SceneManager runtime state.
///
/// SceneManagerState contains runtime operational data that changes during
/// SceneManager execution. This includes event subscriptions and other
/// mutable state. Unlike SceneManagerConfig, this state is mutable
/// and changes frequently during runtime.
/////////////////////////////////////////////////
struct SceneManagerState {
  SceneManagerState() = default;

  /////////////////////////////////////////////////
  /// @brief Map of all event subscriptions, stored by event type
  /////////////////////////////////////////////////
  std::unordered_map<EventType, std::shared_ptr<Subscriber>> subscriptions;

  // Future: additional runtime state
  // - active scene tracking
  // - scene loading state
  // - transition state
};

} // namespace steamrot
