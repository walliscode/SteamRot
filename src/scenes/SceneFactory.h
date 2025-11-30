/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SceneFactory class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "GameContext.h"
#include "PathProvider.h"
#include "Scene.h"
#include "uuid.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <expected>
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
/// @class SceneFactory
/// @brief Generates scenes objects ready for use in the game engine.
///
/////////////////////////////////////////////////
class SceneFactory {
private:
/////////////////////////////////////////////////
  /// \brief create a uuid if none is in provided json data
  ///
/////////////////////////////////////////////////
  const uuids::uuid CreateUUID();

public:
/////////////////////////////////////////////////
  /// \brief Default constructor
  ///
/////////////////////////////////////////////////
  SceneFactory() = default;

/////////////////////////////////////////////////
  /// \brief gathers all scene creation methods
  ///
  /// @param scene_type SceneType enum indicating which scene to create
  /// @param game_context Reference to the game context
  /// @param path_provider Reference to PathProvider for data loading
/////////////////////////////////////////////////
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateDefaultScene(const SceneType &scene_type,
                     const GameContext &game_context,
                     const PathProvider &path_provider);
};
} // namespace steamrot
