/////////////////////////////////////////////////
/// @file
/// @brief Declarration of FlatbuffersLogicConfigCollectionProvider class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ILogicConfigCollectionProvider.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @class FlatbuffersLogicConfigCollectionProvider
/// @brief Provider of LogicConfigCollection objects configured using
/// Flatbuffers.
/////////////////////////////////////////////////
class FlatbuffersLogicConfigCollectionProvider
    : public ILogicConfigCollectionProvider {

public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor.
  /////////////////////////////////////////////////
  virtual ~FlatbuffersLogicConfigCollectionProvider() = default;
  /////////////////////////////////////////////////
  /// @brief Provide a configured LogicConfigCollection object.
  ///
  /// @return Configured LogicConfigCollection object.
  /////////////////////////////////////////////////
  std::expected<LogicConfigCollection, FailInfo>
  CreateLogicConfigCollection() const override;
  /////////////////////////////////////////////////
  /// @brief Configure the provided LogicConfigCollection object.
  ///
  /// @param logic_config_collection LogicConfigCollection object to configure.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ConfigureLogicConfigCollection(
      LogicConfigCollection &logic_config_collection) const override;
};

} // namespace steamrot
