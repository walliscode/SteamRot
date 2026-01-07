/////////////////////////////////////////////////
/// @file
/// @brief Declaration of IEntityImporter interface
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "containers.h"
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @class IEntityImporter
/// @brief Interface for importing entity data into EntityMemoryPool.
///
/// Implementations read from various sources and configure runtime entities.
/// The importer wraps the data source and provides a format-agnostic
/// interface for entity import operations.
/////////////////////////////////////////////////
class IEntityImporter {
public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor
  /////////////////////////////////////////////////
  virtual ~IEntityImporter() = default;

  /////////////////////////////////////////////////
  /// @brief Import entities into the given EntityMemoryPool.
  ///
  /// @param emp EntityMemoryPool to populate with entity data
  /// @return std::monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ImportEntities(EntityMemoryPool &emp) = 0;
};

} // namespace steamrot
