/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ITestDataProvider class and declaration of the
/// FlatbuffersTestDataProvider class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ITestDataProvider.h"
#include "TestMetaData.h"
#include "test_data_generated.h"
#include <variant>

// Forward declare EventHandler to avoid circular dependencies
namespace steamrot {
class EventHandler;
}

class FlatbuffersTestDataProvider : public ITestDataProvider {

private:
  /////////////////////////////////////////////////
  /// @brief Reference to EventHandler for entity configuration
  /////////////////////////////////////////////////
  steamrot::EventHandler &m_event_handler;

public:
  FlatbuffersTestDataProvider(std::filesystem::path obj_dir_path,
                              steamrot::EventHandler &event_handler);

  /////////////////////////////////////////////////
  /// @brief Provide all instances of TestData struct
  /////////////////////////////////////////////////
  std::expected<std::vector<steamrot::TestData>, steamrot::FailInfo>
  ProvideAllTestData() const override;

  /////////////////////////////////////////////////
  /// @brief Create a TestData instance from Flatbuffers TestData
  ///
  /// @param fbs_test_data Flatbuffers TestData instance
  /////////////////////////////////////////////////
  std::expected<steamrot::TestData, steamrot::FailInfo>
  CreateTestData(const steamrot::TestDataFbs *fbs_test_data) const;

  /////////////////////////////////////////////////
  /// @brief Configures a TestMetaData instance from Flatbuffers data
  ///
  /// @param test_meta_data TestMetaData instance to configure
  /// @param fbs_test_meta_data The Flatbuffers TestMetaData instance
  /////////////////////////////////////////////////
  std::expected<std::monostate, steamrot::FailInfo> ConfigureTestMetaData(
      steamrot::TestMetaData &test_meta_data,
      const steamrot::TestMetadataFbs *fbs_test_meta_data) const;

  /////////////////////////////////////////////////
  /// @brief Configures a SimulationData instance from Flatbuffers data
  ///
  /// @param simulation_data SimulationData instance to configure
  /// @param fbs_simulation_data The Flatbuffers SimulationData
  /////////////////////////////////////////////////
  std::expected<std::monostate, steamrot::FailInfo> ConfigureSimulationData(
      steamrot::SimulationData &simulation_data,
      const steamrot::SimulationDataFbs *fbs_simulation_data) const;

  /////////////////////////////////////////////////
  /// @brief Configures a single EngineSnapshot from Flatbuffers data
  ///
  /// @param engine_snapshot EngineSnapshot instance to configure
  /// @param fbs_engine_snapshot The Flatbuffers EngineSnapshotFbs instance
  /////////////////////////////////////////////////
  std::expected<std::monostate, steamrot::FailInfo> ConfigureEngineSnapshot(
      steamrot::EngineSnapshot &engine_snapshot,
      const steamrot::EngineSnapshotFbs *fbs_engine_snapshot) const;

  /////////////////////////////////////////////////
  /// @brief Configures expected engine snapshots map from Flatbuffers data
  ///
  /// @param expected_snapshots Map to populate with tick->snapshot pairs
  /// @param fbs_tick_snapshot_pairs Vector of Flatbuffers tick-snapshot pairs
  /////////////////////////////////////////////////
  std::expected<std::monostate, steamrot::FailInfo>
  ConfigureExpectedEngineSnapshots(
      std::map<size_t, steamrot::EngineSnapshot> &expected_snapshots,
      const flatbuffers::Vector<
          flatbuffers::Offset<steamrot::TickSnapshotPairFbs>> *
          fbs_tick_snapshot_pairs) const;
};
