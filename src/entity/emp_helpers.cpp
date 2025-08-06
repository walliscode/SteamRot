/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "emp_helpers.h"

namespace steamrot::emp_helpers {
/////////////////////////////////////////////////

void ResizeEntityMemoryPool(EntityMemoryPool &entity_memory_pool,
                            const size_t new_size) {
  // Use std::apply to resize the memory pool with a lambda function
  std::apply(
      [new_size](auto &...component_vector) {
        (component_vector.resize(new_size), ...);
      },
      entity_memory_pool);
}
/////////////////////////////////////////////////
size_t GetMemoryPoolSize(const EntityMemoryPool &entity_memory_pool) {
  // Get the size of the first component vector in the tuple
  return std::get<0>(entity_memory_pool).size();
}
} // namespace steamrot::emp_helpers
