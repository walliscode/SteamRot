/////////////////////////////////////////////////
/// @file
/// @brief Declaration of objects for the part graph library.
/// This provides predefined PartGraphPackages for testing and analysis.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "PartGraphBuilder.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Accessor for a simple pair of a joint and a fragment.
///
/// Uses function-local static initialization to avoid global dynamic-init
/// side effects during Catch2 test discovery.
/////////////////////////////////////////////////
const PartGraphPackage &pair();

/////////////////////////////////////////////////
/// @brief Accessor for a linear chain of three parts:
///        fragment ── joint ── fragment.
///
/// Uses function-local static initialization to avoid global dynamic-init
/// side effects during Catch2 test discovery.
/////////////////////////////////////////////////
const PartGraphPackage &linear_chain_3();

/////////////////////////////////////////////////
/// @brief Accessor for a linear chain of five parts:
///        fragment ── joint ── fragment ── joint ── fragment.
///
/// Uses function-local static initialization to avoid global dynamic-init
/// side effects during Catch2 test discovery.
/////////////////////////////////////////////////
const PartGraphPackage &linear_chain_5();

/////////////////////////////////////////////////
/// @brief Accessor for three joints wired in a cycle:
///        joint0.socket[0] ↔ joint1.socket[0]
///        joint1.socket[1] ↔ joint2.socket[0]
///        joint2.socket[1] ↔ joint0.socket[1]
///
/// Uses function-local static initialization to avoid global dynamic-init
/// side effects during Catch2 test discovery.
/////////////////////////////////////////////////
const PartGraphPackage &ring();

} // namespace steamrot::tests
