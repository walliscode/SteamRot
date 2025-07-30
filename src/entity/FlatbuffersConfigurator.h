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
#include "FlatbuffersDataLoader.h"
#include "containers.h"
#include "entities_generated.h"

namespace steamrot {
class FlatbuffersConfigurator : public EntityConfigurator {

private:
  /////////////////////////////////////////////////
  /// @brief FlatbuffersDataLoader instance for providing flatbuffers data
  /////////////////////////////////////////////////
  FlatbuffersDataLoader m_data_loader;

public:
  FlatbuffersConfigurator(const EnvironmentType env_type);

  void ConfigureEntitiesFromDefaultData(
      components::containers::EntityMemoryPool &entity_memory_pool,
      const EntityCollection *entity_collection);
};

} // namespace steamrot
