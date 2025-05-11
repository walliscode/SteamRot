#pragma once

#include "ArchetypeManager.h"
#include "TypeDefs.h"
#include "containers.h"
#include <bitset>
#include <memory>

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

  ////////////////////////////////////////////////////////////
  /// \brief returns the Archetype for the derived logic class
  ///
  ////////////////////////////////////////////////////////////
  virtual const steamrot::components::containers::ComponentRegister &
  GetArchetype() const = 0;

protected:
  ////////////////////////////////////////////////////////////
  /// \brief Members
  ///
  ////////////////////////////////////////////////////////////
  size_t m_update_frequency;
  size_t m_cycle_count{0};
  std::vector<ArchetypeID> m_archtype_IDs;

  ////////////////////////////////////////////////////////////
  /// \brief Carries out Logic for the game
  ///
  ////////////////////////////////////////////////////////////
  virtual void ProcessLogic(
      std::unique_ptr<steamrot::components::containers::EntityMemoryPool>
          &entities,
      const EntityIndicies &entity_indicies) = 0;

  ////////////////////////////////////////////////////////////
  /// \brief template function to generate bitset from provided types
  ///
  ////////////////////////////////////////////////////////////
  template <typename Component> void SetComponentBit(ArchetypeID &id) {
    // set the bit for the provided Component against the ComponentRegister
    id.set(std::get<Component>(
        steamrot::components::containers::ComponentRegister(), true));
  };
};
