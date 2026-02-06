/////////////////////////////////////////////////
/// @file
/// @brief Declarration of ILogicConfigProvider interface
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
#include <expected>
#include <variant>

namespace steamrot {

class ILogicConfigCollectionProvider {

public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor.
  /////////////////////////////////////////////////
  virtual ~ILogicConfigCollectionProvider() = default;

  /////////////////////////////////////////////////
  /// @brief Provide a configured LogicConfigCollection object.
  ///
  /// @return Configured LogicConfigCollection object.
  /////////////////////////////////////////////////
  virtual std::expected<LogicConfigCollection, FailInfo>
  CreateLogicConfigCollection() const = 0;

  /////////////////////////////////////////////////
  /// @brief Configure the provided LogicConfigCollection object.
  ///
  /// @param logic_config_collection LogicConfigCollection object to configure.
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureLogicConfigCollection(
      LogicConfigCollection &logic_config_collection) const = 0;
};
} // namespace steamrot
