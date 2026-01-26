/////////////////////////////////////////////////
/// @file
/// @brief Declaration of FlatbuffersEngineSnapshotProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventHandler.h"
#include "IEngineSnapShotProvider.h"
#include "engine_snapshot_generated.h"
#include <expected>

namespace steamrot {

class FlatbuffersEngineSnapshotProvider
    : public steamrot::IEngineSnapShotProvider {

private:
  /////////////////////////////////////////////////
  /// @brief Reference to EventHandler
  /////////////////////////////////////////////////
  EventHandler &m_event_handler;

  /////////////////////////////////////////////////
  /// @brief Pointer to FlatBuffers EngineSnapshotFbs
  /////////////////////////////////////////////////
  const EngineSnapshotFbs *m_engine_snapshot_fbs{nullptr};

public:
  /////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param event_handler Reference to EventHandler for event reporting.
  /////////////////////////////////////////////////
  FlatbuffersEngineSnapshotProvider(EventHandler &event_handler);

  /////////////////////////////////////////////////
  /// @brief Constructor with FlatBuffers data
  ///
  /// @param event_handler Reference to EventHandler for event reporting.
  /// @param engine_snapshot_fbs Pointer to FlatBuffers EngineSnapshotFbs.
  /////////////////////////////////////////////////
  FlatbuffersEngineSnapshotProvider(EventHandler &event_handler,
                                    const EngineSnapshotFbs *engine_snapshot_fbs);

  /////////////////////////////////////////////////
  /// @brief Create and provide a configured EngineSnapshot object.
  ///
  /// @return EngineSnapshot object or FailInfo on error.
  /////////////////////////////////////////////////
  std::expected<EngineSnapshot, FailInfo>
  CreateEngineSnapshot() const override;

  /////////////////////////////////////////////////
  /// @brief Configure the provided EngineSnapshot object.
  ///
  /// @param engine_snapshot EngineSnapshot object to configure.
  /// @return std::monostate on success or FailInfo on error.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureEngineSnapshot(EngineSnapshot &engine_snapshot) const override;
};
} // namespace steamrot
