
/// @file
/// @brief Declarataion of abstract DataLoader class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "Fragment.h"
#include "PathProvider.h"
#include <expected>
#include <string>
namespace steamrot {

enum class DataFailMode {
  FileNotFound,
};
/////////////////////////////////////////////////
/// @class DataLoader
/// @brief Abstract base class for data loading
///
/////////////////////////////////////////////////
class DataLoader {

private:
  /////////////////////////////////////////////////
  /// @brief member variable to hold the path provider instance
  /////////////////////////////////////////////////
  PathProvider m_path_provider;

  bool CheckFileExists(const std::string &file_name) const;

public:
  // Virtual destructor to ensure proper cleanup of derived classes
  virtual ~DataLoader() = default;

  /////////////////////////////////////////////////
  /// @brief Constructor for DataLoader taking an environment type
  ///
  /// @param env_type [TODO:parameter]
  /////////////////////////////////////////////////
  DataLoader(const EnvironmentType env_type = EnvironmentType::None);

  virtual std::expected<Fragment, DataFailMode>
  ProvideFragment(const std::string &fragment_name) const = 0;
};
} // namespace steamrot
