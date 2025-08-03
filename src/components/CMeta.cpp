////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "CMeta.h"
#include "containers.h"

namespace steamrot {

size_t CMeta::GetComponentRegisterIndex() const {

  // Get the index of the CMeta component in the component register
  static constexpr size_t index = components::containers::TupleTypeIndex<
      CMeta, components::containers::ComponentRegister>;

  return index;
}
} // namespace steamrot
