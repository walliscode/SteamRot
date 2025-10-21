/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for Component equality operators
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CGrimoireMachina.h"
#include "CMachinaForm.h"
#include "CMeta.h"
#include "CUIState.h"
#include "CUserInterface.h"
#include "component_matchers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CMeta equality operator works correctly",
          "[unit][Components][CMeta][equality]") {
  steamrot::CMeta meta1;
  steamrot::CMeta meta2;

  SECTION("Default constructed CMeta objects are equal") {
    REQUIRE(meta1 == meta2);
  }

  SECTION("CMeta objects with different m_active are not equal") {
    meta1.m_active = true;
    meta2.m_active = false;
    REQUIRE_FALSE(meta1 == meta2);
  }

  SECTION("CMeta objects with different m_entity_active are not equal") {
    meta1.m_entity_active = true;
    meta2.m_entity_active = false;
    REQUIRE_FALSE(meta1 == meta2);
  }

  SECTION("CMeta objects with same values are equal") {
    meta1.m_active = true;
    meta1.m_entity_active = true;
    meta2.m_active = true;
    meta2.m_entity_active = true;
    REQUIRE(meta1 == meta2);
  }
}

TEST_CASE("CMeta spaceship operator works correctly",
          "[unit][Components][CMeta][spaceship]") {
  steamrot::CMeta meta1;
  steamrot::CMeta meta2;

  SECTION("Default constructed CMeta objects compare equal") {
    REQUIRE((meta1 <=> meta2) == std::strong_ordering::equal);
  }

  SECTION("CMeta with m_active=true > m_active=false") {
    meta1.m_active = true;
    meta2.m_active = false;
    REQUIRE((meta1 <=> meta2) != std::strong_ordering::equal);
  }
}

TEST_CASE("CUserInterface equality operator works correctly",
          "[unit][Components][CUserInterface][equality]") {
  steamrot::CUserInterface ui1;
  steamrot::CUserInterface ui2;

  SECTION("Default constructed CUserInterface objects are equal") {
    REQUIRE(ui1 == ui2);
  }

  SECTION("CUserInterface with different m_name are not equal") {
    ui1.m_name = "UI1";
    ui2.m_name = "UI2";
    REQUIRE_FALSE(ui1 == ui2);
  }

  SECTION("CUserInterface with different m_UI_visible are not equal") {
    ui1.m_UI_visible = true;
    ui2.m_UI_visible = false;
    REQUIRE_FALSE(ui1 == ui2);
  }

  SECTION("CUserInterface with same values are equal") {
    ui1.m_name = "TestUI";
    ui1.m_UI_visible = true;
    ui2.m_name = "TestUI";
    ui2.m_UI_visible = true;
    REQUIRE(ui1 == ui2);
  }
}

TEST_CASE("CMachinaForm equality operator works correctly",
          "[unit][Components][CMachinaForm][equality]") {
  steamrot::CMachinaForm form1;
  steamrot::CMachinaForm form2;

  SECTION("Default constructed CMachinaForm objects are equal") {
    REQUIRE(form1 == form2);
  }

  SECTION("CMachinaForm with different m_active are not equal") {
    form1.m_active = true;
    form2.m_active = false;
    REQUIRE_FALSE(form1 == form2);
  }
}

TEST_CASE("CMachinaForm spaceship operator works correctly",
          "[unit][Components][CMachinaForm][spaceship]") {
  steamrot::CMachinaForm form1;
  steamrot::CMachinaForm form2;

  SECTION("Default constructed CMachinaForm objects compare equal") {
    REQUIRE((form1 <=> form2) == std::strong_ordering::equal);
  }
}

TEST_CASE("CGrimoireMachina equality operator works correctly",
          "[unit][Components][CGrimoireMachina][equality]") {
  steamrot::CGrimoireMachina grimoire1;
  steamrot::CGrimoireMachina grimoire2;

  SECTION("Default constructed CGrimoireMachina objects are equal") {
    REQUIRE(grimoire1 == grimoire2);
  }

  SECTION("CGrimoireMachina with different m_active are not equal") {
    grimoire1.m_active = true;
    grimoire2.m_active = false;
    REQUIRE_FALSE(grimoire1 == grimoire2);
  }

  SECTION("CGrimoireMachina with same holding_form nullptr are equal") {
    grimoire1.m_holding_form = nullptr;
    grimoire2.m_holding_form = nullptr;
    REQUIRE(grimoire1 == grimoire2);
  }

  SECTION("CGrimoireMachina with different holding_form pointers") {
    grimoire1.m_holding_form = std::make_unique<steamrot::CMachinaForm>();
    grimoire2.m_holding_form = nullptr;
    REQUIRE_FALSE(grimoire1 == grimoire2);
  }
}

TEST_CASE("CUIState equality operator works correctly",
          "[unit][Components][CUIState][equality]") {
  steamrot::CUIState state1;
  steamrot::CUIState state2;

  SECTION("Default constructed CUIState objects are equal") {
    REQUIRE(state1 == state2);
  }

  SECTION("CUIState with different m_state_values are not equal") {
    state1.m_state_values["key1"] = true;
    state2.m_state_values["key1"] = false;
    REQUIRE_FALSE(state1 == state2);
  }

  SECTION("CUIState with same m_state_values are equal") {
    state1.m_state_values["key1"] = true;
    state2.m_state_values["key1"] = true;
    REQUIRE(state1 == state2);
  }
}

TEST_CASE("UIVisibilityState equality operator works correctly",
          "[unit][Components][UIVisibilityState][equality]") {
  steamrot::UIVisibilityState vis1;
  steamrot::UIVisibilityState vis2;

  SECTION("Default constructed UIVisibilityState objects are equal") {
    REQUIRE(vis1 == vis2);
  }

  SECTION("UIVisibilityState with different m_ui_indices_on are not equal") {
    vis1.m_ui_indices_on = {1, 2, 3};
    vis2.m_ui_indices_on = {1, 2};
    REQUIRE_FALSE(vis1 == vis2);
  }

  SECTION("UIVisibilityState with same indices are equal") {
    vis1.m_ui_indices_on = {1, 2, 3};
    vis1.m_ui_indices_off = {4, 5};
    vis2.m_ui_indices_on = {1, 2, 3};
    vis2.m_ui_indices_off = {4, 5};
    REQUIRE(vis1 == vis2);
  }
}
