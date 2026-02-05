/////////////////////////////////////////////////
/// @file
/// @brief Declaration of FlatbuffersGrimoireMachinaProvider class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "IGrimoireMachinaProvider.h"

namespace steamrot {

class FlatbuffersGrimoireMachinaProvider : public IGrimoireMachinaProvider {

public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor.
  /////////////////////////////////////////////////
  ~FlatbuffersGrimoireMachinaProvider() override = default;
  /////////////////////////////////////////////////
  /// @brief Create and provide a configured GrimoireMachina object.
  ///
  /// @return GrimoireMachina object or FailInfo on error.
  /////////////////////////////////////////////////
  std::expected<GrimoireMachina, FailInfo>
  CreateGrimoireMachina() const override;
  /////////////////////////////////////////////////
  /// @brief Configure the provided GrimoireMachina object.
  ///
  /// @param grimoire_machina GrimoireMachina object to configure.
  /// @return std::monostate on success or FailInfo on error.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureGrimoireMachina(GrimoireMachina &grimoire_machina) const override;
};
} // namespace steamrot
