/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the action_select_and_place free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_select_and_place.h"
#include "EventHandler.h"
#include "EventPayload.h"
#include "EventType.h"
#include "SelectAndPlaceState.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SelectItem sets is_item_selected and stores item details",
          "[unit][action_select_and_place]") {
  steamrot::SelectAndPlaceState state;
  REQUIRE_FALSE(state.is_item_selected);

  steamrot::logic::action::select_and_place::SelectItem(state, "TestFragment",
                                                        "fragment");

  REQUIRE(state.is_item_selected);
  REQUIRE(state.selected_item_name == "TestFragment");
  REQUIRE(state.selected_item_type == "fragment");
}

TEST_CASE("SelectItem overwrites previous selection",
          "[unit][action_select_and_place]") {
  steamrot::SelectAndPlaceState state;
  steamrot::logic::action::select_and_place::SelectItem(state, "First",
                                                        "fragment");
  steamrot::logic::action::select_and_place::SelectItem(state, "Second",
                                                        "joint");

  REQUIRE(state.is_item_selected);
  REQUIRE(state.selected_item_name == "Second");
  REQUIRE(state.selected_item_type == "joint");
}

TEST_CASE("ClearSelection resets all state fields",
          "[unit][action_select_and_place]") {
  steamrot::SelectAndPlaceState state;
  steamrot::logic::action::select_and_place::SelectItem(state, "TestFragment",
                                                        "fragment");
  REQUIRE(state.is_item_selected);

  steamrot::logic::action::select_and_place::ClearSelection(state);

  REQUIRE_FALSE(state.is_item_selected);
  REQUIRE(state.selected_item_name.empty());
  REQUIRE(state.selected_item_type.empty());
}

TEST_CASE("PlaceItem fires a PLACE_ITEM event when an item is selected",
          "[unit][action_select_and_place]") {
  steamrot::tests::TestFixture fixture;
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;

  steamrot::SelectAndPlaceState state;
  steamrot::logic::action::select_and_place::SelectItem(state, "MyFragment",
                                                        "fragment");

  REQUIRE(event_handler.GetWaitingRoomEventBus().size() == 0);

  steamrot::logic::action::select_and_place::PlaceItem(state, event_handler);

  REQUIRE(event_handler.GetWaitingRoomEventBus().size() == 1);

  const auto &packet = event_handler.GetWaitingRoomEventBus().front();
  REQUIRE(packet.type == steamrot::EventType::SELECT_AND_PLACE);
  REQUIRE(std::holds_alternative<steamrot::SelectAndPlacePayload>(
      packet.payload));

  const auto &payload =
      std::get<steamrot::SelectAndPlacePayload>(packet.payload);
  REQUIRE(payload.action == steamrot::SelectAndPlacePayload::Action::PLACE_ITEM);
  REQUIRE(payload.item_name == "MyFragment");
  REQUIRE(payload.item_type == "fragment");
}

TEST_CASE("PlaceItem does nothing when no item is selected",
          "[unit][action_select_and_place]") {
  steamrot::tests::TestFixture fixture;
  steamrot::EventHandler &event_handler =
      fixture.GetGameContext().event_handler;

  steamrot::SelectAndPlaceState state;
  REQUIRE_FALSE(state.is_item_selected);

  steamrot::logic::action::select_and_place::PlaceItem(state, event_handler);

  REQUIRE(event_handler.GetWaitingRoomEventBus().size() == 0);
}
