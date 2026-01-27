/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for EntityTransportVariant
/// comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EntityTransportEqualsMatcher.h"
#include "EntityMemoryPoolEqualsMatcher.h"
#include "conmat.h"
#include <format>

namespace steamrot::tests {

/////////////////////////////////////////////////
EntityTransportEqualsMatcher::EntityTransportEqualsMatcher(
    const EntityTransportVariant &expected)
    : m_expected(expected), m_context(std::nullopt) {}

/////////////////////////////////////////////////
EntityTransportEqualsMatcher::EntityTransportEqualsMatcher(
    const EntityTransportVariant &expected, const TestContext &context)
    : m_expected(expected), m_context(context) {}

/////////////////////////////////////////////////
bool EntityTransportEqualsMatcher::match(
    const EntityTransportVariant &actual) const {
  m_mismatch_description.clear();
  std::ostringstream oss;

  // 1. Check variant indices match
  if (actual.index() != m_expected.index()) {
    oss << conmat::Indent(1) << conmat::TestFailed() << "Variant type mismatch"
        << "\n";
    oss << conmat::Indent(2) << "actual index: "
        << conmat::Colorize(std::to_string(actual.index()), conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected index: "
        << conmat::Colorize(std::to_string(m_expected.index()),
                            conmat::Color::Blue)
        << "\n";
    m_mismatch_description = oss.str();
    return false;
  }

  // 2. Handle monostate (both empty)
  if (std::holds_alternative<std::monostate>(actual)) {
    return true; // Both empty is a match
  }

  // 3. Handle EntityMemoryPool (PRIMARY CASE for TestEngine)
  if (std::holds_alternative<EntityMemoryPool>(actual)) {
    const auto &actual_pool = std::get<EntityMemoryPool>(actual);
    const auto &expected_pool = std::get<EntityMemoryPool>(m_expected);

    // Create matcher with or without context
    EntityMemoryPoolEqualsMatcher pool_matcher =
        m_context.has_value()
            ? EntityMemoryPoolEqualsMatcher(expected_pool, m_context.value())
            : EntityMemoryPoolEqualsMatcher(expected_pool);

    if (!pool_matcher.match(actual_pool)) {
      m_mismatch_description = pool_matcher.describe();
      return false;
    }
    return true;
  }

  // 4. Handle shared_ptr<EntityMemoryPool>
  if (std::holds_alternative<std::shared_ptr<EntityMemoryPool>>(actual)) {
    auto actual_ptr = std::get<std::shared_ptr<EntityMemoryPool>>(actual);
    auto expected_ptr = std::get<std::shared_ptr<EntityMemoryPool>>(m_expected);

    // Check for null pointers
    if (!actual_ptr && !expected_ptr) {
      return true; // Both null is a match
    }

    if (!actual_ptr || !expected_ptr) {
      oss << conmat::Indent(1) << conmat::TestFailed()
          << "Null pointer in shared_ptr<EntityMemoryPool>" << "\n";
      oss << conmat::Indent(2) << "actual is null: "
          << conmat::Colorize(actual_ptr ? "false" : "true", conmat::Color::Red)
          << "\n";
      oss << conmat::Indent(2) << "expected is null: "
          << conmat::Colorize(expected_ptr ? "false" : "true",
                              conmat::Color::Blue)
          << "\n";
      m_mismatch_description = oss.str();
      return false;
    }

    // Both are non-null, compare the dereferenced pools
    EntityMemoryPoolEqualsMatcher pool_matcher =
        m_context.has_value()
            ? EntityMemoryPoolEqualsMatcher(*expected_ptr, m_context.value())
            : EntityMemoryPoolEqualsMatcher(*expected_ptr);

    if (!pool_matcher.match(*actual_ptr)) {
      m_mismatch_description = pool_matcher.describe();
      return false;
    }
    return true;
  }

  // 5. Handle unique_ptr<IEntityImporter>
  if (std::holds_alternative<std::unique_ptr<IEntityImporter>>(actual)) {
    // IEntityImporter cannot be meaningfully compared in snapshots
    // This should not occur in TestEngine snapshots
    oss << conmat::Indent(1) << conmat::TestFailed()
        << "IEntityImporter comparison not supported in snapshot testing"
        << "\n";
    m_mismatch_description = oss.str();
    return false;
  }

  // Should never reach here
  oss << conmat::Indent(1) << conmat::TestFailed()
      << "Unknown variant type encountered" << "\n";
  m_mismatch_description = oss.str();
  return false;
}

/////////////////////////////////////////////////
std::string EntityTransportEqualsMatcher::describe() const {
  if (m_mismatch_description.empty()) {
    std::ostringstream oss;
    oss << conmat::Header(conmat::TestPassed() + "EntityTransport Match:", 3)
        << "\n";

    // Add context information if available
    if (m_context.has_value()) {
      oss << conmat::Indent(1) << "Test: " << m_context->test_name << "\n";
      oss << conmat::Indent(1)
          << std::format("Tick: {}/{}", m_context->current_tick,
                         m_context->total_ticks)
          << "\n";
    }
    return oss.str();
  } else {
    std::ostringstream oss;
    oss << conmat::Header(conmat::TestFailed() + "EntityTransport Mismatch:", 3)
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
