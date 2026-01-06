/////////////////////////////////////////////////
/// @file
/// @brief Declaration of FlatbuffersEntityImporter class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventHandler.h"
#include "IEntityImporter.h"
#include "entities_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersEntityImporter
/// @brief Imports entities from FlatBuffers EntityCollectionFbs data.
///
/// This class wraps the EntityCollectionFbs reference and provides
/// the IEntityImporter interface. Internally, it uses
/// FlatbuffersEntityConfigurator to perform the actual import.
/////////////////////////////////////////////////
class FlatbuffersEntityImporter : public IEntityImporter {
private:
  /////////////////////////////////////////////////
  /// @brief Reference to EventHandler for creating subscribers
  /////////////////////////////////////////////////
  EventHandler &m_event_handler;

  /////////////////////////////////////////////////
  /// @brief Reference to the EntityCollectionFbs data
  /////////////////////////////////////////////////
  const EntityCollectionFbs &m_entity_collection;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param event_handler Reference to event handler for UI events
  /// @param entity_collection FlatBuffers entity data
  /////////////////////////////////////////////////
  FlatbuffersEntityImporter(EventHandler &event_handler,
                            const EntityCollectionFbs &entity_collection);

  /////////////////////////////////////////////////
  /// @brief Import entities from FlatBuffers into EntityMemoryPool
  ///
  /// Creates a FlatbuffersEntityConfigurator and delegates to it.
  ///
  /// @param emp EntityMemoryPool to populate
  /// @return std::monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ImportEntities(EntityMemoryPool &emp) override;
};

} // namespace steamrot
