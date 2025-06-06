#include "EntityHelpers.h"

namespace steamrot {

size_t GetMemoryPoolSize(
    const components::containers::EntityMemoryPool &entity_memory_pool) {
  // Get the size of the first component vector in the tuple
  return std::get<0>(entity_memory_pool).size();
}
} // namespace steamrot
