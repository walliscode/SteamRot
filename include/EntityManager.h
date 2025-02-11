#pragma once
// #include "ArchetypeManager.h"
#include "EntityMemoryPool.h"
#include <cstddef>

#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

class EntityManager {

private:
  std::vector<size_t> m_entities;
  std::vector<size_t> m_entitiesToAdd; // list of entities to add next update
  std::vector<size_t>
      m_entitiesToRemove; // list of entities to remove next update
  std::shared_ptr<EntityMemoryPool> m_pool; // pool of all entity
  // data

  // ArchetypeManager m_archetypeManager;

public:
  EntityManager(const size_t &poolSize);
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // all functions related to entity management (not getting component data)
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // std::vector<size_t> getEntities();
  // size_t addEntity();
  // void removeEntity(size_t id);
  // void updateWaitingRooms();
  //
  // void intialiseEntities(std::string sceneName);
  //
  // template <typename T> void resetVal(T &componentVector, const size_t index)
  // {
  //
  //   // Get the type of the vector to populate the vector with
  //   using vecType = typename T::value_type;
  //
  //   // Set the index to a default constructed value
  //   componentVector[index] = vecType();
  //   std::cout << "-> Index of type: '" << typeid(vecType).name() << "'
  //   reset\n";
  // };
  //
  // template <typename TupleT, std::size_t... Indices>
  // void resetTupleElements(TupleT &tp, std::index_sequence<Indices...>,
  //                         const size_t index) {
  //
  //   std::cout << "\nIndexed tuple split...\n";
  //   (resetVal(std::get<Indices>(tp), index), ...);
  // }
  //
  // template <typename TupleT, std::size_t TupSize = std::tuple_size_v<TupleT>>
  // void refreshEntity(TupleT &tp, const size_t index) {
  //
  //   resetTupleElements(tp, std::make_index_sequence<TupSize>{}, index);
  //   std::cout << "Tuple index reset complete\n\n";
  // }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // all functions related to getting component data
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // template <typename T> T &getComponent(size_t entityID) {
  //   auto &components = std::get<std::vector<T>>(*(*m_pool).getData());
  //   return components[entityID]; // return the component of the passed
  //   component
  //                                // type for the requested entityID
  // }
  //
  // template <typename T> bool hasComponent(size_t entityID) {
  //   auto &components = std::get<std::vector<T>>(*(*m_pool).getData());
  //   return components[entityID]
  //       .getHas(); // return the component of the passed component type for
  //       the
  //                  // requested entityID
  // }

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
