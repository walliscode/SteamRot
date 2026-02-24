////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersInputActionConfigProvider.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "FlatbuffersInputActionConfigProvider.h"
#include "configure_input_action.h"

namespace steamrot {

////////////////////////////////////////////////////////////
std::expected<InputActionRegistry, FailInfo>
FlatbuffersInputActionConfigProvider::CreateInputActionRegistry() const {

  // Load the FlatBuffers binary data.
  auto config_result = m_loader.ProvideDefaultInputActionConfigFbs();
  if (!config_result.has_value()) {
    return std::unexpected(config_result.error());
  }

  // Build the registry using the configure free functions.
  InputActionRegistry registry;
  auto configure_result =
      data::configure::ConfigureInputActionRegistry(registry,
                                                    config_result.value());
  if (!configure_result.has_value()) {
    return std::unexpected(configure_result.error());
  }

  return registry;
}

} // namespace steamrot
