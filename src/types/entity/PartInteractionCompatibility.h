////////////////////////////////////////////////////////////
/// @file
/// @brief Compile-time compatibility rules for part interactions.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Preprocessor Directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include <type_traits>

namespace steamrot {

class FragmentInstance;
class JointInstance;

////////////////////////////////////////////////////////////
/// @brief Tag type for connection compatibility rules.
////////////////////////////////////////////////////////////
struct ConnectInteraction {};

////////////////////////////////////////////////////////////
/// @brief Tag type for collision compatibility rules.
////////////////////////////////////////////////////////////
struct CollisionInteraction {};

////////////////////////////////////////////////////////////
/// @brief Tag type for alignment compatibility rules.
////////////////////////////////////////////////////////////
struct AlignmentInteraction {};

////////////////////////////////////////////////////////////
/// @brief Primary compatibility trait. Interactions are forbidden by default.
///
/// @tparam Interaction Interaction tag type.
/// @tparam A Left-hand part instance type.
/// @tparam B Right-hand part instance type.
////////////////////////////////////////////////////////////
template <typename Interaction, typename A, typename B>
struct IsCompatible : std::false_type {};

template <>
struct IsCompatible<ConnectInteraction, FragmentInstance, JointInstance>
    : std::true_type {};

template <>
struct IsCompatible<ConnectInteraction, JointInstance, FragmentInstance>
    : std::true_type {};

template <>
struct IsCompatible<CollisionInteraction, FragmentInstance, JointInstance>
    : std::true_type {};

template <>
struct IsCompatible<CollisionInteraction, JointInstance, FragmentInstance>
    : std::true_type {};

template <>
struct IsCompatible<AlignmentInteraction, FragmentInstance, JointInstance>
    : std::true_type {};

template <>
struct IsCompatible<AlignmentInteraction, JointInstance, FragmentInstance>
    : std::true_type {};

} // namespace steamrot
