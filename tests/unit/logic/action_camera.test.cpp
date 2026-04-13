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

  steamrot::logic::action::camera::ProcessSubscribers({subscriber},
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

  steamrot::logic::action::camera::ProcessSubscribers({subscriber},
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

  steamrot::logic::action::camera::ProcessSubscribers({subscriber},
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

  steamrot::logic::action::camera::ProcessSubscribers({subscriber},
                                                      camera_state);

  // zoom level should have changed from 1.0
  REQUIRE(camera_state.m_zoom_level != 1.0f);
}

TEST_CASE(
    "action_camera::ProcessSubscribers: multiple active subscribers each "
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
  steamrot::logic::action::camera::ProcessSubscribers({subscriber_a},
                                                      intermediate_state);

  // Apply both subscribers — zoom should differ from single application
  steamrot::logic::action::camera::ProcessSubscribers(
      {subscriber_a, subscriber_b}, camera_state);

  REQUIRE(camera_state.m_zoom_level != intermediate_state.m_zoom_level);
}
