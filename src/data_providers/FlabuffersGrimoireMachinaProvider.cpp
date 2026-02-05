/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersGrimoireMachinaProvider class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersGrimoireMachinaProvider.h"

namespace steamrot {
/////////////////////////////////////////////////
std::expected<GrimoireMachina, FailInfo>
FlatbuffersGrimoireMachinaProvider::CreateGrimoireMachina() const {
  GrimoireMachina grimoire_machina;
  return grimoire_machina;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersGrimoireMachinaProvider::ConfigureGrimoireMachina(
    GrimoireMachina &grimoire_machina) const {

  return std::monostate{};
}

} // namespace steamrot
