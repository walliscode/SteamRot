#pragma once
#include "ArchetypeManager.h"
#include "EntityMemoryPool.h"
#include "nlohmann/json.hpp"
#include <vector>


using json = nlohmann::json;

class Scene; // forward declaration of scene class to avoid circular dependency

class EntityManager {

private:
  std::vector<size_t> m_entities;      // all active entities
  std::vector<size_t> m_entitiesToAdd; // list of entities to add next update
  std::vector<size_t>
      m_entitiesToRemove; // list of entities to remove next update
  std::shared_ptr<EntityMemoryPool> m_pool; // pool of all entity data
  Scene &m_scene;                           // scene the manager is attached to

  ArchetypeManager
      m_archetypeManager; // archetype manager to handle entity set searching

public:
  EntityManager(size_t poolSize,
                Scene &scene); // constructor to create a new manager with a
                               // size of pool to manage

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // all functions related to entity management (not getting component data)
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  std::vector<size_t>
  getEntities(); // return the index list of current active entities
  size_t
  addEntity(); // add a new entity (assign new values into pool at next index)
  void removeEntity(size_t id); // remove the entity with the supplied id
  void updateWaitingRooms(); // update the entity list with the entities to add
                             // and remove

  void intialiseEntities(
      std::string sceneName); // initialise the entities for the scene

  template <typename T>
  void resetVal(T &componentVector,
                const size_t index) // for a passed vector and index, reset the
                                    // component at the index to a default value
  {
    using vecType = typename T::value_type; // Get the variable type to populate
                                            // the vector with
    componentVector[index] =
        vecType(); // Set the index to a default constructed value
    std::cout << "-> Index of type: '" << typeid(vecType).name() << "' reset\n";
  };

  template <typename TupleT,
            std::size_t... Indices> // unfold function to call reset value for
                                    // each tuple element at a given index
                                    void resetTupleElements(
                                        TupleT &tp,
                                        std::index_sequence<Indices...>,
                                        const size_t index) {
    std::cout << "\nIndexed tuple split...\n";
    (resetVal(std::get<Indices>(tp), index), ...);
  }

  template <typename TupleT, std::size_t TupSize = std::tuple_size_v<TupleT>>
  void refreshEntity(TupleT &tp, const size_t index) {
    resetTupleElements(tp, std::make_index_sequence<TupSize>{},
                       index); // call the reset tuple elements function with
                               // the size of the tuple being passed
    std::cout << "Tuple index reset complete\n\n";
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // all functions related to memory pool management
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  EntityMemoryPool &getPool(); // return the memory pool for the manager

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // all functions related to getting component data
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  template <typename T> T &getComponent(size_t entityID) {
    auto &components = std::get<std::vector<T>>(*(*m_pool).getData());
    return components[entityID]; // return the component of the passed component
                                 // type for the requested entityID
  }

  template <typename T> bool hasComponent(size_t entityID) {
    auto &components = std::get<std::vector<T>>(*(*m_pool).getData());
    return components[entityID]
        .getHas(); // return the component of the passed component type for the
                   // requested entityID
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // all functions related to archetype management
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  const ArchetypeManager &
  getArchetypeManager() const; // function for returning archtype manager for
                               // testing and if required

  const Archetype &getExactArchetype(std::vector<std::string> requirements)
      const; // pass through for getting exact archetype
  const std::vector<size_t> &
  getExactArchetypeEntities(std::vector<std::string> requirements)
      const; // pass through for getting exact archetype entities

  const std::shared_ptr<std::vector<Archetype>>
  getInclusiveArchetype(std::vector<std::string> requirements)
      const; // pass through for getting archetypes of inclusive component sets
  const std::shared_ptr<std::vector<size_t>>
  getInclusiveArchetypeEntities(std::vector<std::string> requirements)
      const; // pass through for getting entities for inclusive component sets

  // extracting data to json

  json toJSON(); // extract data to a json object
};
