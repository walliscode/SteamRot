/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the free functions for the configuration of the logic
/// config.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "LogicConfig.h"
#include "logic_config_generated.h"
#include <expected>
#include <variant>

namespace steamrot::data::configure {

/////////////////////////////////////////////////
/// @brief Configures the logic config from the logic config flatbuffers.
///
/// @param logic_config LogicConfig object to configure.
/// @param logic_config_fbs LogicConfigFbs flatbuffers object to configure from.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureLogicConfig(LogicConfig &logic_config,
                     const LogicConfigFbs *logic_config_fbs);
} // namespace steamrot::data::configure
