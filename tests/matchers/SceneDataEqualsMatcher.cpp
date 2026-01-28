/////////////////////////////////////////////////
/// @file
/// @brief Implementation of SceneDataEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneDataEqualsMatcher.h"
#include "EntityTransportEqualsMatcher.h"
#include "SceneInfoEqualsMatcher.h"
#include "conmat.h"
#include <format>
#include <sstream>

namespace steamrot::tests {

/////////////////////////////////////////////////
SceneDataEqualsMatcher::SceneDataEqualsMatcher(const SceneData &expected)
    : m_expected(expected), m_context(std::nullopt) {}

/////////////////////////////////////////////////
SceneDataEqualsMatcher::SceneDataEqualsMatcher(const SceneData &expected,
                                               const TestContext &context)
    : m_expected(expected), m_context(context) {}

/////////////////////////////////////////////////
bool SceneDataEqualsMatcher::match(const SceneData &actual) const {
  std::ostringstream oss;
  bool all_match = true;

  // Compare scene_info
  SceneInfoEqualsMatcher info_matcher(m_expected.scene_info);
  if (!info_matcher.match(actual.scene_info)) {
    oss << info_matcher.describe() << "\n";
    all_match = false;
  }

  // Compare entity_transport (CRITICAL)
  EntityTransportEqualsMatcher transport_matcher =
      m_context.has_value()
          ? EntityTransportEqualsMatcher(m_expected.entity_transport,
                                         m_context.value())
          : EntityTransportEqualsMatcher(m_expected.entity_transport);

  if (!transport_matcher.match(actual.entity_transport)) {
    oss << transport_matcher.describe() << "\n";
    all_match = false;
  }

  if (!all_match) {
    m_mismatch_description = oss.str();
  }
  return all_match;
}

/////////////////////////////////////////////////
std::string SceneDataEqualsMatcher::describe() const {
  if (m_mismatch_description.empty()) {
    std::ostringstream oss;
    oss << conmat::Header(conmat::TestPassed() + "SceneData Match:", 3) << "\n";

    // Add context information if available
    if (m_context.has_value()) {
      oss << conmat::Indent(1) << "Test: " << m_context->test_name << "\n";
      oss << conmat::Indent(1)
          << std::format("Tick: {}/{}", m_context->current_tick,
                         m_context->total_ticks)
          << "\n";
    }

    // Add scene UUID for multi-scene scenarios
    oss << conmat::Indent(1)
        << "Scene UUID: " << uuids::to_string(m_expected.scene_info.id) << "\n";

    return oss.str();
  } else {
    std::ostringstream oss;
    oss << conmat::Header(conmat::TestFailed() + "SceneData Mismatch:", 3)
        << "\n";

    // Add context information if available
    if (m_context.has_value()) {
      oss << conmat::Indent(1) << "Test: " << m_context->test_name << "\n";
      oss << conmat::Indent(1)
          << std::format("Tick: {}/{}", m_context->current_tick,
                         m_context->total_ticks)
          << "\n";
    }

    // Add scene UUID for multi-scene scenarios
    oss << conmat::Indent(1)
        << "Scene UUID: " << uuids::to_string(m_expected.scene_info.id) << "\n";

    oss << m_mismatch_description;
    return oss.str();
  }
}

} // namespace steamrot::tests
