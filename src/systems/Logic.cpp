#include "Logic.h"

////////////////////////////////////////////////////////////
Logic::Logic(size_t update_frequency) : m_update_frequency(update_frequency) {}

////////////////////////////////////////////////////////////
void Logic::RunLogic(std::unique_ptr<EntityMemoryPool> &entities,
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
