/////////////////////////////////////////////////
/// @file
/// @brief FlatBuffers implementation of IGameConfigProvider.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include "IGameConfigProvider.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersGameConfigProvider
/// @brief FlatBuffers implementation of IGameConfigProvider.
///
/// Wraps FlatbuffersDataLoader to provide game configuration data.
/// Returns FlatBuffers types directly (not converted to native structs).
///
/// NOTE: This is a temporary provider for Phase 3 of the migration.
/// Phase 4 will convert these to native structs and replace this provider.
/////////////////////////////////////////////////
class FlatbuffersGameConfigProvider : public IGameConfigProvider {
private:
  /////////////////////////////////////////////////
  /// @brief Internal FlatBuffers data loader
  /////////////////////////////////////////////////
  FlatbuffersDataLoader m_loader;

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor
  /////////////////////////////////////////////////
  FlatbuffersGameConfigProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Load complete engine configuration.
  ///
  /// @return EngineDataFbs pointer or failure information
  /////////////////////////////////////////////////
  std::expected<const EngineDataFbs *, FailInfo>
  LoadEngineConfig() const override;

  /////////////////////////////////////////////////
  /// @brief Load scene manager configuration.
  ///
  /// @return SceneManagerData pointer or failure information
  /////////////////////////////////////////////////
  std::expected<const SceneManagerData *, FailInfo>
  LoadSceneManagerConfig() const override;
};

} // namespace steamrot
