/////////////////////////////////////////////////
/// @file
/// @brief unit tests for the GrimoireMachina action processing functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_grimoire_machina.h"
#include "MachinaFormScaffold.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("InitialiseActiveMachinaForm adds a new MachinaForm to the "
          "GrimoireMachina active form",

          "[GrimoireMachina]") {
  steamrot::GrimoireMachina grimoire_machina;

  auto result = steamrot::logic::actions::grimoire_machina::
      InitialiseActiveMachinaFormScaffold(grimoire_machina);
  REQUIRE(result.has_value());
  REQUIRE(grimoire_machina.m_scaffold_form != nullptr);
}

TEST_CASE("ClearActiveMachinaForm clears the active MachinaForm in the "
          "GrimoireMachina",
          "[GrimoireMachina]") {
  steamrot::GrimoireMachina grimoire_machina;
  grimoire_machina.m_scaffold_form =
      std::make_unique<steamrot::MachinaFormScaffold>();
  auto result = steamrot::logic::actions::grimoire_machina::
      ClearActiveMachinaFormScaffold(grimoire_machina);
  REQUIRE(result.has_value());
  REQUIRE(grimoire_machina.m_scaffold_form == nullptr);
}
