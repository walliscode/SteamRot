/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the MachinaArchetype instances
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "descriptors_machina_archetypes.h"
#include "DescriptorResult.h"
#include "MachinaArchetypeBuilder.h"
#include "descriptors_chain_descriptors.h"

namespace steamrot::logic::descriptors::MA {

/////////////////////////////////////////////////
const MachinaArchetype &Grab() {
  static const MachinaArchetype instance =
      MachinaArchetypeBuilder<GrabResult>{}
          .Then(is_joint_chain(), &GrabResult::anchor)
          .AtLeastNOf(is_serial_chain_with_minimum_length_3(), 2,
                      &GrabResult::arms)
          .Build("Grab");
  return instance;
}

} // namespace steamrot::logic::descriptors::MA
