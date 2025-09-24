
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
  /// @brief Default constructor
  /////////////////////////////////////////////////
  DataLoader() = default;

  /////////////////////////////////////////////////
  /// @brief Provide a Fragment object given its name
  ///
  /// @param fragment_name String name of the fragment to be loaded
  /////////////////////////////////////////////////
  virtual std::expected<Fragment, FailInfo>
  ProvideFragment(const std::string &fragment_name) const = 0;

  /////////////////////////////////////////////////
  /// @brief Give a list of fragment names, provide a map of Fragment objects
  ///
  /// @param fragment_names Fragment names to be loaded
  /////////////////////////////////////////////////
  virtual std::expected<std::map<std::string, Fragment>, FailInfo>
  ProvideAllFragments(std::vector<std::string> fragment_names) const = 0;
};
} // namespace steamrot
