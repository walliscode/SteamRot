#pragma once
#include <algorithm>
#include <cstddef>
#include <memory>
#include <tuple>

// global definition for the ComponentCollectionTuple in the header file
// do not add extra componets anywhere else
#include "ComponentCollections.h"

class EntityMemoryPool {

private:
  // the number of "entities" is the length of the component vectors
  // these NEED to be the same for each vector
  size_t m_num_entities;

  // this is where all the component data is stored
  std::shared_ptr<ComponentCollection> m_data;

public:
  EntityMemoryPool(
      const size_t &pool_size); // constructor for setting pool size

  // Entity pool functions
  // return the next "free" entity index. this will still have Components but
  // they will all be reset
  const size_t getNextEntityIndex();

  std::shared_ptr<ComponentCollection> getData();
  // Entity pool templates

  // This is designed to work with the ComponentCollectionTuple
  // it takes in each std::vector<T> and resizes it to the size passed in
  template <typename T>
  void DefineComponentVectorSize(T &component_vector,
                                 const size_t &vector_size) {

    component_vector.resize(vector_size);

    // fill the vector with default constructed values for the components
    std::fill(component_vector.begin(), component_vector.end(),
              typename T::value_type());
  }

  // take in a tuple (again, designed to work with the ComponentCollectionTuple)
  // create an index sequence from the size of the tuple (essentially a for
  // loop) vector size is also passed in to pass to define each
  // std::vector<Component>
  template <typename TupleT, std::size_t... tuple_index_sequence>
  void DefineTupleElements(TupleT &tuple,
                           std::index_sequence<tuple_index_sequence...>,
                           size_t vector_size)

  {
    (DefineComponentVectorSize(std::get<tuple_index_sequence>(tuple),
                               vector_size),
     ...);
  }

  // this is essentially the init function for the memory pool
  // other setup functions can be added here
  template <typename TupleT, std::size_t TupleSize = std::tuple_size_v<TupleT>>
  void DefineFreshTuple(TupleT &tuple, const size_t vector_size) {

    DefineTupleElements(tuple, std::make_index_sequence<TupleSize>{},
                        vector_size);
  }
};
