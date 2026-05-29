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
#include "descriptors_node_descriptors.h"

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
const MachinaArchetype grab =
    MachinaArchetypeBuilder<GrabResult>{}
        .Then(lift_to_chain(is_joint), &GrabResult::anchor)

        .Build("Grab");

} // namespace steamrot::logic::descriptors
