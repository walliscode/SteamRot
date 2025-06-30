/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CMachinaForm component
/////////////////////////////////////////////////

#include "CMachinaForm.h"
#include "containers.h"

namespace steamrot {

/////////////////////////////////////////////////
const std::string &CMachinaForm::Name() {
  static const std::string name = "CMachinaForm";
  return name;
}

/////////////////////////////////////////////////
const size_t CMachinaForm::GetComponentRegisterIndex() const {

  // Get the index of this component in the component register
  static constexpr size_t index = components::containers::TupleTypeIndex<
      CMachinaForm, components::containers::ComponentRegister>;
  return index;
}
} // namespace steamrot
