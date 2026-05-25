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
#include <string>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
ChainDescriptor is_serial_chain_of_min_length(size_t min_length) {
  if (min_length == 0)
    min_length = 1;
  return ChainDescriptorBuilder{}
      .WhileIsTrueForN(is_serial, min_length)
      .Then(is_terminal)
      .Build("is_serial_chain(min_length=" + std::to_string(min_length) + ")");
}

/////////////////////////////////////////////////
const ChainDescriptor is_serial_chain = is_serial_chain_of_min_length(1);
} // namespace steamrot::logic::descriptors
