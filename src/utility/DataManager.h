
////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "SceneType.h"
#include "scenes_generated.h"
#include "themes_generated.h"
#include <SFML/Graphics/Font.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace steamrot {
class DataManager {

private:
  ////////////////////////////////////////////////////////////
  /// \brief check file exists, if not throw error
  ///
  ////////////////////////////////////////////////////////////
  void CheckFileExists(const std::filesystem::path &file_path);

  ////////////////////////////////////////////////////////////
  /// \brief load binary data from file and return as vector of chars
  ////////////////////////////////////////////////////////////
  char *LoadBinaryData(const std::filesystem::path &file_path);

public:
  ////////////////////////////////////////////////////////////
  /// \brief Default constructor
  ///
  ////////////////////////////////////////////////////////////
  DataManager() = default;

  ////////////////////////////////////////////////////////////
  /// \brief provide theme data as flatbuffers data
  ////////////////////////////////////////////////////////////
  const themes::UIObjects *ProvideThemeData(const std::string &theme_name);

  /**
   * @brief Provides Scene configuration data as a flatbuffer object.
   *
   * @param scene_identifier String scene identifier
   * @return Raw Pointer to SceneData flatbuffer object
   */
  const SceneData *ProvideSceneData(const SceneType &scene_type);

  std::vector<std::pair<std::string, sf::Font>>
  ProvideFonts(const SceneType &scene_type);
};

} // namespace steamrot
