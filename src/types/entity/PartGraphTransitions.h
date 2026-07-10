/////////////////////////////////////////////////
/// @file
/// @brief Declaration of structs for storing transform state of PartInstances
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <SFML/Graphics/Transform.hpp>
#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Type alias for a single frame of PartInstance transforms
///
/// Maps PartInstance ID -> transform for one sampled frame.
/////////////////////////////////////////////////
using PartGraphFrame = std::unordered_map<uint32_t, sf::Transform>;

/////////////////////////////////////////////////
/// @brief Ordered sequence of frames used for animation/transition playback
/////////////////////////////////////////////////
using PartGraphSequence = std::vector<PartGraphFrame>;

/////////////////////////////////////////////////
/// @brief Concept constraining template parameters to enum types
/////////////////////////////////////////////////
template <typename T>
concept EnumType = std::is_enum_v<T>;

/////////////////////////////////////////////////
/// @brief Struct representing a key for a transition between two states of a
/// PartGraph
///
/// @tparam EnumType The enum type representing the states of the PartGraph
/////////////////////////////////////////////////
template <typename EnumType> struct TransitionStateKey {
  /////////////////////////////////////////////////
  /// @brief The state the PartGraph is transitioning from
  /////////////////////////////////////////////////
  const EnumType from_state;

  /////////////////////////////////////////////////
  /// @brief The state the PartGraph is transitioning to
  /////////////////////////////////////////////////
  const EnumType to_state;

  /////////////////////////////////////////////////
  /// @brief Overloaded equality operator for TransitionStateKey
  ///
  /// @param other The other TransitionStateKey to compare against
  /// @return True if the two TransitionStateKeys are equal, false otherwise
  /////////////////////////////////////////////////
  bool operator==(const TransitionStateKey &other) const {
    return from_state == other.from_state && to_state == other.to_state;
  }
};

/////////////////////////////////////////////////
/// @brief Hash function for TransitionStateKey, allowing it to be used as a key
/// in unordered_map
///
/// @tparam EnumType The enum type representing the states of the PartGraph
/////////////////////////////////////////////////
template <typename EnumType> struct TransitionStateKeyHash {
  /////////////////////////////////////////////////
  /// @brief Overloaded function call operator for hashing a TransitionStateKey
  ///
  /// @param key The TransitionStateKey to hash
  /// @return The hash value of the TransitionStateKey
  /////////////////////////////////////////////////
  std::size_t operator()(const TransitionStateKey<EnumType> &key) const {
    return std::hash<std::underlying_type_t<EnumType>>{}(
               static_cast<std::underlying_type_t<EnumType>>(key.from_state)) ^
           (std::hash<std::underlying_type_t<EnumType>>{}(
                static_cast<std::underlying_type_t<EnumType>>(key.to_state))
            << 1);
  }
};

/////////////////////////////////////////////////
/// @brief A map of TransitionStateKeys to transition frame sequences
///
/// Efficient lookup of animation/pose data for a specific from->to state
/// transition.
///
/// @tparam EnumType The enum type representing the states of the PartGraph
/////////////////////////////////////////////////
template <typename EnumType>
using PartGraphTransitionStateMap =
    std::unordered_map<TransitionStateKey<EnumType>, PartGraphSequence,
                       TransitionStateKeyHash<EnumType>>;

/////////////////////////////////////////////////
/// @brief A map of steady (non-transitioning) states to a single frame
///
/// @tparam EnumType The enum type representing the states of the PartGraph
/////////////////////////////////////////////////
template <typename EnumType>
using PartGraphSteadyStateMap = std::unordered_map<EnumType, PartGraphFrame>;

/////////////////////////////////////////////////
/// @brief Runtime state tracker for a PartGraph FSM data store
///
/// Tracks whether the graph is currently steady or transitioning, the currently
/// active steady state, and (when transitioning) iterators into the active
/// transition sequence.
/////////////////////////////////////////////////
template <typename EnumType> struct TransitionState {

  /////////////////////////////////////////////////
  /// @brief Runtime mode of the state machine data
  /////////////////////////////////////////////////
  enum class Mode { STEADY, TRANSITIONING };

  /////////////////////////////////////////////////
  /// @brief Current runtime mode (defaults to steady)
  /////////////////////////////////////////////////
  Mode mode{Mode::STEADY};

  /////////////////////////////////////////////////
  /// @brief Current steady/static state
  ///
  /// When mode is STEADY, this is the active state.
  /// When mode is TRANSITIONING, this can still represent the logical
  /// "current base state" depending on your update policy.
  /////////////////////////////////////////////////
  EnumType current_static_state;

  /////////////////////////////////////////////////
  /// @brief Construct a TransitionState with an initial steady state
  ///
  /// @param initial_state Initial static/steady state
  /////////////////////////////////////////////////
  TransitionState(EnumType initial_state)
      : current_static_state(initial_state) {};

private:
  /////////////////////////////////////////////////
  /// @brief Iterator to the current frame in the active transition sequence
  ///
  /// Set only while mode == TRANSITIONING.
  /////////////////////////////////////////////////
  std::optional<PartGraphSequence::const_iterator> sequence_iterator;

  /////////////////////////////////////////////////
  /// @brief End iterator for the active transition sequence
  ///
  /// Used with sequence_iterator to detect transition completion.
  /////////////////////////////////////////////////
  std::optional<PartGraphSequence::const_iterator> end_iterator;
};

/////////////////////////////////////////////////
/// @brief Container for all FSM transform data for a PartGraph
///
/// Holds:
/// - transition data (from->to => sequence)
/// - steady-state data (state => frame)
/////////////////////////////////////////////////
template <typename EnumType> struct PartGraphTransforms {

  /////////////////////////////////////////////////
  /// @brief Transition frame sequences keyed by from->to states
  /////////////////////////////////////////////////
  PartGraphTransitionStateMap<EnumType> transitions;

  /////////////////////////////////////////////////
  /// @brief Steady-state frame data keyed by state
  /////////////////////////////////////////////////
  PartGraphSteadyStateMap<EnumType> steady_states;
};

} // namespace steamrot
