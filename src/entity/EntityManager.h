/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the EntityManager class
/////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "ArchetypeManager.h"
#include "FlatbuffersConfigurator.h"
#include "PathProvider.h"
#include "containers.h"
#include <cstddef>
#include <variant>

namespace steamrot {

enum class DataType {
  Flatbuffers,

};
/////////////////////////////////////////////////
/// @class EntityManager
/// @brief The EntityManager class is responsiblle for managing entity lifetimes
/// and the access to the memory pool.
///
/////////////////////////////////////////////////
class EntityManager {

private:
  /////////////////////////////////////////////////
  /// @brief Member variable to hold all the entities/components for a scene.
  /////////////////////////////////////////////////
  EntityMemoryPool m_entity_memory_pool;

  /////////////////////////////////////////////////
  /// @brief Holds the archetype manager instance for this EntityManager and
  /// this Scene
  /////////////////////////////////////////////////
  ArchetypeManager m_archetype_manager;
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

  /////////////////////////////////////////////////
  /// @brief Function to resize the entity memory pool.
  ///
  /// @param new_size New size for the memory pool. (essentially the number of
  /// entities);
  /////////////////////////////////////////////////
  void ResizeEntityMemoryPool(const size_t pool_size);

public:
  ////////////////////////////////////////////////////////////
  /// \brief Default constructor
  ///
  ////////////////////////////////////////////////////////////
  EntityManager(const size_t &pool_size = 100);

  /////////////////////////////////////////////////
  /// @brief Call correct configurator to configure entities from default data
  ///
  /// @param scene_type SceneType to configure entities for
  /// @param data_type DataType to configure entities from
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureEntitiesFromDefaultData(const SceneType scene_type,
                                   const EnvironmentType env_type,
                                   const DataType data_type);

  /////////////////////////////////////////////////
  /// @brief Get a read/write reference to the entity memory pool
  /////////////////////////////////////////////////
  EntityMemoryPool &GetEntityMemoryPool();

  /////////////////////////////////////////////////
  /// @brief Get a reference to the archetype manager
  ///
  /////////////////////////////////////////////////
  const ArchetypeManager &GetArchetypeManager();

  ////////////////////////////////////////////////////////////
  /// \brief return index of next "free" entity
  ///
  ////////////////////////////////////////////////////////////
  size_t GetNextFreeEntityIndex();
};
} // namespace steamrot
