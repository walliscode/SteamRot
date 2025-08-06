/////////////////////////////////////////////////
/// @file
/// @brief Declaration of FlatbuffersConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EntityConfigurator.h"
#include "FailInfo.h"
#include "FlatbuffersDataLoader.h"
#include "containers.h"

#include <expected>
#include <variant>

namespace steamrot {
class FlatbuffersConfigurator : public EntityConfigurator {

private:
  /////////////////////////////////////////////////
  /// @brief FlatbuffersDataLoader instance for providing flatbuffers data
  /////////////////////////////////////////////////
  FlatbuffersDataLoader m_data_loader;

public:
  FlatbuffersConfigurator(const EnvironmentType env_type);

  std::expected<std::monostate, FailInfo>
  ConfigureEntitiesFromDefaultData(EntityMemoryPool &entity_memory_pool,
                                   const SceneType scene_type);
};

} // namespace steamrot
