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
#include "Vector2fEqualsMatcher.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

namespace steamrot::tests {

using namespace steamrot::logic::action::camera;

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

/////////////////////////////////////////////////
// ApplyPanStart / ApplyPanStop
/////////////////////////////////////////////////

TEST_CASE("action_camera::apply_pan_start: UP sets m_panning_up",
          "[unit][action_camera]") {
  steamrot::CameraState camera_state;
  steamrot::logic::action::camera::apply_pan_start(
      camera_state, steamrot::CameraPayload::PanDirection::UP);
  REQUIRE(camera_state.m_panning_up == true);
  REQUIRE(camera_state.m_panning_down == false);
  REQUIRE(camera_state.m_panning_left == false);
  REQUIRE(camera_state.m_panning_right == false);
}

TEST_CASE("action_camera::apply_pan_start: DOWN sets m_panning_down",
          "[unit][action_camera]") {
  steamrot::CameraState camera_state;
  steamrot::logic::action::camera::apply_pan_start(
      camera_state, steamrot::CameraPayload::PanDirection::DOWN);
  REQUIRE(camera_state.m_panning_down == true);
}

TEST_CASE("action_camera::apply_pan_start: LEFT sets m_panning_left",
          "[unit][action_camera]") {
  steamrot::CameraState camera_state;
  steamrot::logic::action::camera::apply_pan_start(
      camera_state, steamrot::CameraPayload::PanDirection::LEFT);
  REQUIRE(camera_state.m_panning_left == true);
}

TEST_CASE("action_camera::apply_pan_start: RIGHT sets m_panning_right",
          "[unit][action_camera]") {
  steamrot::CameraState camera_state;
  steamrot::logic::action::camera::apply_pan_start(
      camera_state, steamrot::CameraPayload::PanDirection::RIGHT);
  REQUIRE(camera_state.m_panning_right == true);
}

TEST_CASE("action_camera::apply_pan_start: NONE changes no flags",
          "[unit][action_camera]") {
  steamrot::CameraState camera_state;
  steamrot::logic::action::camera::apply_pan_start(
      camera_state, steamrot::CameraPayload::PanDirection::NONE);
  REQUIRE(camera_state.m_panning_up == false);
  REQUIRE(camera_state.m_panning_down == false);
  REQUIRE(camera_state.m_panning_left == false);
  REQUIRE(camera_state.m_panning_right == false);
}

TEST_CASE("action_camera::apply_pan_stop: UP clears m_panning_up",
          "[unit][action_camera]") {
  steamrot::CameraState camera_state;
  camera_state.m_panning_up = true;
  steamrot::logic::action::camera::apply_pan_stop(
      camera_state, steamrot::CameraPayload::PanDirection::UP);
  REQUIRE(camera_state.m_panning_up == false);
}

TEST_CASE("action_camera::apply_pan_stop: DOWN clears m_panning_down",
          "[unit][action_camera]") {
  steamrot::CameraState camera_state;
  camera_state.m_panning_down = true;
  steamrot::logic::action::camera::apply_pan_stop(
      camera_state, steamrot::CameraPayload::PanDirection::DOWN);
  REQUIRE(camera_state.m_panning_down == false);
}

TEST_CASE("action_camera::apply_pan_stop: LEFT clears m_panning_left",
          "[unit][action_camera]") {
  steamrot::CameraState camera_state;
  camera_state.m_panning_left = true;
  steamrot::logic::action::camera::apply_pan_stop(
      camera_state, steamrot::CameraPayload::PanDirection::LEFT);
  REQUIRE(camera_state.m_panning_left == false);
}

TEST_CASE("action_camera::apply_pan_stop: RIGHT clears m_panning_right",
          "[unit][action_camera]") {
  steamrot::CameraState camera_state;
  camera_state.m_panning_right = true;
  steamrot::logic::action::camera::apply_pan_stop(
      camera_state, steamrot::CameraPayload::PanDirection::RIGHT);
  REQUIRE(camera_state.m_panning_right == false);
}

/////////////////////////////////////////////////
// ProcessSubscribers — panning
/////////////////////////////////////////////////

TEST_CASE(
    "action_camera::ProcessSubscribers: PAN_PRESS UP activates panning_up",
    "[unit][action_camera]") {
  steamrot::CameraState camera_state;

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->captured_payload =
      steamrot::CameraPayload{steamrot::CameraPayload::CameraAction::PAN_PRESS,
                              steamrot::CameraPayload::PanDirection::UP};

  steamrot::logic::action::camera::process_subscribers({subscriber},
                                                       camera_state);

  REQUIRE(camera_state.m_panning_up == true);
}

TEST_CASE(
    "action_camera::ProcessSubscribers: PAN_RELEASE UP deactivates panning_up",
    "[unit][action_camera]") {
  steamrot::CameraState camera_state;
  camera_state.m_panning_up = true;

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->captured_payload = steamrot::CameraPayload{
      steamrot::CameraPayload::CameraAction::PAN_RELEASE,
      steamrot::CameraPayload::PanDirection::UP};

  steamrot::logic::action::camera::process_subscribers({subscriber},
                                                       camera_state);

  REQUIRE(camera_state.m_panning_up == false);
}

TEST_CASE("action_camera::ProcessSubscribers: PAN_PRESS RIGHT activates "
          "panning_right",
          "[unit][action_camera]") {
  steamrot::CameraState camera_state;

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = true;
  subscriber->captured_payload =
      steamrot::CameraPayload{steamrot::CameraPayload::CameraAction::PAN_PRESS,
                              steamrot::CameraPayload::PanDirection::RIGHT};

  steamrot::logic::action::camera::process_subscribers({subscriber},
                                                       camera_state);

  REQUIRE(camera_state.m_panning_right == true);
}

TEST_CASE("action_camera::ProcessSubscribers: inactive PAN_PRESS subscriber is "
          "skipped",
          "[unit][action_camera]") {
  steamrot::CameraState camera_state;

  auto subscriber = std::make_shared<steamrot::Subscriber>();
  subscriber->m_active = false;
  subscriber->captured_payload =
      steamrot::CameraPayload{steamrot::CameraPayload::CameraAction::PAN_PRESS,
                              steamrot::CameraPayload::PanDirection::UP};

  steamrot::logic::action::camera::process_subscribers({subscriber},
                                                       camera_state);

  REQUIRE(camera_state.m_panning_up == false);
}

TEST_CASE(
    "action_camera::ProcessSubscribers: multiple active subscribers each apply "
    "their scroll delta",
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

TEST_CASE("align_camera_to_machina_form_scaffold tests") {
  // arrange
  CameraState camera_state;
  MachinaFormScaffold scaffold;
  REQUIRE_THAT(camera_state.m_position, EqualsVector2f({0.f, 0.f}));

  Fragment fragment_one;
  sf::VertexArray &front_array =
      fragment_one.positioning_views[ViewDirection::Front];
  front_array.setPrimitiveType(sf::PrimitiveType::Triangles);
  front_array.append(sf::Vertex(sf::Vector2f(0.f, 0.f)));
  front_array.append(sf::Vertex(sf::Vector2f(30.f, 0.f)));
  front_array.append(sf::Vertex(sf::Vector2f(15.f, 30.f)));

  SECTION("when part graph is empty, camera position alignts to 0,0 with zoom "
          "1.0") {
    // arrange
    camera_state.m_position = sf::Vector2f(100.f, 100.f);
    camera_state.m_zoom_level = 2.0f;

    // act
    align_camera_to_machina_form_scaffold(scaffold, camera_state);

    // assert
    REQUIRE_THAT(camera_state.m_position, EqualsVector2f({0.f, 0.f}));
    REQUIRE(camera_state.m_zoom_level == 1.0f);
  }

  SECTION("when part graph is provided, aligns camera to the centre of the "
          "bounding box of the part graph") {
    // arrange
    camera_state.m_zoom_level = 3.0f;
    // Create a simple part graph with one part at position (50, 50)
    FragmentInstance fragment_instance{&fragment_one};
    fragment_instance.transform.translate({50.f, 50.f});
    scaffold.parts.emplace(fragment_instance.id, fragment_instance);

    // act
    align_camera_to_machina_form_scaffold(scaffold, camera_state);
    // assert
    REQUIRE_THAT(camera_state.m_position, EqualsVector2f({65.f, 65.f}));
    REQUIRE(camera_state.m_zoom_level == 1.0f);
  }
}

} // namespace steamrot::tests
