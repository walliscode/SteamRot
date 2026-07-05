/////////////////////////////////////////////////
/// @file
/// @brief Free functions for creating Joint objects with specific
/// configurations and properties.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "Joint.h"
#include "JointBuilder.h"
namespace steamrot::parts {

/////////////////////////////////////////////////
inline const Joint JointSquareWithOneSocket = [] -> Joint {
  return JointBuilder()
      .AddViews(PremadeViews::WhiteSquare)
      .SetOrigin({10, 10})
      .SetSocketConfig(1, 13, 0, 0, false, 0, 90)
      .Build("JointSquareWithOneSocket");
}();

/////////////////////////////////////////////////
inline const Joint JointSquareWithTwoSockets = [] -> Joint {
  return JointBuilder()
      .AddViews(PremadeViews::WhiteSquare)
      .SetOrigin({10, 10})
      .SetSocketConfig(2, 13, 0, 0, false, 0, 90)
      .Build("JointSquareWith2Sockets");
}();

/////////////////////////////////////////////////
inline const Joint JointSquareWithThreeSockets = [] -> Joint {
  return JointBuilder()
      .AddViews(PremadeViews::WhiteSquare)
      .SetOrigin({10, 10})
      .SetSocketConfig(3, 13, 0, 0, false, 0, 90)
      .Build("JointSquareWithThreeSockets");
}();

} // namespace steamrot::parts
