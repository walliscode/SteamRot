#include "EntityMemoryPool.h"
#include "containers.h"
#include <memory>
#include <vector>

EntityMemoryPool::EntityMemoryPool(const size_t &pool_size)
    : m_num_entities(pool_size),
      m_data(std::make_shared<
             steamrot::components::containers::ComponentCollection>()) {
  DefineFreshTuple(*m_data, pool_size);
}

const size_t EntityMemoryPool::getNextEntityIndex() {

  // 0 will be used as no free positions (possible collisions with index 0?)
  size_t index = 0;
  // the CMeta component contains meta data about the entity
  // in this scenario, is the entity active or not?
  // if not then we can use it as another entity
  std::vector<CMeta> cmeta_vector = std::get<std::vector<CMeta>>(*m_data);

  // return the index for the next free entity index using CMeta active member
  for (int i = 0; i != cmeta_vector.size(); ++i) {

    if (!cmeta_vector[i].m_entity_active) {
      index = i;
      break;
    }
  }
  return index;
}

std::shared_ptr<steamrot::components::containers::ComponentCollection>
EntityMemoryPool::getData() {
  return m_data;
}
