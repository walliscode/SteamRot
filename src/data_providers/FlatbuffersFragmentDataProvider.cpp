/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersFragmentDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersFragmentDataProvider.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<Fragment, FailInfo>
FlatbuffersFragmentDataProvider::LoadFragment(
    const std::string &fragment_name) const {
  // Delegate to existing loader - it already returns Fragment struct
  return m_loader.ProvideFragment(fragment_name);
}

/////////////////////////////////////////////////
std::expected<std::map<std::string, Fragment>, FailInfo>
FlatbuffersFragmentDataProvider::LoadAllFragments(
    const std::vector<std::string> &fragment_names) const {
  // Delegate to existing loader - it already returns map<string, Fragment>
  return m_loader.ProvideAllFragments(fragment_names);
}

} // namespace steamrot
