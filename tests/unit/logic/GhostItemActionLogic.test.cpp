/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for GhostItemActionLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GhostItemActionLogic.h"
#include "EventPayload.h"
#include "EventType.h"
#include "GhostItemState.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GhostItemActionLogic constructor does not throw",
          "[unit][GhostItemActionLogic]") {
  steamrot::tests::TestFixture fixture;
  REQUIRE_NOTHROW(
      steamrot::logic::GhostItemActionLogic(fixture.GetSceneContext()));
}

TEST_CASE("GhostItemActionLogic::GetLogicType returns GhostItemAction",
          "[unit][GhostItemActionLogic]") {
  steamrot::tests::TestFixture fixture;
  steamrot::logic::GhostItemActionLogic logic(fixture.GetSceneContext());

  REQUIRE(logic.GetLogicType() == steamrot::LogicType::GhostItemAction);
}

TEST_CASE("GhostItemActionLogic registers three subscribers on construction",
          "[unit][GhostItemActionLogic]") {
  steamrot::tests::TestFixture fixture;
  steamrot::logic::GhostItemActionLogic logic(fixture.GetSceneContext());

  // Expect: SELECT_GHOST_ITEM, CLEAR_GHOST_ITEM (LOGIC), SELECT (USER_INPUT)
  REQUIRE(logic.GetSubscribers().size() == 3);

  const auto &subs = logic.GetSubscribers();

  // First two should be LOGIC event subscribers
  REQUIRE(subs[0]->event_type == steamrot::EventType::LOGIC);
  REQUIRE(subs[1]->event_type == steamrot::EventType::LOGIC);

  // Third should be USER_INPUT subscriber
  REQUIRE(subs[2]->event_type == steamrot::EventType::USER_INPUT);
}

TEST_CASE("GhostItemActionLogic::ProcessLogic activates ghost on "
          "SELECT_GHOST_ITEM event",
          "[unit][GhostItemActionLogic]") {
  steamrot::tests::TestFixture fixture;

  auto &scene_context = fixture.GetSceneContext();
  steamrot::logic::GhostItemActionLogic logic(scene_context);

  // Manually activate the SELECT_GHOST_ITEM subscriber with item key
  auto &subs = logic.GetSubscribers();
  // subscriber[0] filters on SELECT_GHOST_ITEM
  steamrot::LogicPayload select_payload(
      steamrot::LogicPayload::LogicToggle::SELECT_GHOST_ITEM);
  select_payload.item_key = "test_fragment";
  subs[0]->m_active = true;
  subs[0]->captured_payload = select_payload;

  logic.RunLogic();

  REQUIRE(scene_context.ghost_item_state.m_is_active == true);
  REQUIRE(scene_context.ghost_item_state.m_item_key.has_value());
  REQUIRE(scene_context.ghost_item_state.m_item_key.value() == "test_fragment");
}

TEST_CASE("GhostItemActionLogic::ProcessLogic deactivates ghost on "
          "CLEAR_GHOST_ITEM event",
          "[unit][GhostItemActionLogic]") {
  steamrot::tests::TestFixture fixture;

  auto &scene_context = fixture.GetSceneContext();
  steamrot::logic::GhostItemActionLogic logic(scene_context);

  // Pre-activate the ghost item state
  scene_context.ghost_item_state.m_is_active = true;
  scene_context.ghost_item_state.m_item_key = "some_item";

  // Manually activate the CLEAR_GHOST_ITEM subscriber
  auto &subs = logic.GetSubscribers();
  // subscriber[1] filters on CLEAR_GHOST_ITEM
  steamrot::LogicPayload clear_payload(
      steamrot::LogicPayload::LogicToggle::CLEAR_GHOST_ITEM);
  subs[1]->m_active = true;
  subs[1]->captured_payload = clear_payload;

  logic.RunLogic();

  REQUIRE(scene_context.ghost_item_state.m_is_active == false);
  REQUIRE(!scene_context.ghost_item_state.m_item_key.has_value());
}

TEST_CASE("GhostItemActionLogic::ProcessLogic fires PLACE_GHOST_ITEM and "
          "clears state on SELECT input when ghost is active",
          "[unit][GhostItemActionLogic]") {
  steamrot::tests::TestFixture fixture;

  auto &scene_context = fixture.GetSceneContext();
  steamrot::logic::GhostItemActionLogic logic(scene_context);

  // Pre-activate the ghost item state
  scene_context.ghost_item_state.m_is_active = true;
  scene_context.ghost_item_state.m_item_key = "fragment_a";

  // Manually activate the USER_INPUT SELECT subscriber
  auto &subs = logic.GetSubscribers();
  // subscriber[2] filters on USER_INPUT SELECT
  subs[2]->m_active = true;

  logic.RunLogic();

  // Ghost state should be cleared after placement
  REQUIRE(scene_context.ghost_item_state.m_is_active == false);
  REQUIRE(!scene_context.ghost_item_state.m_item_key.has_value());

  // A PLACE_GHOST_ITEM event should have been added to the waiting room bus
  const auto &waiting_room =
      scene_context.event_handler.GetWaitingRoomEventBus();
  REQUIRE(!waiting_room.empty());

  bool found_place_event = false;
  for (const auto &packet : waiting_room) {
    if (packet.type == steamrot::EventType::LOGIC &&
        std::holds_alternative<steamrot::LogicPayload>(packet.payload)) {
      const auto &payload =
          std::get<steamrot::LogicPayload>(packet.payload);
      if (payload.toggle_name ==
          steamrot::LogicPayload::LogicToggle::PLACE_GHOST_ITEM) {
        found_place_event = true;
        REQUIRE(payload.item_key.has_value());
        REQUIRE(payload.item_key.value() == "fragment_a");
      }
    }
  }
  REQUIRE(found_place_event);
}

TEST_CASE("GhostItemActionLogic::ProcessLogic does not fire PLACE_GHOST_ITEM "
          "when ghost is not active",
          "[unit][GhostItemActionLogic]") {
  steamrot::tests::TestFixture fixture;

  auto &scene_context = fixture.GetSceneContext();
  steamrot::logic::GhostItemActionLogic logic(scene_context);

  // Ghost is NOT active
  scene_context.ghost_item_state.m_is_active = false;

  // Manually activate the USER_INPUT SELECT subscriber
  auto &subs = logic.GetSubscribers();
  subs[2]->m_active = true;

  logic.RunLogic();

  // No PLACE_GHOST_ITEM event should have been added
  const auto &waiting_room =
      scene_context.event_handler.GetWaitingRoomEventBus();
  bool found_place_event = false;
  for (const auto &packet : waiting_room) {
    if (packet.type == steamrot::EventType::LOGIC &&
        std::holds_alternative<steamrot::LogicPayload>(packet.payload)) {
      const auto &payload =
          std::get<steamrot::LogicPayload>(packet.payload);
      if (payload.toggle_name ==
          steamrot::LogicPayload::LogicToggle::PLACE_GHOST_ITEM) {
        found_place_event = true;
      }
    }
  }
  REQUIRE(!found_place_event);
}

TEST_CASE("GhostItemActionLogic::ProcessLogic updates ghost position to "
          "mouse position when active",
          "[unit][GhostItemActionLogic]") {
  steamrot::tests::TestFixture fixture;

  auto &scene_context = fixture.GetSceneContext();

  // Set a known mouse position and pre-activate the ghost
  scene_context.mouse_position = {150, 250};
  scene_context.ghost_item_state.m_is_active = true;
  scene_context.ghost_item_state.m_item_key = "item_x";

  steamrot::logic::GhostItemActionLogic logic(scene_context);
  logic.RunLogic();

  REQUIRE(scene_context.ghost_item_state.m_position.x == 150.f);
  REQUIRE(scene_context.ghost_item_state.m_position.y == 250.f);
}
