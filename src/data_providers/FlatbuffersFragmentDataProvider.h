/////////////////////////////////////////////////
/// @file
/// @brief FlatBuffers implementation of IFragmentDataProvider.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "IFragmentDataProvider.h"
#include "FlatbuffersDataLoader.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersFragmentDataProvider
/// @brief FlatBuffers implementation of IFragmentDataProvider.
///
/// Loads fragment data from FlatBuffers binary files.
/////////////////////////////////////////////////
class FlatbuffersFragmentDataProvider : public IFragmentDataProvider {
private:
  FlatbuffersDataLoader m_loader;

public:
  FlatbuffersFragmentDataProvider() = default;

  std::expected<Fragment, FailInfo>
  LoadFragment(const std::string &fragment_name) const override;

  std::expected<std::map<std::string, Fragment>, FailInfo>
  LoadAllFragments(const std::vector<std::string> &fragment_names) const override;
};

} // namespace steamrot
