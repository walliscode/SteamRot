
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

#include "FailInfo.h"
#include "Fragment.h"
#include "PathProvider.h"
#include <expected>
#include <map>
#include <string>

namespace steamrot {

enum class DataFailMode {
  FunctionNotImplemented,
  FileNotFound,
  FlatbufferDataNotFound,
  InvalidSceneType,
};

/////////////////////////////////////////////////
/// @class DataLoader
/// @brief Abstract base class for data loading
///
/////////////////////////////////////////////////
class DataLoader {

protected:
  /////////////////////////////////////////////////
  /// @brief member variable to hold the path provider instance
  /////////////////////////////////////////////////
  PathProvider m_path_provider;

  ////////////////////////////////////////////////////////////
  /// \brief load binary data from file and return as vector of chars
  ////////////////////////////////////////////////////////////
  char *LoadBinaryData(const std::filesystem::path &file_path) const;

public:
  // Virtual destructor to ensure proper cleanup of derived classes
  virtual ~DataLoader() = default;

  /////////////////////////////////////////////////
  /// @brief Constructor for DataLoader taking an environment type
  ///
  /// @param env_type [TODO:parameter]
  /////////////////////////////////////////////////
  DataLoader(const EnvironmentType env_type = EnvironmentType::None);

  virtual std::expected<Fragment, FailInfo>
  ProvideFragment(const std::string &fragment_name) const = 0;

  virtual std::expected<std::map<std::string, Fragment>, FailInfo>
  ProvideAllFragments(std::vector<std::string> fragment_names) const = 0;
};
} // namespace steamrot
