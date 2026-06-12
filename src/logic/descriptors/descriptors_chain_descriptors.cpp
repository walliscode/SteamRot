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
const ChainDescriptor &is_serial_chain() {
  static const ChainDescriptor instance = ChainDescriptorBuilder{}
                                              .WhileIsTrue(is_serial())
                                              .Then(is_terminal())
                                              .Build("is_serial_chain");
  return instance;
}

/////////////////////////////////////////////////
ChainDescriptor is_serial_chain_with_minimum_length_n(uint32_t n) {
  return ChainDescriptorBuilder{}
      // we make sure to account for the terminal node by using n-1
      .WhileIsTrueForMinimumN(is_serial(), n - 1)
      .Then(is_terminal())
      .Build("is_serial_chain_with_minimum_length_" + std::to_string(n));
}

/////////////////////////////////////////////////
const ChainDescriptor &is_serial_chain_with_minimum_length_3() {
  static const ChainDescriptor instance =
      is_serial_chain_with_minimum_length_n(3);
  return instance;
}

/////////////////////////////////////////////////
const ChainDescriptor &is_joint_chain() {
  static const ChainDescriptor instance =
      ChainDescriptorBuilder{}.Then(is_joint()).Build(is_joint().GetName());
  return instance;
}
} // namespace steamrot::logic::descriptors
