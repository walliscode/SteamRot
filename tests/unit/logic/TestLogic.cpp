/////////////////////////////////////////////////
/// @file
/// @brief Implementation of TestLogic class for testing Logic objects.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestLogic.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
TestLogic::TestLogic(const SceneContext &scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void TestLogic::ProcessLogic() {
  // This function can be left empty for testing purposes, or you can add
  // simple logic here to verify that ProcessLogic is being called correctly.
}
} // namespace steamrot::tests
