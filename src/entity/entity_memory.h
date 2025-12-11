/////////////////////////////////////////////////
/// @file
/// @brief Declaration of functions that deal with the EntityMemoryPool
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "containers.h"

namespace steamrot::entity::memory {

/////////////////////////////////////////////////
/// @brief template to get vector of given component type
///
/////////////////////////////////////////////////
template <typename T>
std::vector<T> &GetComponentVector(EntityMemoryPool &entity_memory_pool) {

  auto &component_vector = std::get<std::vector<T>>(entity_memory_pool);
  return component_vector;
}

template <typename T>
const std::vector<T> &
GetComponentVector(const EntityMemoryPool &entity_memory_pool) {

  const auto &component_vector = std::get<std::vector<T>>(entity_memory_pool);
  return component_vector;
}

/////////////////////////////////////////////////
/// @brief get the a component at a given index
///
/////////////////////////////////////////////////
template <typename T>
T &GetComponent(size_t entity_id, EntityMemoryPool &entity_memory_pool) {

  return GetComponentVector<T>(entity_memory_pool)[entity_id];
}

/////////////////////////////////////////////////
/// @brief Get a const component at a given index
///
/// @tparam T Component type
/// @param entity_id Index of the entity
/// @param entity_memory_pool EntityMemoryPool instance
/// @return Reference to the component of type T at the given index
/////////////////////////////////////////////////
template <typename T>
const T &GetComponent(size_t entity_id,
                      const EntityMemoryPool &entity_memory_pool) {
  return GetComponentVector<T>(entity_memory_pool)[entity_id];
}

/////////////////////////////////////////////////
/// @brief Resize the entity memory pool
///
/// @param entity_memory_pool EntityMemoryPool instance to resize
/// @param new_size New size for the memory pool
/////////////////////////////////////////////////
void ResizeEntityMemoryPool(EntityMemoryPool &entity_memory_pool,
                            const size_t new_size);
/////////////////////////////////////////////////
/// @brief Getter for the component vector of a given type
///
/// @param entity_memory_pool Instance of EntityMemoryPool
/// @return Size of the EntityMemoryPool
/////////////////////////////////////////////////
size_t GetMemoryPoolSize(const EntityMemoryPool &entity_memory_pool);
} // namespace steamrot::entity::memory
