
#include "BaseLogic.h"
#include <iostream>

namespace steamrot {

/////////////////////////////////////////////////
BaseLogic::BaseLogic(const LogicContext logic_context)
    : m_logic_context(logic_context) {
  std::cout << "BaseLogic constructor called." << std::endl;
}

/////////////////////////////////////////////////
void BaseLogic::RunLogic() {

  // [TODO: add in some frequency modifier]
  ProcessLogic();
}

} // namespace steamrot
