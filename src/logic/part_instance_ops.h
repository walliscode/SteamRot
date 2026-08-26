/////////////////////////////////////////////////
/// @file
/// @brief free functions for working with PartInstance variants
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "PartInstance.h"
#include "PartTraits.h"
#include <expected>
#include <variant>

namespace steamrot::logic {

/////////////////////////////////////////////////
/// @brief a function to align two PartInstances of potentially different types,
/// if their traits are compatible. handles incompatible types at runtime
///
/// @tparam LeftTrait [TODO:tparam]
/// @tparam RightTrait [TODO:tparam]
/// @param instance_a [TODO:parameter]
/// @param socket_id_a [TODO:parameter]
/// @param instance_b [TODO:parameter]
/// @param socket_id_b [TODO:parameter]
/// @return [TODO:return]
/////////////////////////////////////////////////
template <typename LeftTrait, typename RightTrait>
std::expected<std::monostate, FailInfo> try_align_part_instances(
    PartInstance<LeftTrait> &instance_a, uint32_t socket_id_a,
    const PartInstance<RightTrait> &instance_b, uint32_t socket_id_b) {

  if constexpr (CompatibleTraits<LeftTrait, RightTrait>) {
    return instance_a.AlignOntoOtherPartInstance(socket_id_a, instance_b,
                                                 socket_id_b);
  } else {
    return std::unexpected(
        FailInfo{FailMode::BadValue,
                 std::format("Traits are not compatible for alignment")});
  }
}
} // namespace steamrot::logic
