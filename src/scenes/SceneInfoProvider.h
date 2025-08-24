/////////////////////////////////////////////////
/// @file
/// @brief Implementation of TextureProvider class
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
  virtual std::expected<std::unordered_map<uuids::uuid, std::reference_wrapper<
                                                            sf::RenderTexture>>,
                        FailInfo>
  ProvideTextures(std::vector<uuids::uuid> &scene_ids) = 0;

  virtual std::expected<std::vector<SceneInfo>, FailInfo>
  ProvideAvailableSceneInfo() = 0;
};
} // namespace steamrot
