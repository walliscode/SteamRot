////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "CMeta.h"
#include "containers.h"

namespace steamrot {
////////////////////////////////////////////////////////////
const std::string &CMeta::Name() {

  static const std::string name = "CMeta";
  return name;
}

////////////////////////////////////////////////////////////
void CMeta::Configure(const json &data) {
  // configure variables
  m_entity_active = data["m_entity_active"];
}

const size_t CMeta::GetComponentRegisterIndex() const {

  // Get the index of the CMeta component in the component register
  static constexpr size_t index = components::containers::TupleTypeIndex<
      CMeta, components::containers::ComponentRegister>;

  return index;
}
} // namespace steamrot
