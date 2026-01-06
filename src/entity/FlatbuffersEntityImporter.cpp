/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersEntityImporter class
/////////////////////////////////////////////////

#include "FlatbuffersEntityImporter.h"
#include "FlatbuffersEntityConfigurator.h"

namespace steamrot {

/////////////////////////////////////////////////
FlatbuffersEntityImporter::FlatbuffersEntityImporter(
    EventHandler &event_handler, const EntityCollectionFbs &entity_collection)
    : m_event_handler(event_handler), m_entity_collection(entity_collection) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersEntityImporter::ImportEntities(EntityMemoryPool &emp) {

  // Create configurator with the wrapped data
  FlatbuffersEntityConfigurator configurator(m_event_handler,
                                             m_entity_collection);

  // Delegate to configurator to do the actual work
  return configurator.ConfigureEntityMemoryPool(emp);
}

} // namespace steamrot
