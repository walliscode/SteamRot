/////////////////////////////////////////////////
/// @file
/// @brief Implementation of TestScenarios class
/////////////////////////////////////////////////

#include "TestScenarios.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
EntityMemoryPool TestScenarios::CreateEmptyPool() {
  return EntityMemoryPool();
}

/////////////////////////////////////////////////
EntityMemoryPool TestScenarios::CreatePoolWithNEntities(size_t n) {
  EntityMemoryPool pool;
  
  // Add entities by pushing default-constructed components to each vector
  std::apply([n](auto&... vectors) {
    ((vectors.resize(n)), ...);
  }, pool);
  
  return pool;
}

/////////////////////////////////////////////////
EntityMemoryPool TestScenarios::CreatePoolWithArchetype(ArchetypeID archetype_id,
                                                        size_t n) {
  EntityMemoryPool pool = CreatePoolWithNEntities(n);
  
  for (size_t i = 0; i < n; ++i) {
    // Activate components based on the archetype ID bitset
    // Each bit in archetype_id corresponds to a component index
    for (size_t bit_index = 0; bit_index < archetype_id.size(); ++bit_index) {
      if (archetype_id[bit_index]) {
        // Component is part of the archetype, activate it
        ActivateComponentByIndex(i, bit_index, pool);
      }
    }
  }
  
  return pool;
}

/////////////////////////////////////////////////
EntityMemoryPool TestScenarios::CreatePoolWithSingleUIEntity() {
  EntityMemoryPool pool = CreatePoolWithNEntities(1);
  ActivateComponent<CUserInterface>(0, pool);
  return pool;
}

/////////////////////////////////////////////////
EntityMemoryPool TestScenarios::CreatePoolWithMultipleUIEntities(size_t n) {
  EntityMemoryPool pool = CreatePoolWithNEntities(n);
  
  for (size_t i = 0; i < n; ++i) {
    ActivateComponent<CUserInterface>(i, pool);
  }
  
  return pool;
}

/////////////////////////////////////////////////
EntityMemoryPool TestScenarios::CreatePoolWithSingleGrimoireEntity() {
  EntityMemoryPool pool = CreatePoolWithNEntities(1);
  ActivateComponent<CGrimoireMachina>(0, pool);
  return pool;
}

/////////////////////////////////////////////////
ArchetypeManager TestScenarios::CreatePopulatedArchetypeManager(
    const EntityMemoryPool &pool) {
  ArchetypeManager manager(pool);
  manager.GenerateAllArchetypes();
  return manager;
}

/////////////////////////////////////////////////
// Helper to activate component by tuple index
/////////////////////////////////////////////////
void TestScenarios::ActivateComponentByIndex(size_t entity_id,
                                              size_t component_index,
                                              EntityMemoryPool &pool) {
  // This helper activates a component based on its position in ComponentRegister
  // We use a switch statement to handle each component type
  
  switch (component_index) {
    case 0:
      ActivateComponent<std::tuple_element_t<0, ComponentRegister>>(entity_id, pool);
      break;
    case 1:
      ActivateComponent<std::tuple_element_t<1, ComponentRegister>>(entity_id, pool);
      break;
    case 2:
      ActivateComponent<std::tuple_element_t<2, ComponentRegister>>(entity_id, pool);
      break;
    case 3:
      ActivateComponent<std::tuple_element_t<3, ComponentRegister>>(entity_id, pool);
      break;
    case 4:
      ActivateComponent<std::tuple_element_t<4, ComponentRegister>>(entity_id, pool);
      break;
    // Add more cases as components are added to ComponentRegister
    default:
      // Component index out of range, do nothing
      break;
  }
}

} // namespace steamrot::tests
