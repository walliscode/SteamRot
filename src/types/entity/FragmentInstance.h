/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the FragmentInstance class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Fragment.h"
#include "PartInstance.h"
#include "SocketState.h"

namespace steamrot {

struct FragmentTraits {
  using SocketType = FragmentSocketState;

  using PartType = Fragment;
};

class FragmentInstance : public PartInstance<FragmentTraits> {

public:
  FragmentInstance(const uint32_t id, const Fragment &fragment)
      : PartInstance<FragmentTraits>(id, fragment) {}

  FragmentInstance(const uint32_t id, const Fragment &fragment,
                   const std::string &alias)
      : PartInstance<FragmentTraits>(id, alias, fragment) {}
};
} // namespace steamrot
