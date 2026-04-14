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
#include "Subscriber.h"
#include <memory>
#include <vector>

namespace steamrot::logic::action::camera {

/////////////////////////////////////////////////
/// @brief Process all active subscribers and apply their camera scroll deltas.
///
/// Iterates the subscriber list, skips inactive entries, and for each active
/// subscriber extracts a CameraPayload and applies its scroll_delta to
/// @p camera_state via CameraState::ApplyScrollDelta.
///
/// @param subscribers  Subscribers owned by the Logic instance.
/// @param camera_state CameraState instance to mutate.
/////////////////////////////////////////////////
void ProcessSubscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    CameraState &camera_state);

} // namespace steamrot::logic::action::camera
