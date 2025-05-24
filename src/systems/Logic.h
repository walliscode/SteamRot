#pragma once

#include "ArchetypeManager.h"

#include "containers.h"
#include <bitset>
#include <memory>

using EntityIndicies = std::vector<size_t>;

template <typename... AllComponentTypes> class Logic {
public:
  ////////////////////////////////////////////////////////////
  /// \brief default constructor
  ///
  ////////////////////////////////////////////////////////////
  Logic(size_t update_frequency);

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
  size_t m_update_frequency;
  size_t m_cycle_count{0};
  std::vector<ArchetypeID> m_archetype_IDs;

  ////////////////////////////////////////////////////////////
  /// \brief Carries out Logic for the game
  ///
  ////////////////////////////////////////////////////////////
  virtual void ProcessLogic(
      std::unique_ptr<steamrot::components::containers::EntityMemoryPool>
          &entities,
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
