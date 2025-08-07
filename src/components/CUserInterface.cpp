////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "CUserInterface.h"

#include "containers.h"

namespace steamrot {

/////////////////////////////////////////////////
size_t CUserInterface::GetComponentRegisterIndex() const {

  // Get the index of this component in the component register
  static constexpr size_t index =
      TupleTypeIndex<CUserInterface, ComponentRegister>;
  return index;
}
} // namespace steamrot
