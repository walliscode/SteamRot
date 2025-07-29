/////////////////////////////////////////////////
/// @file
/// @brief Definition of FlatbuffersDataLoader class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DataLoader.h"

namespace steamrot {
class FlatbuffersDataLoader : public DataLoader {

public:
  FlatbuffersDataLoader(const EnvironmentType env_type = EnvironmentType::None);
  /////////////////////////////////////////////////
  /// @brief Provides Fragment object based on the fragment name
  ///
  /// Any fail modes are provided through std::expected as the error type
  /// @param fragment_name String representing the name of the fragment
  /////////////////////////////////////////////////
  std::expected<Fragment, FailureData>
  ProvideFragment(const std::string &fragment_name) const override;
};

} // namespace steamrot
