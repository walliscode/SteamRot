/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for matching payloads of events.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "payload_matchers.h"
#include <variant>

namespace steamrot::events {
/////////////////////////////////////////////////
bool MatchPayload(const InputPayload &filter_payload,
                  const InputPayload &event_payload) {

  if (filter_payload.action != event_payload.action) {
    return false;
  }
  return true;
}

/////////////////////////////////////////////////
bool MatchPayload(const UIPayload &filter_payload,
                  const UIPayload &event_payload) {

  if (filter_payload.action != event_payload.action) {
    return false;
  }

  if (filter_payload.c_ui_state_name != event_payload.c_ui_state_name) {
    return false;
  }

  return true;
}

/////////////////////////////////////////////////
bool MatchPayload(const LogicPayload &filter_payload,
                  const LogicPayload &event_payload) {

  if (filter_payload.toggle_name != event_payload.toggle_name) {
    return false;
  }

  return true;
}

/////////////////////////////////////////////////
bool MatchPayload(const ScenePayload &filter_payload,
                  const ScenePayload &event_payload) {

  if (filter_payload.action != event_payload.action) {
    return false;
  }

  // Currently only Scene actions are needed for checking, the rest is just
  // specific data

  // if (filter_payload.scene_type != event_payload.scene_type) {
  //   return false;
  // }
  //
  // // Check scene_id if both have values
  // if (filter_payload.scene_id.has_value() &&
  //     event_payload.scene_id.has_value()) {
  //   if (filter_payload.scene_id.value() != event_payload.scene_id.value()) {
  //     return false;
  //   }
  // } else if (filter_payload.scene_id.has_value() !=
  //            event_payload.scene_id.has_value()) {
  //   // If one has value and other doesn't, they don't match
  //   return false;
  // }

  return true;
}

/////////////////////////////////////////////////
bool MatchPayload(const SystemPayload &filter_payload,
                  const SystemPayload &event_payload) {

  if (filter_payload.action != event_payload.action) {
    return false;
  }

  return true;
}

/////////////////////////////////////////////////
bool MatchPayload(const GhostPayload &filter_payload,
                  const GhostPayload &event_payload) {

  if (filter_payload.action != event_payload.action) {
    return false;
  }

  // A monostate filter selection acts as a wildcard: it matches any selection
  // type carried by the event. This allows subscribers to listen for all
  // SELECT (or CLEAR) events without specifying a particular selection key.
  if (std::holds_alternative<std::monostate>(filter_payload.m_selection)) {
    return true;
  }

  // Compare the selection variant (type + key must both match)
  if (filter_payload.m_selection.index() != event_payload.m_selection.index()) {
    return false;
  }

  return std::visit(
      [](const auto &filter_sel, const auto &event_sel) -> bool {
        if constexpr (std::is_same_v<decltype(filter_sel), decltype(event_sel)>) {
          if constexpr (std::is_same_v<std::decay_t<decltype(filter_sel)>,
                                       std::monostate>) {
            return true;
          } else {
            return filter_sel.key == event_sel.key;
          }
        }
        return false;
      },
      filter_payload.m_selection, event_payload.m_selection);
}

/////////////////////////////////////////////////
bool MatchPayload(const CameraPayload & /*filter_payload*/,
                  const CameraPayload & /*event_payload*/) {
  // Any camera scroll event matches — the subscriber reads the delta
  // from captured_payload rather than filtering on a specific value.
  return true;
}

/////////////////////////////////////////////////
bool MatchPayload(const EventPayload &filter_payload,
                  const EventPayload &event_payload) {

  return std::visit(
      // Visitor called with the concrete values stored in the variants.
      [](const auto &filter, const auto &event) -> bool {
        // Compile-time type check:
        // decltype(filter) / decltype(event) will be "const T&" for some T.
        // When both variants hold the same alternative T, these types match.
        if constexpr (std::is_same_v<decltype(filter), decltype(event)>) {

          // Types match, so delegate to the type-specific overload:
          //   bool MatchPayload(const T&, const T&);
          // If no such overload exists for a variant alternative T, you'll get
          // a compile-time error when this visitor is instantiated for (T, T).
          return MatchPayload(filter, event);
        } else {

          // Types differ (e.g., filter holds A, event holds B). We consider
          // that a non-match and *do not* attempt to call MatchPayload(A, B).
          return false;
        }
      },
      filter_payload, event_payload);
}
} // namespace steamrot::events
