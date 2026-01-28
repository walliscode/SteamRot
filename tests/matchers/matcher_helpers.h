/////////////////////////////////////////////////
/// @file
/// @brief General matcher helper functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "CMeta.h"
#include "CUserInterface.h"
#include "containers.h"
#include "entity_memory.h"
#include <ostream>
#include <vector>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Helper to create a simple EntityMemoryPool for testing
///
/// @param size The size of the pool to create
/// @return EntityMemoryPool with resized component vectors
/////////////////////////////////////////////////
inline EntityMemoryPool CreateTestPool(size_t size) {
  EntityMemoryPool pool;
  auto &cmeta_vec = entity::memory::GetComponentVector<CMeta>(pool);
  cmeta_vec.resize(size);
  auto &cui_vec = entity::memory::GetComponentVector<CUserInterface>(pool);
  cui_vec.resize(size);
  return pool;
}

/////////////////////////////////////////////////
/// @brief Helper to print std::vector to output stream
///
/// @tparam T [TODO:tparam]
/// @param os [TODO:parameter]
/// @param vec [TODO:parameter]
/// @return [TODO:return]
/////////////////////////////////////////////////
template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
  os << "[";
  for (size_t i = 0; i < vec.size(); ++i) {
    os << vec[i];
    if (i + 1 < vec.size())
      os << ", ";
  }
  os << "]";
  return os;
}
} // namespace steamrot::tests
