////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "DataLoader.h"
#include "containers.h"
#include "entities_generated.h"
#include <memory>
#include <nlohmann/json.hpp>

namespace steamrot {

class EntityConfigurationFactory {
private:
  /////////////////////////////////////////////////
  /// @brief shared pointer to derived data loader instance
  /////////////////////////////////////////////////
  std::shared_ptr<DataLoader> m_data_loader;

  /////////////////////////////////////////////////
  /// @brief Overload function to configure CGrimoireMachina component
  ///
  /// @param grimoire CGrimoireMachina component to configure
  /////////////////////////////////////////////////
  void ConfigureComponentSpecifics(CGrimoireMachina &grimoire);

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for EntityConfigurationFactory
  ///
  /// @param data_loader shared pointer to instance of derived DataLoader object
  /////////////////////////////////////////////////
  EntityConfigurationFactory(std::shared_ptr<DataLoader> data_loader);

  /////////////////////////////////////////////////
  /// @brief Interface function for configuring any Component
  ///
  /// @param component Component to configure
  /////////////////////////////////////////////////
  void ConfigureComponent(Component &component);
  ////////////////////////////////////////////////////////////
  /// |brief Configure entities (overload this function per data type)
  ///
  ////////////////////////////////////////////////////////////

  void ConfigureEntitiesFromDefaultData(
      components::containers::EntityMemoryPool &entity_memory_pool,
      const EntityCollection *entities_collection);
};

} // namespace steamrot
