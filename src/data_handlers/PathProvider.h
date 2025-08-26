/////////////////////////////////////////////////
/// @file
/// @brief Declaration of PathProvider class with std::expected and FailInfo
/////////////////////////////////////////////////

#pragma once

#include "FailInfo.h"
#include <expected>
#include <filesystem>

namespace steamrot {

enum class EnvironmentType {
  None = 0,
  Test,
  Production,
};

/////////////////////////////////////////////////
/// @class PathProvider
/// @brief Provide paths for data files, with error handling via std::expected
///
/////////////////////////////////////////////////
class PathProvider {

private:
  /////////////////////////////////////////////////
  /// @brief Environment type to determine the base path
  /////////////////////////////////////////////////
  static EnvironmentType m_environment;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for PathProvider taking an environment type
  ///
  /// @param env_type Environment type enum to determine the base path
  /////////////////////////////////////////////////
  PathProvider(EnvironmentType env_type = EnvironmentType::None);

  /////////////////////////////////////////////////
  /// @brief Gets the current environment type
  ///
  /// @return EnvironmentType
  /////////////////////////////////////////////////
  EnvironmentType GetEnvironment() const;

  /////////////////////////////////////////////////
  /// @brief Provides top level data directory path
  ///
  /// @return std::expected<std::filesystem::path, FailInfo>
  /////////////////////////////////////////////////
  std::expected<std::filesystem::path, FailInfo> GetDataDirectory() const;

  /////////////////////////////////////////////////
  /// @brief Provides the path to the fragments directory
  ///
  /// @return std::expected<std::filesystem::path, FailInfo>
  /////////////////////////////////////////////////
  std::expected<std::filesystem::path, FailInfo> GetFragmentDirectory() const;

  /////////////////////////////////////////////////
  /// @brief Provides the path to the scenes directory
  ///
  /// @return std::expected<std::filesystem::path, FailInfo>
  /////////////////////////////////////////////////
  std::expected<std::filesystem::path, FailInfo> GetSceneDirectory() const;

  /////////////////////////////////////////////////
  /// @brief Provides the path to the assets directory
  /////////////////////////////////////////////////
  std::expected<std::filesystem::path, FailInfo> GetAssetsDirectory() const;

  /////////////////////////////////////////////////
  /// @brief Provides the path to the fonts directory
  /////////////////////////////////////////////////
  std::expected<std::filesystem::path, FailInfo> GetFontsDirectory() const;

  /////////////////////////////////////////////////
  /// @brief Provides the path to the ui_styles directory
  /////////////////////////////////////////////////
  std::expected<std::filesystem::path, FailInfo> GetUIStylesDirectory() const;
};

} // namespace steamrot
