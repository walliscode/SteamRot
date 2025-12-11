/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CGrimoireMachina component
/////////////////////////////////////////////////

#include "CGrimoireMachina.h"
#include "containers.h"
namespace steamrot {

/////////////////////////////////////////////////
size_t CGrimoireMachina::GetComponentRegisterIndex() const {

  // Get the index of this component in the component register
  static constexpr size_t index =
      TupleTypeIndex<CGrimoireMachina, ComponentRegister>;
  return index;
}

} // namespace steamrot
