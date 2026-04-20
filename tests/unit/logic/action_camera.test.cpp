/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the action_camera free functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_camera.h"
#include "CameraState.h"
#include "EventPayload.h"
#include "Subscriber.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

TEST_CASE("reset_zoom() tests", "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;

  SECTION("reset_zoom() sets zoom level to default value") {
    // Change zoom level from default
    camera_state.m_zoom_level = 2.0f;
    steamrot::logic::action::camera::reset_zoom(camera_state);
    REQUIRE(camera_state.m_zoom_level == 1.0f);
  }
}
/////////////////////////////////////////////////
// ApplyZoom
/////////////////////////////////////////////////

TEST_CASE("positioning_camera::ApplyZoom: positive delta decreases zoom level",
          "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  const float initial_zoom = camera_state.m_zoom_level;

  steamrot::logic::action::camera::apply_zoom(camera_state, 1.0f);

  REQUIRE(camera_state.m_zoom_level < initial_zoom);
}

TEST_CASE("positioning_camera::ApplyZoom: negative delta increases zoom level",
          "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  const float initial_zoom = camera_state.m_zoom_level;

  steamrot::logic::action::camera::apply_zoom(camera_state, -1.0f);

  REQUIRE(camera_state.m_zoom_level > initial_zoom);
}

TEST_CASE(
    "positioning_camera::ApplyZoom: zero delta leaves zoom level unchanged",
    "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;
  const float initial_zoom = camera_state.m_zoom_level;

  steamrot::logic::action::camera::apply_zoom(camera_state, 0.0f);

  REQUIRE(camera_state.m_zoom_level == initial_zoom);
}

TEST_CASE("positioning_camera::ApplyZoom: clamps zoom to kMinZoom",
          "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;

  // Large positive delta should zoom in past the minimum
  steamrot::logic::action::camera::apply_zoom(camera_state, 1000.0f);

  REQUIRE(camera_state.m_zoom_level == steamrot::CameraState::kMinZoom);
}

TEST_CASE("positioning_camera::ApplyZoom: clamps zoom to kMaxZoom",
          "[unit][positioning_camera]") {
  steamrot::CameraState camera_state;

  // Large negative delta should zoom out past the maximum
  steamrot::logic::action::camera::apply_zoom(camera_state, -1000.0f);

  REQUIRE(camera_state.m_zoom_level == steamrot::CameraState::kMaxZoom);
}
/////////////////////////////////////////////////
// ProcessSubscribers
/////////////////////////////////////////////////

TEST_CASE("action_camera::ProcessSubscribers: inactive subscriber is skipped",
          "[unit][action_camera]") {
  steamrot::CameraState camera_state;
  const float initial_zoom = camera_state.m_zoom_level;

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = false;
  subscriber->captured_payload = steamrot::CameraPayload{1.0f};

  steamrot::logic::action::camera::process_subscribers({subscriber},
                                                       camera_state);

  REQUIRE(camera_state.m_zoom_level == initial_zoom);
}

TEST_CASE(
    "action_camera::ProcessSubscribers: active subscriber with no payload is "
    "skipped",
    "[unit][action_camera]") {
  steamrot::CameraState camera_state;
  const float initial_zoom = camera_state.m_zoom_level;

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  // captured_payload left as std::nullopt

  steamrot::logic::action::camera::process_subscribers({subscriber},
                                                       camera_state);

  REQUIRE(camera_state.m_zoom_level == initial_zoom);
}

TEST_CASE(
    "action_camera::ProcessSubscribers: active subscriber with non-camera "
    "payload is skipped",
    "[unit][action_camera]") {
  steamrot::CameraState camera_state;
  const float initial_zoom = camera_state.m_zoom_level;

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->captured_payload = std::monostate{};

  steamrot::logic::action::camera::process_subscribers({subscriber},
                                                       camera_state);

  REQUIRE(camera_state.m_zoom_level == initial_zoom);
}

TEST_CASE(
    "action_camera::ProcessSubscribers: active subscriber applies scroll delta",
    "[unit][action_camera]") {
  steamrot::CameraState camera_state;

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->captured_payload = steamrot::CameraPayload{1.0f};

  steamrot::logic::action::camera::process_subscribers({subscriber},
                                                       camera_state);

  // zoom level should have changed from 1.0
  REQUIRE(camera_state.m_zoom_level != 1.0f);
}

TEST_CASE("action_camera::ProcessSubscribers: multiple active subscribers each "
          "apply their scroll delta",
          "[unit][action_camera]") {
  steamrot::CameraState camera_state;

  auto subscriber_a = std::make_shared<steamrot::Subscriber>();
  subscriber_a->m_active = true;
  subscriber_a->captured_payload = steamrot::CameraPayload{1.0f};

  auto subscriber_b = std::make_shared<steamrot::Subscriber>();
  subscriber_b->m_active = true;
  subscriber_b->captured_payload = steamrot::CameraPayload{1.0f};

  // Apply just subscriber_a to get an intermediate reference
  steamrot::CameraState intermediate_state;
  steamrot::logic::action::camera::process_subscribers({subscriber_a},
                                                       intermediate_state);

  // Apply both subscribers — zoom should differ from single application
  steamrot::logic::action::camera::process_subscribers(
      {subscriber_a, subscriber_b}, camera_state);

  REQUIRE(camera_state.m_zoom_level != intermediate_state.m_zoom_level);
}
