/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the TestDataSource class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestDataSource.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
TestDataSource::TestDataSource(const TestDataConfig *config)
    : m_config(config) {}

/////////////////////////////////////////////////
const EntityCollection *TestDataSource::GetEntityCollection() const {
  if (!m_config) {
    return nullptr;
  }

  if (!m_config->start_data_collection()) {
    return nullptr;
  }

  return m_config->start_data_collection()->entity_collection();
}

/////////////////////////////////////////////////
size_t TestDataSource::GetEntityCount() const {
  const EntityCollection *collection = GetEntityCollection();

  if (collection && collection->entities()) {
    return collection->entities()->size();
  }

  return 0;
}

/////////////////////////////////////////////////
std::string TestDataSource::GetSourceIdentifier() const {
  std::string identifier = "TestDataSource";

  if (m_config && m_config->metadata() && m_config->metadata()->test_name()) {
    identifier += "(";
    identifier += m_config->metadata()->test_name()->str();
    identifier += ")";
  }

  return identifier;
}

/////////////////////////////////////////////////
const TestDataConfig *TestDataSource::GetTestConfig() const { return m_config; }

} // namespace steamrot::tests
