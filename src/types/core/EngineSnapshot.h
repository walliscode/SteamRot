/////////////////////////////////////////////////
/// @file
/// @brief Declaration of EngineSnapshot struct
/////////////////////////////////////////////////
///
/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPacket.h"
#include "SceneLoadData.h"
#include "SceneManagerData.h"
#include <optional>
#include <vector>

namespace steamrot {

// Forward declaration for EventBus type
using EventBus = std::vector<EventPacket>;

/////////////////////////////////////////////////
/// @struct EngineSnapshot
/// @brief Extensible container for complete engine state snapshot
///
/// This struct is designed for data-driven testing and captures the complete
/// state of the engine at a specific tick. It uses std::optional for all
/// fields to allow selective comparison - tests can verify only the aspects
/// they care about.
///
/// **Extensibility by Design:**
/// - Add new optional fields for any engine-level data structure
/// - Tests only populate/compare fields relevant to their scenario
/// - No breaking changes when adding new snapshot capabilities
///
/// **Current Snapshot Capabilities:**
/// - Scene entity state (EntityMemoryPool per scene, keyed by UUID)
/// - Global event bus state
/// - Tick number for context
///
/// **Future Extensions:**
/// - Scene manager state
/// - Asset manager cache state
/// - Performance metrics
/// - Audio state
/// - Input state
/// - Any other engine-level data structures
/////////////////////////////////////////////////
struct EngineSnapshot {

  /////////////////////////////////////////////////
  /// @brief Default constructor
  /////////////////////////////////////////////////
  EngineSnapshot() = default;

  /////////////////////////////////////////////////
  /// @brief Optional tick number for context
  /////////////////////////////////////////////////
  std::optional<size_t> tick_number;

  /////////////////////////////////////////////////
  /// @brief Global event bus state
  ///
  /// Captures all events in the global event bus at this tick.
  /// Useful for testing event lifetime, propagation, and processing.
  /////////////////////////////////////////////////
  std::optional<EventBus> global_event_bus;

  /////////////////////////////////////////////////
  /// @brief Contains all data needed to save and restore the state of the
  /// SceneManager.
  ///
  /// This is optional because it can also be loaded from defaults
  /////////////////////////////////////////////////
  std::optional<SceneManagerData> scene_manager_data;

  /////////////////////////////////////////////////
  /// @brief SceneLoadData for all scenes in the collection
  /////////////////////////////////////////////////
  SceneCollectionData scene_collection_data;
};

} // namespace steamrot
