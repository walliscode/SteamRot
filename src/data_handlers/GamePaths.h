/////////////////////////////////////////////////
/// @file
/// @brief Declaration of GamePaths class for production environment paths
/////////////////////////////////////////////////

#pragma once

#include "PathProvider.h"
#include <filesystem>

namespace steamrot {

/////////////////////////////////////////////////
/// @class GamePaths
/// @brief Provides paths for production game data files
///
/// GamePaths inherits from PathProvider and returns paths pointing to
/// the production data directory (data).
/////////////////////////////////////////////////
class GamePaths : public PathProvider {

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor
  /////////////////////////////////////////////////
  GamePaths() = default;

  /////////////////////////////////////////////////
  /// @brief Virtual destructor
  /////////////////////////////////////////////////
  ~GamePaths() override = default;

  /////////////////////////////////////////////////
  /// @brief Provides the production data directory path
  ///
  /// @return std::filesystem::path The production data directory path
  /////////////////////////////////////////////////
  std::filesystem::path GetDataDirectory() const override;
};

} // namespace steamrot
