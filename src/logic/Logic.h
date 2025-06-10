#pragma once

#include "ArchetypeManager.h"
#include "BaseLogic.h"
#include "containers.h"
#include <bitset>
namespace steamrot {

template <typename... AllComponentTypes> class Logic : BaseLogic {
public:
  ////////////////////////////////////////////////////////////
  /// \brief default constructor
  ///
  ////////////////////////////////////////////////////////////

  Logic<AllComponentTypes...>() {

    // create the archetype ID for this logic class
    ArchetypeIDFactory<AllComponentTypes...>();
  };

protected:
  virtual void ProcessLogic(components::containers::EntityMemoryPool &entities,
                            const EntityIndicies &entity_indicies) = 0;

  std::vector<ArchetypeID> m_archetype_IDs;

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

}; // namespace steamrot
