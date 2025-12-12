////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of data file utility free functions
////////////////////////////////////////////////////////////

#include "data_file_utils.h"
#include <fstream>
#include <sstream>

namespace steamrot {
namespace data {
namespace file {

////////////////////////////////////////////////////////////
bool FileExists(const std::filesystem::path &file_path) {
  return std::filesystem::exists(file_path) &&
         std::filesystem::is_regular_file(file_path);
}

////////////////////////////////////////////////////////////
std::expected<std::vector<char>, FailInfo>
LoadBinaryFile(const std::filesystem::path &file_path) {
  if (!FileExists(file_path)) {
    std::ostringstream error_msg;
    error_msg << "File not found: " << file_path.string();
    return std::unexpected(FailInfo{FailMode::FileNotFound, error_msg.str()});
  }

  std::ifstream file(file_path, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    std::ostringstream error_msg;
    error_msg << "Failed to open file: " << file_path.string();
    return std::unexpected(FailInfo{FailMode::FileOpenFailure, error_msg.str()});
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  if (!file.read(buffer.data(), size)) {
    std::ostringstream error_msg;
    error_msg << "Failed to read file: " << file_path.string();
    return std::unexpected(FailInfo{FailMode::FileReadFailure, error_msg.str()});
  }

  return buffer;
}

////////////////////////////////////////////////////////////
std::expected<size_t, FailInfo>
GetFileSize(const std::filesystem::path &file_path) {
  if (!FileExists(file_path)) {
    std::ostringstream error_msg;
    error_msg << "File not found: " << file_path.string();
    return std::unexpected(FailInfo{FailMode::FileNotFound, error_msg.str()});
  }

  try {
    return std::filesystem::file_size(file_path);
  } catch (const std::filesystem::filesystem_error &e) {
    std::ostringstream error_msg;
    error_msg << "Failed to get file size: " << e.what();
    return std::unexpected(
        FailInfo{FailMode::FileSystemError, error_msg.str()});
  }
}

} // namespace file
} // namespace data
} // namespace steamrot
