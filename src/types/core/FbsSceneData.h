/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Flatbuffers implementation of SceneData struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneData.h"
#include "entities_generated.h"
#include "scene_resources_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @struct FbsSceneData
/// @brief FlatBuffers implementation of SceneData with pointers to binary data
///
/// This struct extends SceneData to include pointers to specific FlatBuffers
/// data needed for scene configuration. The pointer-based approach is used
/// instead of pre-configuring an EntityMemoryPool to optimize memory usage
/// and startup performance.
///
/// The base SceneData struct handles scene_info and asset_config, while this
/// derived struct provides access to entity and resource configuration data.
/// This avoids duplication - we don't store pointers to data that's already
/// represented in the base struct members.
///
/// **Design Rationale**:
/// - Memory Efficiency: Avoids temporary EntityMemoryPool allocation (~500KB-5MB)
/// - Performance: Single-pass configuration directly into Scene's pool
/// - Zero-Copy: Leverages FlatBuffers' zero-copy deserialization
/// - No Duplication: Only pointers to data not in base struct
///
/// **Lifetime Requirements**:
/// These pointers must remain valid during the entire configuration process
/// (until ConfigureEntities and ConfigureSceneResources complete). The data
/// is owned by FlatbuffersDataLoader and managed via shared_ptr to ensure
/// lifetime guarantees.
///
/// @see documentation/analysis/ENTITY_MEMORY_POOL_CONFIGURATION_STRATEGY.md
/////////////////////////////////////////////////
struct FbsSceneData : public SceneData {

  /////////////////////////////////////////////////
  /// @brief Pointer to binary FlatBuffers entity collection data
  ///
  /// This pointer references the parsed EntityCollectionFbs data and must
  /// remain valid during entity configuration. Ownership is managed by the
  /// data loader.
  /////////////////////////////////////////////////
  const EntityCollectionFbs *entity_collection_fbs{nullptr};

  /////////////////////////////////////////////////
  /// @brief Pointer to binary FlatBuffers scene resources data
  ///
  /// This pointer references the parsed SceneResourcesFbs data and must
  /// remain valid during scene resource configuration. Ownership is managed
  /// by the data loader.
  /////////////////////////////////////////////////
  const SceneResourcesFbs *scene_resources_fbs{nullptr};
};
} // namespace steamrot
