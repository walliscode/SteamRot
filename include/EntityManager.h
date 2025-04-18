////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "ArchetypeManager.h"
#include "ComponentFlags.h"
#include "EntityConfigurationFactory.h"
#include "EntityMemoryPool.h"
#include <cstddef>

#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

class EntityManager {

private:
  ////////////////////////////////////////////////////////////
  // Member data
  ////////////////////////////////////////////////////////////

  std::vector<size_t> m_entities;
  std::vector<size_t> m_entities_to_add;
  std::vector<size_t> m_entities_to_remove;
  std::shared_ptr<EntityMemoryPool> m_pool;
  ArchetypeManager m_archetype_manager;
  EntityConfigurationFactory m_entity_configuration_factory;

  ////////////////////////////////////////////////////////////
  /// \brief Intialise all entities by scene name
  ///
  ////////////////////////////////////////////////////////////
  void InitialiseEntities(std::string scene_name);

public:
  ////////////////////////////////////////////////////////////
  /// \brief Default constructor
  ///
  ////////////////////////////////////////////////////////////
  EntityManager(const size_t &pool_size, const std::string &scene_name);

  ////////////////////////////////////////////////////////////
  /// \brief "activate" an entity by finding the next inactive entity index
  ///
  ////////////////////////////////////////////////////////////
  size_t AddEntity();

  ////////////////////////////////////////////////////////////
  /// |brief add entity to the waiting room for removal
  ///
  ////////////////////////////////////////////////////////////
  void RemoveEntity(size_t entity_index);

  ////////////////////////////////////////////////////////////
  /// |brief update the waiting rooms for addition and removal
  ///
  ////////////////////////////////////////////////////////////
  void UpdateWaitingRooms();

  ////////////////////////////////////////////////////////////
  /// |brief get the active components for an entity
  ///
  ////////////////////////////////////////////////////////////
  const SteamRot::ComponentFlags &GetComponentFlags(size_t entity_id);

  ////////////////////////////////////////////////////////////
  /// |brief reset a component at a given index to default values
  ///
  ////////////////////////////////////////////////////////////
  template <typename T>
  void ResetValues(T &component_vector, const size_t index) {

    // Get the type of the Component the vector holds
    using ComponentType = typename T::value_type;

    // Set the index to a default constructed Component type
    component_vector[index] = ComponentType();
  };

  ////////////////////////////////////////////////////////////
  /// |brief reset all components in a tuple at a given index
  ///
  ////////////////////////////////////////////////////////////
  template <typename TupleT, std::size_t... tuple_index_sequence>
  void ResetTupleElements(TupleT &component_tuple,
                          std::index_sequence<tuple_index_sequence...>,
                          const size_t index) {

    (ResetValues(std::get<tuple_index_sequence>(component_tuple), index), ...);
  }

  ////////////////////////////////////////////////////////////
  /// |brief refresh all components in a tuple at a given index
  ///
  ////////////////////////////////////////////////////////////
  template <typename TupleT, std::size_t TupleSize = std::tuple_size_v<TupleT>>
  void RefreshEntity(TupleT &component_tuple, const size_t index) {

    ResetTupleElements(component_tuple, std::make_index_sequence<TupleSize>{},
                       index);
  }

  ////////////////////////////////////////////////////////////
  /// |brief get the a component at a given index
  ///
  ////////////////////////////////////////////////////////////
  template <typename T> T &GetComponent(size_t entity_id) {
    auto &components = std::get<std::vector<T>>(*(m_pool->getData()));
    return components[entity_id];
  }

  ////////////////////////////////////////////////////////////
  /// |brief check if a component is active for a given entity
  ///
  ////////////////////////////////////////////////////////////
  template <typename T> bool HasComponent(size_t entityID) {
    auto &components = std::get<std::vector<T>>(m_pool->getData());

    return components[entityID].getHas();
  }
};
