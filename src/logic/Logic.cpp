
#include "Logic.h"

namespace steamrot {

/////////////////////////////////////////////////
Logic::Logic(const LogicContext logic_context)
    : m_logic_context(logic_context) {}

/////////////////////////////////////////////////
void Logic::RunLogic() {

  // [TODO: add in some frequency modifier]
  ProcessLogic();
}

} // namespace steamrot
