////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "CUIState.h"

#include "containers.h"

namespace steamrot {

////////////////////////////////////////////////////////////
size_t CUIState::GetComponentRegisterIndex() const {

  // Get the index of this component in the component register
  static constexpr size_t index = TupleTypeIndex<CUIState, ComponentRegister>;
  return index;
}

} // namespace steamrot
