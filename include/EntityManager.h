#pragma once
// #include "ArchetypeManager.h"
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
  std::vector<size_t>
      m_entitiesToRemove; // list of entities to remove next update
  std::shared_ptr<EntityMemoryPool> m_pool; // pool of all entity
  // data

  // ArchetypeManager m_archetypeManager;

public:
  EntityManager(const size_t &poolSize);

  std::vector<size_t> GetEntities();
  size_t AddEntity();
  // void removeEntity(size_t id);
  // void updateWaitingRooms();
  //
  void IntialiseEntities(std::string scene_name);

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
    auto &components = std::get<std::vector<T>>(m_pool->getData());
    return components[entity_id];
  }

  template <typename T> bool hasComponent(size_t entityID) {
    auto &components = std::get<std::vector<T>>(m_pool->getData());

    return components[entityID].getHas();
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // all functions related to archetype management
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // const ArchetypeManager &
  // getArchetypeManager() const; // function for returning archtype manager for
  //                              // testing and if required
  //
  // const Archetype &getExactArchetype(std::vector<std::string> requirements)
  //     const; // pass through for getting exact archetype
  // const std::vector<size_t> &
  // getExactArchetypeEntities(std::vector<std::string> requirements)
  //     const; // pass through for getting exact archetype entities
  //
  // const std::shared_ptr<std::vector<Archetype>>
  // getInclusiveArchetype(std::vector<std::string> requirements)
  //     const; // pass through for getting archetypes of inclusive component
  //     sets
  // const std::shared_ptr<std::vector<size_t>>
  // getInclusiveArchetypeEntities(std::vector<std::string> requirements)
  //     const; // pass through for getting entities for inclusive component
  //     sets
  //
  // extracting data to json

  json toJSON(); // extract data to a json object
};
