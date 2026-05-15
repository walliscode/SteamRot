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

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
const ChainDescriptor is_serial_chain =
    ChainDescriptorBuilder{}.Build("is_serial_chain");
} // namespace steamrot::logic::descriptors
