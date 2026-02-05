/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions to configure grimoire data structures (Fragment and Joint)
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "FailInfo.h"
#include "Fragment.h"
#include "Joint.h"
#include "fragment_generated.h"
#include "joint_generated.h"
#include <expected>
#include <variant>
namespace steamrot::data::configure {

/////////////////////////////////////////////////
/// @brief Configure Fragment from flatbuffers data source.
///
/// @param fragment Fragment to configure.
/// @param fragment_fbs Flatbuffers data source.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureFragment(Fragment &fragment, const FragmentFbs *fragment_fbs);

/////////////////////////////////////////////////
/// @brief Configure Joint from flatbuffers data source.
///
/// @param joint Joint to configure.
/// @param joint_fbs Flatbuffers data source.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureJoint(Joint &joint, const JointFbs *joint_fbs);

} // namespace steamrot::data::configure
