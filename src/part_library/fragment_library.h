/////////////////////////////////////////////////
/// @file
/// @brief Free functions for creating Fragment objects with specific
/// configurations and properties.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "Fragment.h"
#include "FragmentBuilder.h"
namespace steamrot::parts {

inline Fragment CreateRectangleFragmentWith2Sockets(const std::string &name) {
  return FragmentBuilder()
      .AddViews(PremadeViews::WhiteOblong)
      .AddSocket({0, 5}, {1, 0})
      .AddSocket({50, 5}, {-1, 0})
      .Build(name);
}

} // namespace steamrot::parts
