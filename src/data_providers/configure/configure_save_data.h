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
#include "SaveMetaData.h"
#include "save_data_generated.h"
#include <expected>
#include <variant>
namespace steamrot::data::configure {

std::expected<std::monostate, FailInfo>
ConfigureSaveMetaData(SaveMetaData &save_meta_data,
                      const SaveMetaDataFbs *save_meta_data_fbs);
}
