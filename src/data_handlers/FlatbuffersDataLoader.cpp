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
  // check if the bin file exists
  std::filesystem::path fragment_path =
      m_path_provider.GetFragmentDirectory() / (fragment_name + ".bin");

  if (!std::filesystem::exists(fragment_path))
    return std::unexpected(DataFailMode::FileNotFound);

  return Fragment();
}
} // namespace steamrot
