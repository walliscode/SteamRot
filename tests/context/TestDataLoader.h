/////////////////////////////////////////////////
/// @file
/// @brief Declaration of TestDataLoader class for loading test data from FlatBuffers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "ArchetypeManager.h"
#include "EventHandler.h"
#include "FailInfo.h"
#include "PathProvider.h"
#include "containers.h"
#include "entity_memory.h"
#include "test_data_generated.h"
#include <expected>
#include <memory>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Loads and manages test data from FlatBuffers
///
/// This class provides functionality to load test data from
/// JSON files (converted to binary via FlatBuffers) and configure
/// entity memory pools, event buses, and other game state for testing.
/////////////////////////////////////////////////
class TestDataLoader {
private:
  /////////////////////////////////////////////////
  /// @brief Root test data loaded from file
  /////////////////////////////////////////////////
  std::unique_ptr<uint8_t[]> m_buffer;
  const TestDataFile *m_test_data;

  /////////////////////////////////////////////////
  /// @brief Path provider for locating test data files
  /////////////////////////////////////////////////
  PathProvider &m_path_provider;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for TestDataLoader
  ///
  /// @param path_provider PathProvider configured for test environment
  /////////////////////////////////////////////////
  explicit TestDataLoader(PathProvider &path_provider);

  /////////////////////////////////////////////////
  /// @brief Load test data from a binary file
  ///
  /// Loads test data from a FlatBuffers binary file.
  ///
  /// @param filename Name of the binary file (without path)
  /// @return Expected with monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  LoadFromBinary(const std::string &filename);

  /////////////////////////////////////////////////
  /// @brief Get the loaded test case
  ///
  /// @return Pointer to TestCase, or nullptr if not loaded
  /////////////////////////////////////////////////
  const TestCase *GetTestCase() const;

  /////////////////////////////////////////////////
  /// @brief Get test metadata
  ///
  /// @return Pointer to TestMetadata, or nullptr if not loaded
  /////////////////////////////////////////////////
  const TestMetadata *GetMetadata() const;

  /////////////////////////////////////////////////
  /// @brief Configure EntityMemoryPool from before state
  ///
  /// Populates an EntityMemoryPool with data from the test's before state.
  ///
  /// @param pool EntityMemoryPool to configure
  /// @return Expected with monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigurePoolFromBeforeState(EntityMemoryPool &pool) const;

  /////////////////////////////////////////////////
  /// @brief Configure EntityMemoryPool from after state
  ///
  /// Populates an EntityMemoryPool with data from the test's after state.
  ///
  /// @param pool EntityMemoryPool to configure
  /// @return Expected with monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigurePoolFromAfterState(EntityMemoryPool &pool) const;

  /////////////////////////////////////////////////
  /// @brief Configure EntityMemoryPool from intermediate state
  ///
  /// Populates an EntityMemoryPool with data from an intermediate state
  /// at a specific index.
  ///
  /// @param pool EntityMemoryPool to configure
  /// @param state_index Index of the intermediate state
  /// @return Expected with monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigurePoolFromIntermediateState(EntityMemoryPool &pool,
                                     size_t state_index) const;

  /////////////////////////////////////////////////
  /// @brief Get the number of intermediate states
  ///
  /// @return Number of intermediate states in the test case
  /////////////////////////////////////////////////
  size_t GetIntermediateStateCount() const;

  /////////////////////////////////////////////////
  /// @brief Check if test data is loaded
  ///
  /// @return true if test data is loaded, false otherwise
  /////////////////////////////////////////////////
  bool IsLoaded() const;

private:
  /////////////////////////////////////////////////
  /// @brief Configure EntityMemoryPool from a test state snapshot
  ///
  /// @param pool EntityMemoryPool to configure
  /// @param snapshot TestStateSnapshot containing the state data
  /// @return Expected with monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigurePoolFromSnapshot(EntityMemoryPool &pool,
                            const TestStateSnapshot *snapshot) const;

  /////////////////////////////////////////////////
  /// @brief Configure EntityMemoryPool from EntityCollection
  ///
  /// @param pool EntityMemoryPool to configure
  /// @param entity_collection EntityCollection data
  /// @return Expected with monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigurePoolFromEntityCollection(
      EntityMemoryPool &pool, const EntityCollection *entity_collection) const;
};

} // namespace steamrot::tests
