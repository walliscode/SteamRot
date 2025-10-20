/////////////////////////////////////////////////
/// @file
/// @brief Implementation of reusable test assertion functions
/////////////////////////////////////////////////

#include "test_assertions.h"
#include <catch2/catch_test_macros.hpp>
#include <sstream>

namespace steamrot::tests {

/////////////////////////////////////////////////
void AssertComponentActive(const Component &component, bool should_be_active,
                          const std::string &test_name) {
  std::stringstream error_msg;
  error_msg << "Component activation state mismatch";
  if (!test_name.empty()) {
    error_msg << " in test: " << test_name;
  }
  
  REQUIRE(component.m_active == should_be_active);
}

/////////////////////////////////////////////////
void AssertEntityInArchetype(size_t entity_id, ArchetypeID archetype_id,
                             const ArchetypeManager &manager,
                             const std::string &test_name) {
  const auto &archetypes = manager.GetArchetypes();
  auto it = archetypes.find(archetype_id);
  
  std::stringstream error_msg;
  error_msg << "Archetype does not exist";
  if (!test_name.empty()) {
    error_msg << " in test: " << test_name;
  }
  
  REQUIRE(it != archetypes.end());
  
  const Archetype &archetype = it->second;
  bool found = false;
  
  for (size_t id : archetype) {
    if (id == entity_id) {
      found = true;
      break;
    }
  }
  
  std::stringstream entity_error_msg;
  entity_error_msg << "Entity " << entity_id << " not found in archetype";
  if (!test_name.empty()) {
    entity_error_msg << " in test: " << test_name;
  }
  
  REQUIRE(found);
}

/////////////////////////////////////////////////
void AssertArchetypeExists(ArchetypeID archetype_id,
                          const ArchetypeManager &manager,
                          const std::string &test_name) {
  const auto &archetypes = manager.GetArchetypes();
  auto it = archetypes.find(archetype_id);
  
  std::stringstream error_msg;
  error_msg << "Archetype does not exist";
  if (!test_name.empty()) {
    error_msg << " in test: " << test_name;
  }
  
  REQUIRE(it != archetypes.end());
}

/////////////////////////////////////////////////
void AssertArchetypeEntityCount(ArchetypeID archetype_id,
                               size_t expected_count,
                               const ArchetypeManager &manager,
                               const std::string &test_name) {
  AssertArchetypeExists(archetype_id, manager, test_name);
  
  const auto &archetypes = manager.GetArchetypes();
  const Archetype &archetype = archetypes.at(archetype_id);
  
  std::stringstream error_msg;
  error_msg << "Archetype entity count mismatch: expected " << expected_count
            << " but got " << archetype.size();
  if (!test_name.empty()) {
    error_msg << " in test: " << test_name;
  }
  
  REQUIRE(archetype.size() == expected_count);
}

/////////////////////////////////////////////////
void AssertEntityPoolValid(const EntityMemoryPool &pool,
                          const std::string &test_name) {
  // Get the first component vector to determine entity count
  const auto &first_component_vector = std::get<0>(pool);
  size_t entity_count = first_component_vector.size();
  
  // Verify that indices are valid
  std::stringstream error_msg;
  error_msg << "EntityMemoryPool has invalid entity count";
  if (!test_name.empty()) {
    error_msg << " in test: " << test_name;
  }
  
  REQUIRE(entity_count >= 0);
  
  // Additional validation: all component vectors should have same size
  bool all_same_size = true;
  std::apply([entity_count, &all_same_size](const auto&... vectors) {
    ((all_same_size = all_same_size && (vectors.size() == entity_count)), ...);
  }, pool);
  
  REQUIRE(all_same_size);
}

} // namespace steamrot::tests
