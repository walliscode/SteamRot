#pragma once

#include "ArchetypeManager.h"
#include "containers.h"
#include <bitset>
#include <memory>
namespace steamrot {
using EntityIndicies = std::vector<size_t>;

template <typename... AllComponentTypes> class Logic {
public:
  ////////////////////////////////////////////////////////////
  /// \brief default constructor
  ///
  ////////////////////////////////////////////////////////////

  Logic<AllComponentTypes...>() {

    // create the archetype ID for this logic class
    ArchetypeIDFactory<AllComponentTypes...>();
  };

  ////////////////////////////////////////////////////////////
  /// \brief guard function for update frequency
  ///
  ////////////////////////////////////////////////////////////
  void
  RunLogic(std::unique_ptr<steamrot::components::containers::EntityMemoryPool>
               &entities,
           const EntityIndicies &entity_indicies);

protected:
  ////////////////////////////////////////////////////////////
  /// \brief Members
  ///
  ////////////////////////////////////////////////////////////
  size_t m_update_frequency{2};
  size_t m_cycle_count{0};
  std::vector<ArchetypeID> m_archetype_IDs;

  ////////////////////////////////////////////////////////////
  /// \brief Carries out Logic for the game
  ///
  ////////////////////////////////////////////////////////////
  virtual void ProcessLogic(components::containers::EntityMemoryPool &entities,
                            const EntityIndicies &entity_indicies) = 0;

  ////////////////////////////////////////////////////////////
  /// \brief template factory function for ArcetypeId creation. Contains logic
  /// for passing combinations of components
  ///
  ////////////////////////////////////////////////////////////
  template <typename... Components> void ArchetypeIDFactory() {

    // for now just generate a single archetype ID from the component types
    ArchetypeID id = GenerateArchetypeID<Components...>();
    m_archetype_IDs.push_back(id);

    // eventually add Logic for all component combinations
  }
  ////////////////////////////////////////////////////////////
  /// \brief template to generate an archetype ID from the component types
  ///
  ////////////////////////////////////////////////////////////
  template <typename... Component> ArchetypeID GenerateArchetypeID() {

    // create an empty archetype ID
    ArchetypeID id = 0;
    // for each Component Type check Type Index from ComponenRegister and set
    // the bit
    (...,
     id.set(steamrot::components::containers::TupleTypeIndex<
            Component, steamrot::components::containers::ComponentRegister>));

    return id;
  }
};
template <typename... AllComponentTypes>
void Logic<AllComponentTypes...>::RunLogic(
    std::unique_ptr<steamrot::components::containers::EntityMemoryPool>
        &entities,
    const EntityIndicies &entity_indicies) {

  if (m_cycle_count == m_update_frequency) {
    ProcessLogic(entities, entity_indicies);

    // reset the cycle count to 1, make sure this comes at the end of the if
    // block
    m_cycle_count = 1;
  } else {
    // increment the cycle count only if we are not at the update frequency
    m_cycle_count++;
  }
}
}; // namespace steamrot
