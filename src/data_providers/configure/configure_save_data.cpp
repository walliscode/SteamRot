/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions to configure SaveData
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_save_data.h"

namespace steamrot::data::configure {
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSaveMetaData(SaveMetaData &save_meta_data,
                      const SaveMetaDataFbs *save_meta_data_fbs) {

  // handle null input
  if (!save_meta_data_fbs)
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "Null SaveMetaDataFbs pointer"});

  // if no save name provided then return error
  if (!save_meta_data_fbs->save_name())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "SaveMetaDataFbs missing required field: save_name"});

  // assign save name
  save_meta_data.save_name = save_meta_data_fbs->save_name()->str();

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
    save_meta_data.file_id = file_id_str.value();
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSaveData(SaveData &save_data, const SaveDataFbs *save_data_fbs) {

  // handle null input
  if (!save_data_fbs)
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "Null SaveDataFbs pointer"});

  // configure SaveMetaData
  if (!save_data_fbs->save_meta_data()) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "SaveDataFbs missing required field: save_meta_data"});
  }

  auto configure_meta_result =
      ConfigureSaveMetaData(save_data.meta_data, save_data_fbs->save_meta_data());
  if (!configure_meta_result.has_value()) {
    return std::unexpected(configure_meta_result.error());
  }

  // Note: scene_collection_data configuration would be added here
  // when the SceneCollectionData configuration functions are available
  // For now, we leave engine_snapshot.scene_collection_data empty

  return std::monostate{};
}
} // namespace steamrot::data::configure
