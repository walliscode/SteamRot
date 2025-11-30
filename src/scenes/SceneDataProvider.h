/////////////////////////////////////////////////
/// @file
/// @brief Declaration of SceneDataProvider interface class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "FailInfo.h"
#include "Scene.h"
#include "uuid.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <expected>
#include <unordered_map>
#include <vector>
namespace steamrot {

/////////////////////////////////////////////////
/// @class SceneDataProvider
/// @brief Abstract interface class for providing scene data and textures
///
/////////////////////////////////////////////////
class SceneDataProvider {
protected:
public:
/////////////////////////////////////////////////
  /// @brief Virtual destructor
/////////////////////////////////////////////////
  virtual ~SceneDataProvider() = default;

/////////////////////////////////////////////////
  /// @brief Provides a map of textures based on given scene IDs
  ///
  /// @param scene_ids Scene IDs to fetch textures for
/////////////////////////////////////////////////
  virtual const std::expected<
      std::unordered_map<uuids::uuid,
                         std::reference_wrapper<sf::RenderTexture>>,
      FailInfo>
  ProvideTextures(std::vector<uuids::uuid> &scene_ids) const = 0;

  virtual const std::expected<std::vector<SceneData>, FailInfo>
  ProvideAvailableSceneData() const = 0;
};
} // namespace steamrot
