
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
  bool CheckFileExists(const std::string &file_name) const;

public:
  // Virtual destructor to ensure proper cleanup of derived classes
  virtual ~DataLoader() = default;

  virtual std::expected<Fragment, DataFailMode>
  ProvideFragment(const std::string &fragment_name) const = 0;
};
} // namespace steamrot
