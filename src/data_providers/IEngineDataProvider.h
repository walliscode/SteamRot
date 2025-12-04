/////////////////////////////////////////////////
/// @file
/// @brief Interface for loading engine configuration data.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include <cstdint>
#include <expected>
#include <string>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct EngineCoreData
/// @brief Native C++ struct for engine core configuration.
///
/// This replaces the FlatBuffers EngineCoreData type in game code.
/// Provides a simple, mutable structure for engine configuration.
/////////////////////////////////////////////////
struct EngineCoreData {
  uint32_t window_width{800};
  uint32_t window_height{600};
  std::string window_title{"SteamRot"};
  uint32_t framerate_limit{60};
};

/////////////////////////////////////////////////
/// @struct EngineData
/// @brief Complete engine data including core and other subsystems.
/////////////////////////////////////////////////
struct EngineData {
  EngineCoreData core;
  // Future: Add other engine-level configuration
};

/////////////////////////////////////////////////
/// @class IEngineDataProvider
/// @brief Interface for loading engine configuration data.
///
/// Implementations handle the actual data source (files, network, etc.)
/// and format (FlatBuffers, JSON, XML, Lua, etc.).
///
/// Usage:
/// ```cpp
/// IEngineDataProvider& provider = GetEngineDataProvider();
/// auto result = provider.LoadEngineCoreData();
/// if (result.has_value()) {
///   const EngineCoreData& data = result.value();
///   // Use native C++ struct
/// }
/// ```
/////////////////////////////////////////////////
class IEngineDataProvider {
public:
  virtual ~IEngineDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Load engine core configuration.
  ///
  /// @return Engine core data or failure information
  /////////////////////////////////////////////////
  virtual std::expected<EngineCoreData, FailInfo>
  LoadEngineCoreData() const = 0;

  /////////////////////////////////////////////////
  /// @brief Load complete engine data.
  ///
  /// @return Engine data or failure information
  /////////////////////////////////////////////////
  virtual std::expected<EngineData, FailInfo> LoadEngineData() const = 0;
};

} // namespace steamrot
