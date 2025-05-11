#include "Logic.h"

////////////////////////////////////////////////////////////
template <typename... AllComponentTypes>
Logic<AllComponentTypes...>::Logic(size_t update_frequency)
    : m_update_frequency(update_frequency) {

  // create the archetype ID for this logic class
  ArchetypeIDFactory<AllComponentTypes...>();
};

////////////////////////////////////////////////////////////

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
