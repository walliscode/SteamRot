/////////////////////////////////////////////////
/// @file
/// @brief Implementation of EngineSnapshotEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EngineSnapshotEqualsMatcher.h"
#include "EventBusEqualsMatcher.h"
#include "SceneCollectionDataEqualsMatcher.h"
#include "conmat.h"
#include <format>
#include <sstream>

namespace steamrot::tests {

/////////////////////////////////////////////////
EngineSnapshotEqualsMatcher::EngineSnapshotEqualsMatcher(
    const EngineSnapshot &expected)
    : m_expected(expected), m_context(std::nullopt) {}

/////////////////////////////////////////////////
EngineSnapshotEqualsMatcher::EngineSnapshotEqualsMatcher(
    const EngineSnapshot &expected, const TestContext &context)
    : m_expected(expected), m_context(context) {}

/////////////////////////////////////////////////
bool EngineSnapshotEqualsMatcher::match(const EngineSnapshot &actual) const {
  std::ostringstream oss;
  bool all_match = true;

  // 1. Compare tick_number (optional)
  if (m_expected.tick_number.has_value()) {
    if (!actual.tick_number.has_value()) {
      oss << "Expected tick_number, but Actual snapshot has none\n";
      all_match = false;
    } else if (actual.tick_number.value() != m_expected.tick_number.value()) {
      oss << std::format("Tick number mismatch: expected {}, got {}\n",
                         m_expected.tick_number.value(),
                         actual.tick_number.value());
      all_match = false;
    }
  }

  // 2. Compare global_event_bus (optional)
  if (m_expected.global_event_bus.has_value()) {
    if (!actual.global_event_bus.has_value()) {
      oss << "Expected global_event_bus, but actual has none\n";
      all_match = false;
    } else {
      EventBusEqualsMatcher bus_matcher =
          m_context.has_value()
              ? EventBusEqualsMatcher(m_expected.global_event_bus.value(),
                                      m_context.value())
              : EventBusEqualsMatcher(m_expected.global_event_bus.value());

      if (!bus_matcher.match(actual.global_event_bus.value())) {
        oss << bus_matcher.describe() << "\n";
        all_match = false;
      }
    }
  }

  // 3. Compare scene_manager_data (optional) - SKIPPED per instructions

  // 4. Compare scene_collection_data (NOT optional - always present)
  SceneCollectionDataEqualsMatcher collection_matcher =
      m_context.has_value()
          ? SceneCollectionDataEqualsMatcher(m_expected.scene_collection_data,
                                             m_context.value())
          : SceneCollectionDataEqualsMatcher(m_expected.scene_collection_data);

  if (!collection_matcher.match(actual.scene_collection_data)) {
    oss << collection_matcher.describe() << "\n";
    all_match = false;
  }

  if (!all_match) {
    m_mismatch_description = oss.str();
  }
  return all_match;
}

/////////////////////////////////////////////////
std::string EngineSnapshotEqualsMatcher::describe() const {
  if (m_mismatch_description.empty()) {
    std::ostringstream oss;
    oss << conmat::Header(conmat::TestPassed() + "EngineSnapshot Match:", 3)
        << "\n";

    // Add context information if available
    if (m_context.has_value()) {
      oss << conmat::Indent(1) << "Test: " << m_context->test_name << "\n";
      oss << conmat::Indent(1)
          << std::format("Tick: {}/{}", m_context->current_tick,
                         m_context->total_ticks)
          << "\n";
    }

    oss << conmat::Indent(1) << "All fields matched successfully\n";

    return oss.str();
  } else {
    std::ostringstream oss;
    oss << conmat::Header(conmat::TestFailed() + "EngineSnapshot Mismatch:", 3)
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
