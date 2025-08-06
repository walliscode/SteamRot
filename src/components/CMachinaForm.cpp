/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CMachinaForm component
/////////////////////////////////////////////////

#include "CMachinaForm.h"
#include "containers.h"

namespace steamrot {

/////////////////////////////////////////////////
size_t CMachinaForm::GetComponentRegisterIndex() const {

  // Get the index of this component in the component register
  static constexpr size_t index =
      TupleTypeIndex<CMachinaForm, ComponentRegister>;
  return index;
}
} // namespace steamrot
