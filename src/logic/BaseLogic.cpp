
#include "BaseLogic.h"
#include <iostream>

namespace steamrot {

BaseLogic::BaseLogic(const LogicContext logic_context)
    : m_logic_context(logic_context) {
  std::cout << "BaseLogic constructor called." << std::endl;
}

void BaseLogic::RunLogic() {

  if (m_cycle_count == m_update_frequency) {
    ProcessLogic();

    // reset the cycle count to 1, make sure this comes at the end of the if
    // block
    m_cycle_count = 1;
  } else {
    // increment the cycle count only if we are not at the update frequency
    m_cycle_count++;
  }
}
} // namespace steamrot
