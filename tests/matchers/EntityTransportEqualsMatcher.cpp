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

  // Helper lambda to extract EntityMemoryPool pointer from variant
  auto extract_pool = [](const EntityTransportVariant &variant)
      -> const EntityMemoryPool * {
    if (std::holds_alternative<EntityMemoryPool>(variant)) {
      return &std::get<EntityMemoryPool>(variant);
    } else if (std::holds_alternative<std::shared_ptr<EntityMemoryPool>>(
                   variant)) {
      auto ptr = std::get<std::shared_ptr<EntityMemoryPool>>(variant);
      return ptr.get(); // Returns nullptr if ptr is null
    }
    return nullptr;
  };

  // 1. Handle monostate (both empty)
  if (std::holds_alternative<std::monostate>(actual) &&
      std::holds_alternative<std::monostate>(m_expected)) {
    return true; // Both empty is a match
  }

  // 2. Handle IEntityImporter (not supported in snapshots)
  if (std::holds_alternative<std::unique_ptr<IEntityImporter>>(actual) ||
      std::holds_alternative<std::unique_ptr<IEntityImporter>>(m_expected)) {
    oss << conmat::Indent(1) << conmat::TestFailed()
        << "IEntityImporter comparison not supported in snapshot testing"
        << "\n";
    m_mismatch_description = oss.str();
    return false;
  }

  // 3. Extract EntityMemoryPool pointers from both variants
  const EntityMemoryPool *actual_pool = extract_pool(actual);
  const EntityMemoryPool *expected_pool = extract_pool(m_expected);

  // 4. Check if one is monostate and the other has a pool
  if ((actual_pool == nullptr) != (expected_pool == nullptr)) {
    oss << conmat::Indent(1) << conmat::TestFailed()
        << "One variant is empty/null, the other contains EntityMemoryPool"
        << "\n";
    oss << conmat::Indent(2) << "actual has pool: "
        << conmat::Colorize(actual_pool ? "true" : "false", conmat::Color::Red)
        << "\n";
    oss << conmat::Indent(2) << "expected has pool: "
        << conmat::Colorize(expected_pool ? "true" : "false",
                            conmat::Color::Blue)
        << "\n";
    m_mismatch_description = oss.str();
    return false;
  }

  // 5. Both are null/empty - already handled by monostate check, but added for
  // completeness
  if (actual_pool == nullptr && expected_pool == nullptr) {
    return true;
  }

  // 6. Both have EntityMemoryPool - compare them
  EntityMemoryPoolEqualsMatcher pool_matcher =
      m_context.has_value()
          ? EntityMemoryPoolEqualsMatcher(*expected_pool, m_context.value())
          : EntityMemoryPoolEqualsMatcher(*expected_pool);

  if (!pool_matcher.match(*actual_pool)) {
    m_mismatch_description = pool_matcher.describe();
    return false;
  }
  return true;
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
