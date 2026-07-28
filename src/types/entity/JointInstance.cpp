/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the JointInstance class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "JointInstance.h"

namespace steamrot {

/////////////////////////////////////////////////
void JointInstance::PositionSockets(
    const JointSocketPositioningStrategy positioning_strategy) {
  switch (positioning_strategy) {
  case JointSocketPositioningStrategy::EvenSpread:
    break;
  case JointSocketPositioningStrategy::MaximizeDistance:
    maximise_joint_socket_spread(sockets, part.socket_pivot,
                                 part.socket_config);
    break;
  default:
    break;
  }
}

} // namespace steamrot
