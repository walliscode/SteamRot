/////////////////////////////////////////////////
/// @file
/// @brief Implementation of SceneCollectionDataEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneCollectionDataEqualsMatcher.h"
#include "SceneDataEqualsMatcher.h"
#include "conmat.h"
#include <format>
#include <sstream>

namespace steamrot::tests {

/////////////////////////////////////////////////
SceneCollectionDataEqualsMatcher::SceneCollectionDataEqualsMatcher(
    const SceneCollectionData &expected)
    : m_expected(expected), m_context(std::nullopt) {}

/////////////////////////////////////////////////
SceneCollectionDataEqualsMatcher::SceneCollectionDataEqualsMatcher(
    const SceneCollectionData &expected, const TestContext &context)
    : m_expected(expected), m_context(context) {}

/////////////////////////////////////////////////
bool SceneCollectionDataEqualsMatcher::match(
    const SceneCollectionData &actual) const {
  // 1. Check size
  if (actual.size() != m_expected.size()) {
    m_mismatch_description =
        std::format("Scene collection size mismatch: expected {}, got {}",
                    m_expected.size(), actual.size());
    return false;
  }

  // 2. Compare each SceneData
  std::ostringstream oss;
  bool all_match = true;

  for (size_t i = 0; i < m_expected.size(); ++i) {
    SceneDataEqualsMatcher scene_matcher =
        m_context.has_value()
            ? SceneDataEqualsMatcher(m_expected[i], m_context.value())
            : SceneDataEqualsMatcher(m_expected[i]);

    if (!scene_matcher.match(actual[i])) {
      oss << "Scene [" << i << "] " << uuids::to_string(m_expected[i].scene_info.id)
          << ":\n"
          << scene_matcher.describe() << "\n";
      all_match = false;
    }
  }

  if (!all_match) {
    m_mismatch_description = oss.str();
  }
  return all_match;
}

/////////////////////////////////////////////////
std::string SceneCollectionDataEqualsMatcher::describe() const {
  if (m_mismatch_description.empty()) {
    std::ostringstream oss;
    oss << conmat::Header(conmat::TestPassed() + "SceneCollection Match:", 3)
        << "\n";

    // Add context information if available
    if (m_context.has_value()) {
      oss << conmat::Indent(1) << "Test: " << m_context->test_name << "\n";
      oss << conmat::Indent(1)
          << std::format("Tick: {}/{}", m_context->current_tick,
                         m_context->total_ticks)
          << "\n";
    }

    oss << conmat::Indent(1)
        << std::format("Collection size: {}", m_expected.size()) << "\n";

    return oss.str();
  } else {
    std::ostringstream oss;
    oss << conmat::Header(conmat::TestFailed() + "SceneCollection Mismatch:",
                          3)
        << "\n";

    // Add context information if available
    if (m_context.has_value()) {
      oss << conmat::Indent(1) << "Test: " << m_context->test_name << "\n";
      oss << conmat::Indent(1)
          << std::format("Tick: {}/{}", m_context->current_tick,
                         m_context->total_ticks)
          << "\n";
    }

    oss << m_mismatch_description;
    return oss.str();
  }
}

} // namespace steamrot::tests
