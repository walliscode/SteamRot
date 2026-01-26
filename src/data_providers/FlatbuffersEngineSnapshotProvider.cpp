/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersEngineSnapshotProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEngineSnapshotProvider.h"
#include "configure_engine_snapshot.h"

namespace steamrot {

/////////////////////////////////////////////////
FlatbuffersEngineSnapshotProvider::FlatbuffersEngineSnapshotProvider(
    EventHandler &event_handler)
    : m_event_handler(event_handler) {}

/////////////////////////////////////////////////
FlatbuffersEngineSnapshotProvider::FlatbuffersEngineSnapshotProvider(
    EventHandler &event_handler, const EngineSnapshotFbs *engine_snapshot_fbs)
    : m_event_handler(event_handler),
      m_engine_snapshot_fbs(engine_snapshot_fbs) {}

/////////////////////////////////////////////////
std::expected<EngineSnapshot, FailInfo>
FlatbuffersEngineSnapshotProvider::CreateEngineSnapshot() const {

  // Create EngineSnapshot object
  EngineSnapshot engine_snapshot;

  // Configure EngineSnapshot
  auto configure_result = ConfigureEngineSnapshot(engine_snapshot);
  if (!configure_result.has_value()) {
    return std::unexpected(configure_result.error());
  }

  return engine_snapshot;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersEngineSnapshotProvider::ConfigureEngineSnapshot(
    EngineSnapshot &engine_snapshot) const {

  // Validate that m_engine_snapshot_fbs is not null
  if (!m_engine_snapshot_fbs) {
    return std::unexpected(
        FailInfo{FailMode::NullPointer,
                 "FlatBuffers EngineSnapshotFbs pointer is null"});
  }

  // Call the free function to configure EngineSnapshot
  auto configure_result = data::configure::ConfigureEngineSnapshot(
      engine_snapshot, m_engine_snapshot_fbs);
  if (!configure_result.has_value()) {
    return std::unexpected(configure_result.error());
  }

  return std::monostate{};
}

} // namespace steamrot
