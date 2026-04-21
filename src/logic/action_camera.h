/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for processing camera actions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CameraState.h"
#include "EventPayload.h"
#include "Subscriber.h"
#include <memory>
#include <vector>

namespace steamrot::logic::action::camera {

/////////////////////////////////////////////////
/// @brief Apply a mouse-scroll delta to the camera zoom level.
///
/// Positive delta (scroll up) zooms in; negative (scroll down) zooms out.
/// The result is clamped to [CameraState::kMinZoom, CameraState::kMaxZoom].
///
/// @param camera_state Camera state whose zoom level will be mutated.
/// @param scroll_delta Raw scroll delta from the SFML mouse-wheel event.
/////////////////////////////////////////////////
void apply_zoom(CameraState &camera_state, float scroll_delta);

/////////////////////////////////////////////////
/// @brief Reset the camera zoom level to the default value.
///
/// @param camera_state Camera state whose zoom level will be reset to the
/// default value.
/////////////////////////////////////////////////
void reset_zoom(CameraState &camera_state);

/////////////////////////////////////////////////
/// @brief Set the pan flag for the given direction (key-press behaviour).
///
/// @param camera_state  Camera state whose panning flags will be mutated.
/// @param direction     The pan direction to activate.
/////////////////////////////////////////////////
void apply_pan_start(CameraState &camera_state,
                     CameraPayload::PanDirection direction);

/////////////////////////////////////////////////
/// @brief Clear the pan flag for the given direction (key-release behaviour).
///
/// @param camera_state  Camera state whose panning flags will be mutated.
/// @param direction     The pan direction to deactivate.
/////////////////////////////////////////////////
void apply_pan_stop(CameraState &camera_state,
                    CameraPayload::PanDirection direction);

/////////////////////////////////////////////////
/// @brief Process all active subscribers and apply their camera scroll deltas.
///
/// Iterates the subscriber list, skips inactive entries, and for each active
/// subscriber extracts a CameraPayload and applies its scroll_delta to
/// @p camera_state via positioning::camera::ApplyZoom.
///
/// @param subscribers  Subscribers owned by the Logic instance.
/// @param camera_state CameraState instance to mutate.
/////////////////////////////////////////////////
void process_subscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    CameraState &camera_state);

} // namespace steamrot::logic::action::camera
