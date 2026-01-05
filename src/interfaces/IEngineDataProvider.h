/////////////////////////////////////////////////
/// @file
/// @brief Interface for loading engine data.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EngineData.h"
#include "FailInfo.h"
#include <expected>

// Forward declaration
namespace steamrot {
class FlatbuffersSubscriberViewer;
}

namespace steamrot {

/////////////////////////////////////////////////
/// @class IEngineDataProvider
/// @brief Interface for loading all engine data types.
/////////////////////////////////////////////////
class IEngineDataProvider {

public:
  virtual ~IEngineDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Load engine data from file
  /////////////////////////////////////////////////
  virtual std::expected<EngineData, FailInfo> LoadEngineData() const = 0;

  /////////////////////////////////////////////////
  /// @brief Convert FlatBuffers EngineDataFbs to native EngineData.
  ///
  /// This method allows passing pre-loaded FlatBuffers data directly
  /// for conversion without file I/O.
  ///
  /// @param fb_engine_data Pointer to FlatBuffers EngineDataFbs
  /// @return Native EngineData object, or error
  /////////////////////////////////////////////////
  virtual std::expected<EngineData, FailInfo>
  ConvertEngineData(const EngineDataFbs *fb_engine_data) const = 0;
};

} // namespace steamrot
