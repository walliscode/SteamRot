/////////////////////////////////////////////////
/// @file
/// @brief FlatBuffers implementation of IEngineDataProvider.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "IEngineDataProvider.h"
#include "FlatbuffersDataLoader.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersEngineDataProvider
/// @brief FlatBuffers implementation of IEngineDataProvider.
///
/// Loads engine data from FlatBuffers binary files and converts
/// to native C++ structs for use by game code.
/////////////////////////////////////////////////
class FlatbuffersEngineDataProvider : public IEngineDataProvider {
private:
  FlatbuffersDataLoader m_loader;

public:
  FlatbuffersEngineDataProvider() = default;

  std::expected<EngineCoreData, FailInfo>
  LoadEngineCoreData() const override;

  std::expected<EngineData, FailInfo> LoadEngineData() const override;
};

} // namespace steamrot
