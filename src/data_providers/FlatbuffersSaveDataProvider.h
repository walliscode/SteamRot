/////////////////////////////////////////////////
/// @file
/// @brief Declaration of FlatbuffersSaveDataProvider class, which implements
/// ISaveDataProvider
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ISaveDataProvider.h"
#include "SaveData.h"
#include "SaveMetaData.h"
#include "save_data_generated.h"
#include <expected>

namespace steamrot {

class FlatbuffersSaveDataProvider : public ISaveDataProvider {

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for FlatbuffersSaveDataProvider
  /////////////////////////////////////////////////
  FlatbuffersSaveDataProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Configures SaveMetaData from Flatbuffers data.
  ///
  /// @param save_meta_data SaveMetaData object to configure
  /// @param save_meta_data_fbs Flatbuffers SaveMetaData data
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSaveMetaData(SaveMetaData &save_meta_data,
                        const SaveMetaDataFbs *save_meta_data_fbs) const;

  /////////////////////////////////////////////////
  /// @brief Provides a SaveData object populated from Flatbuffers data loaded
  /// from file.
  /////////////////////////////////////////////////
  std::expected<SaveData, FailInfo> ProvideSaveData() const override;

  /////////////////////////////////////////////////
  /// @brief Convert FlatBuffers SaveDataFbs to native SaveData.
  ///
  /// This method allows passing pre-loaded FlatBuffers data directly
  /// for conversion without file I/O.
  ///
  /// @param fb_save_data Pointer to FlatBuffers SaveDataFbs
  /// @return Native SaveData object, or error
  /////////////////////////////////////////////////
  std::expected<SaveData, FailInfo>
  ConvertSaveData(const SaveDataFbs *fb_save_data) const override;
};
} // namespace steamrot
