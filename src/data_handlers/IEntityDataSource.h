/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the IEntityDataSource interface
///
/// This interface abstracts WHERE entity data comes from, allowing
/// both game and test code to use the same data loading path.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "entities_generated.h"
#include <cstddef>
#include <string>

namespace steamrot {

/////////////////////////////////////////////////
/// @interface IEntityDataSource
/// @brief Interface for entity data sources - determines data paths
///
/// This interface provides a uniform way to access entity data
/// regardless of whether it comes from:
/// - Default scene data files (game)
/// - Test data configuration (tests)
/// - Other sources (future extensibility)
///
/// Implementations determine WHERE data comes from, while
/// FlatbuffersConfigurator determines HOW data is interpreted.
/////////////////////////////////////////////////
class IEntityDataSource {
public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor for proper cleanup
  /////////////////////////////////////////////////
  virtual ~IEntityDataSource() = default;

  /////////////////////////////////////////////////
  /// @brief Get entity collection data from this source
  ///
  /// @return Pointer to EntityCollection, or nullptr if unavailable
  /////////////////////////////////////////////////
  virtual const EntityCollection *GetEntityCollection() const = 0;

  /////////////////////////////////////////////////
  /// @brief Get the number of entities in this data source
  ///
  /// @return Number of entities, or 0 if unavailable
  /////////////////////////////////////////////////
  virtual size_t GetEntityCount() const = 0;

  /////////////////////////////////////////////////
  /// @brief Get the source identifier (for logging/debugging)
  ///
  /// @return Human-readable string identifying this data source
  /////////////////////////////////////////////////
  virtual std::string GetSourceIdentifier() const = 0;
};

} // namespace steamrot
