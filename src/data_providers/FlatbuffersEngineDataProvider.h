/////////////////////////////////////////////////
/// @file
/// @brief FlatBuffers implementation of IEngineDataProvider.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "FlatbuffersDataLoader.h"
#include "IEngineDataProvider.h"
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersEngineDataProvider
/// @brief FlatBuffers implementation of IEngineDataProvider.
///
/// Loads engine data from FlatBuffers binary files and converts
/// to native C++ structs for use by game code. Contains a
/// SubscriberDataViewer member to provide access to subscriber
/// configurations.
/////////////////////////////////////////////////
class FlatbuffersEngineDataProvider : public IEngineDataProvider {
private:
  /////////////////////////////////////////////////
  /// @brief Instance of FlatbuffersDataLoader for loading data.
  /////////////////////////////////////////////////
  FlatbuffersDataLoader m_loader;

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor.
  /////////////////////////////////////////////////
  FlatbuffersEngineDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Provides EngineData loaded from FlatBuffers binary files.
  /////////////////////////////////////////////////
  std::expected<EngineData, FailInfo> CreateEngineData() const override;

  /////////////////////////////////////////////////
  /// @brief Configures the provided EngineData.
  ///
  /// @param engine_data Engine data to configure
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureEngineData(EngineData &engine_data) const override;
};

} // namespace steamrot
