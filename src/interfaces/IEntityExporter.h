/////////////////////////////////////////////////
/// @file
/// @brief Declaration of IEntityExporter interface
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
#include <cstdint>
#include <expected>
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
/// @class IEntityExporter
/// @brief Interface for exporting EntityMemoryPool to serialized format.
///
/// Implementations convert runtime entity data to specific formats
/// (FlatBuffers, JSON, etc.) for saving or transmission.
/////////////////////////////////////////////////
class IEntityExporter {
public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor
  /////////////////////////////////////////////////
  virtual ~IEntityExporter() = default;

  /////////////////////////////////////////////////
  /// @brief Export entities to serialized binary format.
  ///
  /// @param emp EntityMemoryPool containing entities to export
  /// @param out_size Output parameter for binary data size
  /// @return Unique pointer to binary data or FailInfo on error
  /////////////////////////////////////////////////
  virtual std::expected<std::unique_ptr<uint8_t[]>, FailInfo>
  ExportEntities(const EntityMemoryPool &emp, size_t &out_size) = 0;
};

} // namespace steamrot
