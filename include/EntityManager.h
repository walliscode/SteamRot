#pragma once
#include "ArchetypeManager.h"
#include "ComponentFlags.h"
#include "EntityMemoryPool.h"
#include <cstddef>

#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

class EntityManager {

private:
  // the EntityManager will be responsible for adding and removing entities
  // most of its members will relove around managing this (waiting rooms)
  std::vector<size_t> m_entities;
  std::vector<size_t> m_entities_to_add;
  std::vector<size_t> m_entities_to_remove;
  std::shared_ptr<EntityMemoryPool> m_pool;

  void InitialiseEntities(std::string scene_name);

  ArchetypeManager m_archetype_manager;

public:
  EntityManager(const size_t &pool_size, const std::string &scene_name);

  std::vector<size_t> GetEntities();
  size_t AddEntity();
  void RemoveEntity(size_t entity_index);
  void UpdateWaitingRooms();

  std::vector<int> getVector() {
    std::vector<int> vec = {1, 2, 3, 4};
    return vec;
  }
  // i envisage getting the CMeta Component Flags often enough that is worth a
  // convenience function
  const SteamRot::ComponentFlags &GetComponentFlags(size_t entity_id);

  // these template functions are designed to act upon the
  // ComponentCollectionTuple so they will (working from smallest to largest)
  // act upon each type of each vector (the vector will contain classes
  // inherited from Component) of the tuple

  // reset indivudual components
  template <typename T>
  void ResetValues(T &component_vector, const size_t index) {

    // Get the type of the Component the vector holds
    using ComponentType = typename T::value_type;

    // Set the index to a default constructed Component type
    component_vector[index] = ComponentType();
  };

  // go through each component at that index and reset
  template <typename TupleT, std::size_t... tuple_index_sequence>
  void ResetTupleElements(TupleT &component_tuple,
                          std::index_sequence<tuple_index_sequence...>,
                          const size_t index) {

    (ResetValues(std::get<tuple_index_sequence>(component_tuple), index), ...);
  }

  // wrapper function for resetting tuple elements. Can add other sequences here
  template <typename TupleT, std::size_t TupleSize = std::tuple_size_v<TupleT>>
  void RefreshEntity(TupleT &component_tuple, const size_t index) {

    ResetTupleElements(component_tuple, std::make_index_sequence<TupleSize>{},
                       index);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // all functions related to getting component data
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  template <typename T> T &GetComponent(size_t entity_id) {
    auto &components = std::get<std::vector<T>>(*(m_pool->getData()));
    return components[entity_id];
  }

  template <typename T> bool hasComponent(size_t entityID) {
    auto &components = std::get<std::vector<T>>(m_pool->getData());

    return components[entityID].getHas();
  }

  // extracting data to json

  json toJSON(); // extract data to a json object
};
