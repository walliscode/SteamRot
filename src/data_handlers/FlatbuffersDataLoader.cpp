/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersDataLoader class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include "Fragment.h"
#include "fragments_generated.h"
#include <iostream>

namespace steamrot {
/////////////////////////////////////////////////
FlatbuffersDataLoader::FlatbuffersDataLoader(const EnvironmentType env_type)
    : DataLoader(env_type) {}

/////////////////////////////////////////////////
std::expected<Fragment, DataFailMode>
FlatbuffersDataLoader::ProvideFragment(const std::string &fragment_name) const {
  // check if the bin file exists
  std::filesystem::path fragment_path = m_path_provider.GetFragmentDirectory() /
                                        (fragment_name + ".fragment.bin");

  if (!std::filesystem::exists(fragment_path)) {
    std::cout << "File not found: " << fragment_path << std::endl;
    return std::unexpected(DataFailMode::FileNotFound);
  }

  const steamrot::FragmentData *fragment_data =
      GetFragmentData(LoadBinaryData(fragment_path));

  Fragment fragment;
  fragment.m_name = fragment_data->name()->str();
  return fragment;
}
} // namespace steamrot
