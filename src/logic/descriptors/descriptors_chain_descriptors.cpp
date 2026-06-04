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
const ChainDescriptor is_serial_chain = ChainDescriptorBuilder{}
                                            .WhileIsTrue(is_serial)
                                            .Then(is_terminal)
                                            .Build("is_serial_chain");

/////////////////////////////////////////////////
ChainDescriptor is_serial_chain_with_minimum_length_n(uint32_t n) {
  return ChainDescriptorBuilder{}
      .WhileIsTrueForMinimumN(is_serial, n)
      .Then(is_terminal)
      .Build("is_serial_chain_with_minimum_length_" + std::to_string(n));
}

/////////////////////////////////////////////////
const ChainDescriptor is_serial_chain_with_minimum_length_2 =
    is_serial_chain_with_minimum_length_n(2);

/////////////////////////////////////////////////
const ChainDescriptor is_joint_chain =
    ChainDescriptorBuilder{}.Then(is_joint).Build(is_joint.GetName());
} // namespace steamrot::logic::descriptors
