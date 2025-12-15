/////////////////////////////////////////////////
/// @file
/// @brief Implementation of TextureProvider class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "SceneInfo.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <expected>
#include <unordered_map>
#include <uuid/uuid.h>
#include <vector>
namespace steamrot {

/////////////////////////////////////////////////
/// @class TextureProvider
/// @brief Abstract interface class for providing sf::Texture objects
///
/////////////////////////////////////////////////
class SceneInfoProvider {
protected:
public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor
  /////////////////////////////////////////////////
  virtual ~SceneInfoProvider() = default;

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

  virtual const std::expected<std::vector<SceneInfo>, FailInfo>
  ProvideAvailableSceneInfo() const = 0;
};
} // namespace steamrot
