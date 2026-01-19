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
FlatbuffersSaveDataProvider::CreateSaveData() const {

  // create SaveData object to populate
  SaveData save_data;

  // return populated SaveData object
  return save_data;
}

} // namespace steamrot
