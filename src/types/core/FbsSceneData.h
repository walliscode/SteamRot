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
#include "scene_data_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @struct FbsSceneData
/// @brief FlatBuffers implementation of SceneData with pointer to binary data
///
/// This struct extends SceneData to include a pointer to the binary FlatBuffers
/// data. The pointer-based approach is used instead of pre-configuring an
/// EntityMemoryPool to optimize memory usage and startup performance.
///
/// **Design Rationale**:
/// - Memory Efficiency: Avoids temporary EntityMemoryPool allocation (~500KB-5MB)
/// - Performance: Single-pass configuration directly into Scene's pool
/// - Zero-Copy: Leverages FlatBuffers' zero-copy deserialization
///
/// **Lifetime Requirements**:
/// The scene_data_fbs pointer must remain valid during the entire configuration
/// process (until ConfigureEntities completes). The data is owned by
/// FlatbuffersDataLoader and managed via shared_ptr to ensure lifetime guarantees.
///
/// @see documentation/analysis/ENTITY_MEMORY_POOL_CONFIGURATION_STRATEGY.md
/////////////////////////////////////////////////
struct FbsSceneData : public SceneData {

  /////////////////////////////////////////////////
  /// @brief Pointer to binary FlatBuffers scene data
  ///
  /// This pointer references the parsed FlatBuffers data and must remain valid
  /// during scene configuration. Ownership is managed by the data loader.
  /////////////////////////////////////////////////
  const SceneDataFbs *scene_data_fbs{nullptr};
};
} // namespace steamrot
