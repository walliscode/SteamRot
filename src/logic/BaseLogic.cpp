
#include "BaseLogic.h"
#include "actions_generated.h"
#include <iostream>

namespace steamrot {

/////////////////////////////////////////////////
BaseLogic::BaseLogic(const LogicContext logic_context)
    : m_logic_context(logic_context) {
  std::cout << "BaseLogic constructor called." << std::endl;
}

/////////////////////////////////////////////////
void BaseLogic::RunLogic() {

  // reset actions and data
  m_logic_action = ActionNames{0};
  m_logic_data = LogicData();

  // [TODO: add in some frequency modifier]
  ProcessLogic();
}

/////////////////////////////////////////////////
const ActionNames &BaseLogic::GetLogicAction() { return m_logic_action; }

/////////////////////////////////////////////////
void BaseLogic::ResetLogicAction() { m_logic_action = ActionNames_ACTION_NONE; }

/////////////////////////////////////////////////
const LogicData &BaseLogic::GetLogicData() const { return m_logic_data; }
} // namespace steamrot
