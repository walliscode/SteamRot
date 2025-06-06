#pragma once

#include "containers.h"

namespace steamrot {
////////////////////////////////////////////////////////////
/// |brief reset a component at a given index to default values
///
////////////////////////////////////////////////////////////
template <typename T>
void ResetValues(T &component_vector, const size_t index) {

  // Get the type of the Component the vector holds
  using ComponentType = typename T::value_type;

  // Set the index to a default constructed Component type
  component_vector[index] = ComponentType();
};

////////////////////////////////////////////////////////////
/// |brief reset all components in a tuple at a given index
///
////////////////////////////////////////////////////////////
template <typename TupleT, std::size_t... tuple_index_sequence>
void ResetTupleElements(TupleT &component_tuple,
                        std::index_sequence<tuple_index_sequence...>,
                        const size_t index) {

  (ResetValues(std::get<tuple_index_sequence>(component_tuple), index), ...);
}

////////////////////////////////////////////////////////////
/// |brief refresh all components in a tuple at a given index
///
////////////////////////////////////////////////////////////
template <typename TupleT, std::size_t TupleSize = std::tuple_size_v<TupleT>>
void RefreshEntity(TupleT &component_tuple, const size_t index) {

  ResetTupleElements(component_tuple, std::make_index_sequence<TupleSize>{},
                     index);
}
////////////////////////////////////////////////////////////
/// |brief template to get vector of given component type
///
////////////////////////////////////////////////////////////
template <typename T>
std::vector<T> &GetComponentVector(
    components::containers::EntityMemoryPool &entity_memory_pool) {

  auto &component_vector = std::get<std::vector<T>>(entity_memory_pool);

  return component_vector;
}
////////////////////////////////////////////////////////////
/// |brief get the a component at a given index
///
////////////////////////////////////////////////////////////
template <typename T>
T &GetComponent(size_t entity_id,
                components::containers::EntityMemoryPool &entity_memory_pool) {

  return GetComponentVector<T>(entity_memory_pool)[entity_id];
}

////////////////////////////////////////////////////////////
/// |brief check if a component is active for a given entity
///
////////////////////////////////////////////////////////////
template <typename T> bool HasComponent(size_t entityID) {
  return GetComponent<T>(entityID).getHas();
}
} // namespace steamrot
