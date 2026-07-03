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

/////////////////////////////////////////////////
inline const Fragment FragmentRectangleWithOneSocket = [] {
  return FragmentBuilder()
      .AddViews(PremadeViews::WhiteOblong)
      .AddSocket({0, 5}, {1, 0})
      .Build("FragmentRectangleWithOneSocket");
}();

/////////////////////////////////////////////////
inline const Fragment FragmentRectangleWithTwoSockets = [] {
  return FragmentBuilder()
      .AddViews(PremadeViews::WhiteOblong)
      .AddSocket({0, 5}, {1, 0})
      .AddSocket({50, 5}, {-1, 0})
      .Build("FragmentRectangleWithTwoSockets");
}();

/////////////////////////////////////////////////
inline const Fragment FragmentRectangleWithThreeSockets = [] {
  return FragmentBuilder()
      .AddViews(PremadeViews::WhiteOblong)
      .AddSocket({0, 5}, {1, 0})
      .AddSocket({50, 5}, {-1, 0})
      .AddSocket({25, 0}, {0, -1})
      .Build("FragmentRectangleWithThreeSockets");
}();

} // namespace steamrot::parts
