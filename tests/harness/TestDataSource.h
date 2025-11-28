/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the TestDataSource class.
///
/// TestDataSource implements IEntityDataSource to load entity data
/// from TestDataConfig (used by the test harness).
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "IEntityDataSource.h"
#include "test_data_generated.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @class TestDataSource
/// @brief Loads entity data from test data configuration.
///
/// This data source is used by TestEngine to load entities from
/// TestDataConfig's start_data_collection field.
///
/// Note: This wraps a pointer to TestDataConfig, so the config
/// must remain valid for the lifetime of this data source.
/////////////////////////////////////////////////
class TestDataSource : public IEntityDataSource {
private:
  /////////////////////////////////////////////////
  /// @brief Pointer to the test data configuration.
  /////////////////////////////////////////////////
  const TestDataConfig *m_config;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor taking the test data configuration.
  ///
  /// @param config Pointer to the test data configuration
  /////////////////////////////////////////////////
  explicit TestDataSource(const TestDataConfig *config);

  /////////////////////////////////////////////////
  /// @brief Get entity collection data from the test config.
  ///
  /// Returns the entity collection from start_data_collection.
  ///
  /// @return Pointer to EntityCollection, or nullptr if unavailable
  /////////////////////////////////////////////////
  const EntityCollection *GetEntityCollection() const override;

  /////////////////////////////////////////////////
  /// @brief Get the number of entities in this data source.
  ///
  /// @return Number of entities, or 0 if unavailable
  /////////////////////////////////////////////////
  size_t GetEntityCount() const override;

  /////////////////////////////////////////////////
  /// @brief Get the source identifier for logging/debugging.
  ///
  /// Returns a string including the test name if available.
  ///
  /// @return String identifying this test data source
  /////////////////////////////////////////////////
  std::string GetSourceIdentifier() const override;

  /////////////////////////////////////////////////
  /// @brief Get the underlying test data configuration.
  ///
  /// @return Pointer to the test data configuration
  /////////////////////////////////////////////////
  const TestDataConfig *GetTestConfig() const;
};

} // namespace steamrot::tests
