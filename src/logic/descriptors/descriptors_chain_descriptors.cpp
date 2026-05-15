/////////////////////////////////////////////////
/// @file
/// @brief Definition of the ChainDescriptor instances declared in
/// descriptors_chain_descriptors.h
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "descriptors_chain_descriptors.h"
#include "ChainDescriptorBuilder.h"
#include "descriptors_node_descriptors.h"

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
const ChainDescriptor is_serial_chain =
    ChainDescriptorBuilder{}.WhileIsTrue(is_serial).Build("is_serial_chain");
} // namespace steamrot::logic::descriptors
