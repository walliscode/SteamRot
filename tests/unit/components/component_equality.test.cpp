/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for Component matchers
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CGrimoireMachina.h"
#include "CMachinaForm.h"
#include "CMeta.h"
#include "CUIState.h"
#include "CUserInterface.h"
#include "CGrimoireMachinaEqualsMatcher.h"
#include "cmachina_form_matchers.h"
#include "CMetaEqualsMatcher.h"
#include "cui_state_matchers.h"
#include "cuser_interface_matchers.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

TEST_CASE("CMeta matcher works correctly",
          "[unit][Components][CMeta][equality]") {
  steamrot::CMeta meta1;
  steamrot::CMeta meta2;

  SECTION("Default constructed CMeta objects are equal") {
    REQUIRE_THAT(meta1, steamrot::tests::EqualsCMeta(meta2));
  }

  SECTION("CMeta objects with different m_active are not equal") {
    meta1.m_active = true;
    meta2.m_active = false;
    REQUIRE_THAT(meta1, !steamrot::tests::EqualsCMeta(meta2));
  }

  SECTION("CMeta objects with different m_entity_active are not equal") {
    meta1.m_entity_active = true;
    meta2.m_entity_active = false;
    REQUIRE_THAT(meta1, !steamrot::tests::EqualsCMeta(meta2));
  }

  SECTION("CMeta objects with same values are equal") {
    meta1.m_active = true;
    meta1.m_entity_active = true;
    meta2.m_active = true;
    meta2.m_entity_active = true;
    REQUIRE_THAT(meta1, steamrot::tests::EqualsCMeta(meta2));
  }
}

TEST_CASE("CUserInterface matcher works correctly",
          "[unit][Components][CUserInterface][equality]") {
  steamrot::CUserInterface ui1;
  steamrot::CUserInterface ui2;

  SECTION("Default constructed CUserInterface objects are equal") {
    REQUIRE_THAT(ui1, steamrot::tests::EqualsCUserInterface(ui2));
  }

  SECTION("CUserInterface with different m_name are not equal") {
    ui1.m_name = "UI1";
    ui2.m_name = "UI2";
    REQUIRE_THAT(ui1, !steamrot::tests::EqualsCUserInterface(ui2));
  }

  SECTION("CUserInterface with different m_UI_visible are not equal") {
    ui1.m_UI_visible = true;
    ui2.m_UI_visible = false;
    REQUIRE_THAT(ui1, !steamrot::tests::EqualsCUserInterface(ui2));
  }

  SECTION("CUserInterface with same values are equal") {
    ui1.m_name = "TestUI";
    ui1.m_UI_visible = true;
    ui2.m_name = "TestUI";
    ui2.m_UI_visible = true;
    REQUIRE_THAT(ui1, steamrot::tests::EqualsCUserInterface(ui2));
  }
}

TEST_CASE("CMachinaForm matcher works correctly",
          "[unit][Components][CMachinaForm][equality]") {
  steamrot::CMachinaForm form1;
  steamrot::CMachinaForm form2;

  SECTION("Default constructed CMachinaForm objects are equal") {
    REQUIRE_THAT(form1, steamrot::tests::EqualsCMachinaForm(form2));
  }

  SECTION("CMachinaForm with different m_active are not equal") {
    form1.m_active = true;
    form2.m_active = false;
    REQUIRE_THAT(form1, !steamrot::tests::EqualsCMachinaForm(form2));
  }
}

TEST_CASE("CGrimoireMachina matcher works correctly",
          "[unit][Components][CGrimoireMachina][equality]") {
  steamrot::CGrimoireMachina grimoire1;
  steamrot::CGrimoireMachina grimoire2;

  SECTION("Default constructed CGrimoireMachina objects are equal") {
    REQUIRE_THAT(grimoire1, steamrot::tests::EqualsCGrimoireMachina(grimoire2));
  }

  SECTION("CGrimoireMachina with different m_active are not equal") {
    grimoire1.m_active = true;
    grimoire2.m_active = false;
    REQUIRE_THAT(grimoire1, !steamrot::tests::EqualsCGrimoireMachina(grimoire2));
  }

  SECTION("CGrimoireMachina with same holding_form nullptr are equal") {
    grimoire1.m_holding_form = nullptr;
    grimoire2.m_holding_form = nullptr;
    REQUIRE_THAT(grimoire1, steamrot::tests::EqualsCGrimoireMachina(grimoire2));
  }

  SECTION("CGrimoireMachina with different holding_form pointers") {
    grimoire1.m_holding_form = std::make_unique<steamrot::CMachinaForm>();
    grimoire2.m_holding_form = nullptr;
    REQUIRE_THAT(grimoire1, !steamrot::tests::EqualsCGrimoireMachina(grimoire2));
  }
}

TEST_CASE("CUIState matcher works correctly",
          "[unit][Components][CUIState][equality]") {
  steamrot::CUIState state1;
  steamrot::CUIState state2;

  SECTION("Default constructed CUIState objects are equal") {
    REQUIRE_THAT(state1, steamrot::tests::EqualsCUIState(state2));
  }

  SECTION("CUIState with different m_state_values are not equal") {
    state1.m_state_values["key1"] = true;
    state2.m_state_values["key1"] = false;
    REQUIRE_THAT(state1, !steamrot::tests::EqualsCUIState(state2));
  }

  SECTION("CUIState with same m_state_values are equal") {
    state1.m_state_values["key1"] = true;
    state2.m_state_values["key1"] = true;
    REQUIRE_THAT(state1, steamrot::tests::EqualsCUIState(state2));
  }
}

TEST_CASE("UIVisibilityState matcher works correctly",
          "[unit][Components][UIVisibilityState][equality]") {
  steamrot::UIVisibilityState vis1;
  steamrot::UIVisibilityState vis2;

  SECTION("Default constructed UIVisibilityState objects are equal") {
    REQUIRE_THAT(vis1, steamrot::tests::EqualsUIVisibilityState(vis2));
  }

  SECTION("UIVisibilityState with different m_ui_indices_on are not equal") {
    vis1.m_ui_indices_on = {1, 2, 3};
    vis2.m_ui_indices_on = {1, 2};
    REQUIRE_THAT(vis1, !steamrot::tests::EqualsUIVisibilityState(vis2));
  }

  SECTION("UIVisibilityState with same indices are equal") {
    vis1.m_ui_indices_on = {1, 2, 3};
    vis1.m_ui_indices_off = {4, 5};
    vis2.m_ui_indices_on = {1, 2, 3};
    vis2.m_ui_indices_off = {4, 5};
    REQUIRE_THAT(vis1, steamrot::tests::EqualsUIVisibilityState(vis2));
  }
}
