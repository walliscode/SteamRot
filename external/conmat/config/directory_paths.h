#include <filesystem>

namespace directory_paths {

////////////////////////////////////////////////////////////
/// \brief return folder path for data,
/// this should be used for other paths
///
////////////////////////////////////////////////////////////
const std::filesystem::path getDataFolder();

} // namespace directory_paths
