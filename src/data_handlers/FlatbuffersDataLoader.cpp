/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersDataLoader class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include "Fragment.h"

namespace steamrot {
/////////////////////////////////////////////////
FlatbuffersDataLoader::FlatbuffersDataLoader(const EnvironmentType env_type)
    : DataLoader(env_type) {}
/////////////////////////////////////////////////
std::expected<Fragment, DataFailMode>
FlatbuffersDataLoader::ProvideFragment(const std::string &fragment_name) const {

  return std::unexpected(DataFailMode::FileNotFound);
}
} // namespace steamrot
