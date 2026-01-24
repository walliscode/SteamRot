/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersSaveDataProvider class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSaveDataProvider.h"
#include "FailInfo.h"
#include "configure_save_data.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<SaveData, FailInfo>
FlatbuffersSaveDataProvider::CreateSaveData() const {

  // create return object
  SaveData data;

  // configure save data
  auto configure_result = ConfigureSaveData(data);
  if (!configure_result.has_value()) {
    return std::unexpected(configure_result.error());
  }

  return data;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSaveDataProvider::ConfigureSaveData(SaveData &save_data) const {

  // TODO: When save file loading is implemented, this would use
  // m_loader.ProvideSaveDataFbs() to get the SaveDataFbs pointer
  // For now, we return an error indicating no save data is available
  
  // Note: This is a placeholder implementation. Once save file loading
  // is implemented in FlatbuffersDataLoader, this method should:
  // 1. Call m_loader.ProvideSaveDataFbs() or similar method
  // 2. Pass the result to data::configure::ConfigureSaveData()
  // 3. Return the configuration result
  
  return std::unexpected(
      FailInfo{FailMode::FileNotFound,
               "No save data file available for loading. Save file loading "
               "is not yet implemented."});
}

} // namespace steamrot
