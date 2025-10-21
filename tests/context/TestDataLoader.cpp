/////////////////////////////////////////////////
/// @file
/// @brief Implementation of TestDataLoader class
/////////////////////////////////////////////////

#include "TestDataLoader.h"
#include "FlatbuffersConfigurator.h"
#include <fstream>
#include <sstream>

namespace steamrot::tests {

/////////////////////////////////////////////////
TestDataLoader::TestDataLoader(PathProvider &path_provider)
    : m_buffer(nullptr), m_test_data(nullptr),
      m_path_provider(path_provider) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
TestDataLoader::LoadFromBinary(const std::string &filename) {
  std::string filepath = m_path_provider.GetTestDataDir() + "/" + filename;

  std::ifstream file(filepath, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    return std::unexpected(
        FailInfo{FailMode::FileNotFound, "Cannot open test data file: " + filepath});
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  m_buffer = std::make_unique<uint8_t[]>(size);
  if (!file.read(reinterpret_cast<char *>(m_buffer.get()), size)) {
    return std::unexpected(
        FailInfo{FailMode::FileReadError, "Cannot read test data file: " + filepath});
  }

  m_test_data = GetTestDataFile(m_buffer.get());
  if (!m_test_data) {
    return std::unexpected(
        FailInfo{FailMode::InvalidData, "Invalid test data format in file: " + filename});
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
const TestCase *TestDataLoader::GetTestCase() const {
  if (!m_test_data) {
    return nullptr;
  }
  return m_test_data->test_case();
}

/////////////////////////////////////////////////
const TestMetadata *TestDataLoader::GetMetadata() const {
  const TestCase *test_case = GetTestCase();
  if (!test_case) {
    return nullptr;
  }
  return test_case->metadata();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
TestDataLoader::ConfigurePoolFromBeforeState(EntityMemoryPool &pool) const {
  const TestCase *test_case = GetTestCase();
  if (!test_case) {
    return std::unexpected(
        FailInfo{FailMode::InvalidData, "No test case loaded"});
  }

  const TestStateSnapshot *before_state = test_case->before_state();
  if (!before_state) {
    return std::unexpected(
        FailInfo{FailMode::InvalidData, "No before state in test case"});
  }

  return ConfigurePoolFromSnapshot(pool, before_state);
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
TestDataLoader::ConfigurePoolFromAfterState(EntityMemoryPool &pool) const {
  const TestCase *test_case = GetTestCase();
  if (!test_case) {
    return std::unexpected(
        FailInfo{FailMode::InvalidData, "No test case loaded"});
  }

  const TestStateSnapshot *after_state = test_case->after_state();
  if (!after_state) {
    return std::unexpected(
        FailInfo{FailMode::InvalidData, "No after state in test case"});
  }

  return ConfigurePoolFromSnapshot(pool, after_state);
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
TestDataLoader::ConfigurePoolFromIntermediateState(EntityMemoryPool &pool,
                                                    size_t state_index) const {
  const TestCase *test_case = GetTestCase();
  if (!test_case) {
    return std::unexpected(
        FailInfo{FailMode::InvalidData, "No test case loaded"});
  }

  const auto *intermediate_states = test_case->intermediate_states();
  if (!intermediate_states || state_index >= intermediate_states->size()) {
    std::ostringstream oss;
    oss << "Invalid intermediate state index: " << state_index;
    return std::unexpected(FailInfo{FailMode::InvalidData, oss.str()});
  }

  const TestStateSnapshot *state = intermediate_states->Get(state_index);
  if (!state) {
    return std::unexpected(
        FailInfo{FailMode::InvalidData, "Intermediate state is null"});
  }

  return ConfigurePoolFromSnapshot(pool, state);
}

/////////////////////////////////////////////////
size_t TestDataLoader::GetIntermediateStateCount() const {
  const TestCase *test_case = GetTestCase();
  if (!test_case) {
    return 0;
  }

  const auto *intermediate_states = test_case->intermediate_states();
  if (!intermediate_states) {
    return 0;
  }

  return intermediate_states->size();
}

/////////////////////////////////////////////////
bool TestDataLoader::IsLoaded() const { return m_test_data != nullptr; }

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
TestDataLoader::ConfigurePoolFromSnapshot(
    EntityMemoryPool &pool, const TestStateSnapshot *snapshot) const {
  if (!snapshot) {
    return std::unexpected(
        FailInfo{FailMode::InvalidData, "Snapshot is null"});
  }

  const TestDataLevel *data_level = snapshot->data();
  if (!data_level) {
    return std::unexpected(
        FailInfo{FailMode::InvalidData, "Data level is null"});
  }

  // Handle different data level types
  switch (data_level->type()) {
  case TestDataLevel_EntityMemoryPoolData: {
    const EntityCollection *entity_collection =
        data_level->AsEntityMemoryPoolData();
    return ConfigurePoolFromEntityCollection(pool, entity_collection);
  }
  case TestDataLevel_ComponentData: {
    const ComponentStateData *component_data =
        data_level->AsComponentData();
    if (!component_data || !component_data->entities()) {
      return std::unexpected(
          FailInfo{FailMode::InvalidData, "Invalid component data"});
    }
    return ConfigurePoolFromEntityCollection(pool, component_data->entities());
  }
  case TestDataLevel_SceneData: {
    // For scene data, we'll use the first scene's entities
    const SceneCollectionData *scene_collection =
        data_level->AsSceneData();
    if (!scene_collection || !scene_collection->scenes() ||
        scene_collection->scenes()->size() == 0) {
      return std::unexpected(
          FailInfo{FailMode::InvalidData, "Invalid scene data"});
    }
    const SceneStateData *scene_state = scene_collection->scenes()->Get(0);
    if (!scene_state || !scene_state->entities()) {
      return std::unexpected(
          FailInfo{FailMode::InvalidData, "Invalid scene state"});
    }
    return ConfigurePoolFromEntityCollection(pool, scene_state->entities());
  }
  default:
    return std::unexpected(
        FailInfo{FailMode::InvalidData, "Unsupported test data level type"});
  }
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
TestDataLoader::ConfigurePoolFromEntityCollection(
    EntityMemoryPool &pool, const EntityCollection *entity_collection) const {
  if (!entity_collection) {
    return std::unexpected(
        FailInfo{FailMode::InvalidData, "Entity collection is null"});
  }

  // Resize pool if entity_memory_pool_size is specified
  if (entity_collection->entity_memory_pool_size() > 0) {
    size_t pool_size = entity_collection->entity_memory_pool_size();
    std::apply(
        [pool_size](auto &...component_vector) {
          (component_vector.resize(pool_size), ...);
        },
        pool);
  }

  // For now, this is a placeholder implementation
  // A full implementation would use FlatbuffersConfigurator
  // to populate the pool from the entity data
  
  // This would require extending FlatbuffersConfigurator to accept
  // an EntityCollection directly, or creating a similar helper function

  return std::monostate{};
}

} // namespace steamrot::tests
