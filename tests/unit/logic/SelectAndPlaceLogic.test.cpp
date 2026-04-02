/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the SelectAndPlaceLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SelectAndPlaceLogic.h"
#include "EventHandler.h"
#include "EventPayload.h"
#include "EventType.h"
#include "TestFixture.h"
#include "event_factory.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SelectAndPlaceLogic constructs without errors",
          "[unit][SelectAndPlaceLogic]") {
  steamrot::tests::TestFixture fixture;
  REQUIRE_NOTHROW(
      steamrot::logic::SelectAndPlaceLogic logic(fixture.GetSceneContext()));
}

TEST_CASE("SelectAndPlaceLogic registers SELECT_AND_PLACE and USER_INPUT "
          "subscribers on construction",
          "[unit][SelectAndPlaceLogic]") {
  steamrot::tests::TestFixture fixture;
  steamrot::logic::SelectAndPlaceLogic logic(fixture.GetSceneContext());

  const auto &sub_register =
      fixture.GetSceneContext().event_handler.GetSubcriberRegister();

  REQUIRE(sub_register.count(steamrot::EventType::SELECT_AND_PLACE) == 1);
  REQUIRE(sub_register.count(steamrot::EventType::USER_INPUT) == 1);
  REQUIRE(sub_register.at(steamrot::EventType::SELECT_AND_PLACE).size() >= 1);
  REQUIRE(sub_register.at(steamrot::EventType::USER_INPUT).size() >= 1);
}

TEST_CASE(
    "SelectAndPlaceLogic fires PLACE_ITEM after SELECT_ITEM then SELECT input",
    "[unit][SelectAndPlaceLogic]") {
  steamrot::tests::TestFixture fixture;
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;

  steamrot::logic::SelectAndPlaceLogic logic(fixture.GetSceneContext());

  // --- Tick 1: deliver a SELECT_ITEM event to select the fragment ---
  auto select_packet = steamrot::events::CreateSelectAndPlaceEventPacket(
      1, steamrot::SelectAndPlacePayload::Action::SELECT_ITEM, "MyFrag",
      "fragment");
  REQUIRE(select_packet.has_value());
  event_handler.AddEvent(select_packet.value());
  event_handler.ProcessWaitingRoomEventBus();
  event_handler.UpdateSubscribersFromGlobalEventBus();

  // RunLogic: SelectAndPlaceLogic stores the selection
  REQUIRE_NOTHROW(logic.RunLogic());

  // --- Tick 2: deliver a USER_INPUT SELECT event to trigger placement ---
  auto input_packet = steamrot::events::CreateInputEventPacket(
      1, steamrot::InputPayload::InputAction::SELECT);
  REQUIRE(input_packet.has_value());
  event_handler.AddEvent(input_packet.value());
  event_handler.ProcessWaitingRoomEventBus();
  event_handler.UpdateSubscribersFromGlobalEventBus();

  REQUIRE_NOTHROW(logic.RunLogic());

  // The PLACE_ITEM event should now be in the waiting room
  const auto &waiting_room = event_handler.GetWaitingRoomEventBus();
  bool place_item_found = false;
  for (const auto &packet : waiting_room) {
    if (packet.type == steamrot::EventType::SELECT_AND_PLACE &&
        std::holds_alternative<steamrot::SelectAndPlacePayload>(packet.payload)) {
      const auto &payload =
          std::get<steamrot::SelectAndPlacePayload>(packet.payload);
      if (payload.action == steamrot::SelectAndPlacePayload::Action::PLACE_ITEM &&
          payload.item_name == "MyFrag" && payload.item_type == "fragment") {
        place_item_found = true;
        break;
      }
    }
  }
  REQUIRE(place_item_found);
}

TEST_CASE("SelectAndPlaceLogic does not fire PLACE_ITEM without a prior "
          "SELECT_ITEM",
          "[unit][SelectAndPlaceLogic]") {
  steamrot::tests::TestFixture fixture;
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;

  steamrot::logic::SelectAndPlaceLogic logic(fixture.GetSceneContext());

  // deliver only a USER_INPUT SELECT event (no prior selection)
  auto input_packet = steamrot::events::CreateInputEventPacket(
      1, steamrot::InputPayload::InputAction::SELECT);
  REQUIRE(input_packet.has_value());
  event_handler.AddEvent(input_packet.value());
  event_handler.ProcessWaitingRoomEventBus();
  event_handler.UpdateSubscribersFromGlobalEventBus();

  REQUIRE_NOTHROW(logic.RunLogic());

  // No PLACE_ITEM event should have been fired
  const auto &waiting_room = event_handler.GetWaitingRoomEventBus();
  for (const auto &packet : waiting_room) {
    if (packet.type == steamrot::EventType::SELECT_AND_PLACE) {
      if (std::holds_alternative<steamrot::SelectAndPlacePayload>(
              packet.payload)) {
        const auto &payload =
            std::get<steamrot::SelectAndPlacePayload>(packet.payload);
        REQUIRE(payload.action !=
                steamrot::SelectAndPlacePayload::Action::PLACE_ITEM);
      }
    }
  }
}
