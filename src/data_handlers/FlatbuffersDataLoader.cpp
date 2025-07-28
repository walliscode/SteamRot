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
#include <utility>

namespace steamrot {
/////////////////////////////////////////////////
FlatbuffersDataLoader::FlatbuffersDataLoader(const EnvironmentType env_type)
    : DataLoader(env_type) {}

/////////////////////////////////////////////////
std::expected<Fragment, FailureData>
FlatbuffersDataLoader::ProvideFragment(const std::string &fragment_name) const {
  // check if the bin file exists
  std::filesystem::path fragment_path = m_path_provider.GetFragmentDirectory() /
                                        (fragment_name + ".fragment.bin");

  if (!std::filesystem::exists(fragment_path)) {

    return std::unexpected(
        std::make_pair(DataFailMode::FileNotFound, "file not found"));
  }

  const steamrot::FragmentData *fragment_data =
      GetFragmentData(LoadBinaryData(fragment_path));

  Fragment fragment;

  // check every possible field, not all flatbuffers data types are required
  // as this operation is not frequent we shall make it belts and braces
  if (!fragment_data->name())
    return std::unexpected(std::make_pair(DataFailMode::FlatbufferDataNotFound,
                                          "fragment name not found"));
  fragment.m_name = fragment_data->name()->str();

  if (!fragment_data->socket_data())
    return std::unexpected(std::make_pair(DataFailMode::FlatbufferDataNotFound,
                                          "fragment socket data not found"));

  if (fragment_data->socket_data()->vertices()->size() == 0)
    return std::unexpected(
        std::make_pair(DataFailMode::FlatbufferDataNotFound,
                       "fragment socket data vertices not found"));

  for (const auto &vertex : *fragment_data->socket_data()->vertices()) {
    if (!vertex->x() || !vertex->y())
      return std::unexpected(
          std::make_pair(DataFailMode::FlatbufferDataNotFound,
                         "vertex from socket data is incomplete"));
  }
  return fragment;
}
} // namespace steamrot
