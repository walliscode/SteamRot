/////////////////////////////////////////////////
/// @file
/// @brief Declaration of PartTraits and templates for concrete part types
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "Fragment.h"
#include "Joint.h"
#include "SocketState.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @class FragmentTraits
/// @brief struct containing type definitions for Fragment parts and sockets
/////////////////////////////////////////////////
struct FragmentTraits {

  using SocketType = FragmentSocketState;

  using PartType = Fragment;
};

/////////////////////////////////////////////////
/// @class JointTraits
/// @brief struct containing type definitions for Joint parts and sockets
/////////////////////////////////////////////////
struct JointTraits {

  using SocketType = JointSocketState;

  using PartType = Joint;
};

/////////////////////////////////////////////////
/// @brief Check if two part traits are compatible for interaction
///
/// The default implementation returns false, but specializations can be
/// provided for specific trait combinations that are compatible.
/// @tparam Trait The first part trait to check for compatibility
/// @tparam OtherTrait  The second part trait to check for compatibility
/////////////////////////////////////////////////
template <typename Trait, typename OtherTrait>
inline constexpr bool is_compatible_v = false;

/////////////////////////////////////////////////
/// @brief Specialization of is_compatible_v for FragmentTraits and JointTraits
/////////////////////////////////////////////////
template <>
inline constexpr bool is_compatible_v<FragmentTraits, JointTraits> = true;

/////////////////////////////////////////////////
/// @brief Specialization of is_compatible_v for JointTraits and FragmentTraits
/////////////////////////////////////////////////
template <>
inline constexpr bool is_compatible_v<JointTraits, FragmentTraits> = true;

/////////////////////////////////////////////////
/// @brief Define a concept to check if two part traits are compatible for
/// interaction
///
/// @tparam Trait The first part trait to check for compatibility
/// @tparam OtherTrait The second part trait to check for compatibility
/////////////////////////////////////////////////
template <typename Trait, typename OtherTrait>
concept CompatibleTraits = is_compatible_v<Trait, OtherTrait>;
} // namespace steamrot
