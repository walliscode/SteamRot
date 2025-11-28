/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the DefaultSceneDataSource class.
///
/// DefaultSceneDataSource implements IEntityDataSource to load entity data
/// from default scene data files (used by the game).
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include "IEntityDataSource.h"
#include "scene_change_packet_generated.h"
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
/// @class DefaultSceneDataSource
/// @brief Loads entity data from default scene data files.
///
/// This data source is used by GameEngine to load entities from
/// the standard scene data files (data/scene/*.bin).
///
/// Internally uses FlatbuffersDataLoader and PathProvider to locate
/// and load the scene data.
/////////////////////////////////////////////////
class DefaultSceneDataSource : public IEntityDataSource {
private:
  /////////////////////////////////////////////////
  /// @brief The scene type to load data for.
  /////////////////////////////////////////////////
  SceneType m_scene_type;

  /////////////////////////////////////////////////
  /// @brief Data loader for loading FlatBuffers data.
  /////////////////////////////////////////////////
  FlatbuffersDataLoader m_loader;

  /////////////////////////////////////////////////
  /// @brief Cached scene data pointer.
  /////////////////////////////////////////////////
  mutable const SceneData *m_scene_data = nullptr;

  /////////////////////////////////////////////////
  /// @brief Flag indicating if data has been loaded.
  /////////////////////////////////////////////////
  mutable bool m_data_loaded = false;

  /////////////////////////////////////////////////
  /// @brief Load the scene data if not already loaded.
  /////////////////////////////////////////////////
  void EnsureDataLoaded() const;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor taking the scene type to load.
  ///
  /// @param scene_type Scene type to load data for
  /////////////////////////////////////////////////
  explicit DefaultSceneDataSource(SceneType scene_type);

  /////////////////////////////////////////////////
  /// @brief Get entity collection data from the scene data.
  ///
  /// @return Pointer to EntityCollection, or nullptr if unavailable
  /////////////////////////////////////////////////
  const EntityCollection *GetEntityCollection() const override;

  /////////////////////////////////////////////////
  /// @brief Get the number of entities in this data source.
  ///
  /// @return Number of entities, or 0 if unavailable
  /////////////////////////////////////////////////
  size_t GetEntityCount() const override;

  /////////////////////////////////////////////////
  /// @brief Get the source identifier for logging/debugging.
  ///
  /// @return String identifying this as a default scene data source
  /////////////////////////////////////////////////
  std::string GetSourceIdentifier() const override;

  /////////////////////////////////////////////////
  /// @brief Get the scene type this source is loading.
  ///
  /// @return The scene type
  /////////////////////////////////////////////////
  SceneType GetSceneType() const;
};

} // namespace steamrot
