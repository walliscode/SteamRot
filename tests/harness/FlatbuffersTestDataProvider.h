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

class FlatbuffersTestDataProvider : public ITestDataProvider {

public:
  FlatbuffersTestDataProvider(std::filesystem::path obj_dir_path);

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
};
