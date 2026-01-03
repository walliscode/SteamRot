/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersSaveDataProvider class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSaveDataProvider.h"

namespace steamrot {
/////////////////////////////////////////////////
std::expected<SaveData, FailInfo>
FlatbuffersSaveDataProvider::ProvideSaveData() const {

  // create SaveData object to populate
  SaveData save_data;

  // return populated SaveData object
  return save_data;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSaveDataProvider::ConfigureSaveMetaData(
    SaveMetaData &save_meta_data,
    const SaveMetaDataFbs *save_meta_data_fbs) const {

  // handle null input
  if (!save_meta_data_fbs)
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "Null SaveMetaDataFbs pointer"});

  // if no save name provided then return error
  if (!save_meta_data_fbs->save_name())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "SaveMetaDataFbs missing required field: save_name"});

  // check if is present
  if (!save_meta_data_fbs->file_id()) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "SaveMetaDataFbs missing required field: file_id"});
  }

  // try to parse UUID
  auto file_id_str =
      uuids::uuid::from_string(save_meta_data_fbs->file_id()->str());

  // if parsing failed then return error
  if (!file_id_str.has_value()) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "SaveMetaDataFbs file_id is not a valid UUID string"});
  } else {
    // save_meta_data.file_id = file_id_str.value();
  }

  return std::monostate{};
}
} // namespace steamrot
