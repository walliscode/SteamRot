/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "containers.h"

namespace steamrot::emp_helpers {

////////////////////////////////////////////////////////////
/// |brief template to get vector of given component type
///
////////////////////////////////////////////////////////////
template <typename T>
std::vector<T> &GetComponentVector(EntityMemoryPool &entity_memory_pool) {

  auto &component_vector = std::get<std::vector<T>>(entity_memory_pool);

  return component_vector;
}

template <typename T>
std::vector<T> &GetComponentVector(const EntityMemoryPool &entity_memory_pool) {
  auto &component_vector = std::get<std::vector<T>>(entity_memory_pool);
  return component_vector;
}

////////////////////////////////////////////////////////////
/// |brief get the a component at a given index
///
////////////////////////////////////////////////////////////
template <typename T>
T &GetComponent(size_t entity_id, EntityMemoryPool &entity_memory_pool) {

  return GetComponentVector<T>(entity_memory_pool)[entity_id];
}

template <typename T>
const T &GetComponent(size_t entity_id,
                      const EntityMemoryPool &entity_memory_pool) {
  return GetComponentVector<T>(entity_memory_pool)[entity_id];
}

/////////////////////////////////////////////////
/// @brief Getter for the component vector of a given type
///
/// @param entity_memory_pool Instance of EntityMemoryPool
/// @return Size of the EntityMemoryPool
/////////////////////////////////////////////////
size_t GetMemoryPoolSize(const EntityMemoryPool &entity_memory_pool);
} // namespace steamrot::emp_helpers
