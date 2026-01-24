/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions to configure SaveData
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "FailInfo.h"
#include "SaveData.h"
#include "SaveMetaData.h"
#include "save_data_generated.h"
#include <expected>
#include <variant>
namespace steamrot::data::configure {

/////////////////////////////////////////////////
/// @brief Logic for populating SaveMetaData from flatbuffers data source.
///
/// @param save_meta_data Reference to SaveMetaData to populate.
/// @param save_meta_data_fbs Pointer to SaveMetaDataFbs flatbuffers data.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSaveMetaData(SaveMetaData &save_meta_data,
                      const SaveMetaDataFbs *save_meta_data_fbs);

/////////////////////////////////////////////////
/// @brief Logic for populating SaveData from flatbuffers data source.
///
/// @param save_data Reference to SaveData to populate.
/// @param save_data_fbs Pointer to SaveDataFbs flatbuffers data.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSaveData(SaveData &save_data, const SaveDataFbs *save_data_fbs);
}
