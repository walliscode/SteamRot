/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the EngineState struct.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Subscriber.h"
#include <chrono>
#include <memory>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Performance metrics for engine monitoring.
/////////////////////////////////////////////////
struct PerformanceMetrics {
  float frame_time_ms{0.0f};
  float fps{0.0f};
  size_t total_frames{0};
  std::chrono::time_point<std::chrono::steady_clock> last_frame_time;
};

/////////////////////////////////////////////////
/// @brief Engine runtime state.
///
/// EngineState contains runtime operational data that changes during
/// engine execution. This includes execution control flags, subscriptions,
/// and performance tracking. Unlike EngineConfig, this state is mutable
/// and changes frequently during runtime.
///
/// EngineState can be configured from data (e.g., subscribers loaded
/// from configuration files) and is also modified during execution.
/////////////////////////////////////////////////
struct EngineState {
  /////////////////////////////////////////////////
  /// @brief Flag indicating if the engine should continue running.
  /////////////////////////////////////////////////
  bool running{false};

  /////////////////////////////////////////////////
  /// @brief Flag indicating if the engine is paused.
  /////////////////////////////////////////////////
  bool paused{false};

  /////////////////////////////////////////////////
  /// @brief All subscribers registered to the Engine.
  /////////////////////////////////////////////////
  std::vector<std::shared_ptr<Subscriber>> subscriptions;

  /////////////////////////////////////////////////
  /// @brief Flag indicating if quit was requested.
  /////////////////////////////////////////////////
  bool quit_requested{false};

  /////////////////////////////////////////////////
  /// @brief Performance tracking metrics.
  /////////////////////////////////////////////////
  PerformanceMetrics performance;
};

} // namespace steamrot
