#include "Logic.h"

////////////////////////////////////////////////////////////
Logic::Logic(size_t update_frequency) : m_update_frequency(update_frequency) {}

////////////////////////////////////////////////////////////
void Logic::RunLogic(size_t cycle, std::unique_ptr<EntityMemoryPool> &entities,
                     const EntityIndicies &entity_indicies) {
  if (cycle % m_update_frequency == 0) {
    ProcessLogic(entities, entity_indicies);
  }
}
