/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CGrimoireMachina component
/////////////////////////////////////////////////

#include "CGrimoireMachina.h"
#include "containers.h"
namespace steamrot {

/////////////////////////////////////////////////
const size_t CGrimoireMachina::GetComponentRegisterIndex() const {

  // Get the index of this component in the component register
  static constexpr size_t index = components::containers::TupleTypeIndex<
      CGrimoireMachina, components::containers::ComponentRegister>;
  return index;
}

/////////////////////////////////////////////////
const std::string &CGrimoireMachina::Name() {
  static const std::string name = "CGrimoireMachina";
  return name;
}
} // namespace steamrot
