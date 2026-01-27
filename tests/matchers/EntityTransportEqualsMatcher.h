/////////////////////////////////////////////////
/// @file
/// @brief Declaration of Catch2 matcher for EntityTransportVariant comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EntityMemoryPoolEqualsMatcher.h"
#include "IEntityImporter.h"
#include "SceneData.h"
#include "containers.h"
#include "test_context.h"
#include <catch2/matchers/catch_matchers.hpp>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <variant>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Type alias for EntityTransportVariant
/////////////////////////////////////////////////
using EntityTransportVariant =
    std::variant<std::monostate, std::unique_ptr<IEntityImporter>,
                 std::shared_ptr<EntityMemoryPool>, EntityMemoryPool>;

/////////////////////////////////////////////////
/// @class EntityTransportEqualsMatcher
/// @brief Custom Catch2 matcher for comparing EntityTransportVariant objects
///
/// This matcher handles all variant types in entity_transport:
/// - std::monostate (empty)
/// - EntityMemoryPool (value type)
/// - std::shared_ptr<EntityMemoryPool>
/// - std::unique_ptr<IEntityImporter> (not supported in snapshots)
/////////////////////////////////////////////////
class EntityTransportEqualsMatcher
    : public Catch::Matchers::MatcherBase<EntityTransportVariant> {
private:
  const EntityTransportVariant &m_expected;
  mutable std::string m_mismatch_description;
  std::optional<TestContext> m_context; // Context for enriched error messages

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for EntityTransportEqualsMatcher
  ///
  /// @param expected Expected EntityTransportVariant object to compare against
  /////////////////////////////////////////////////
  explicit EntityTransportEqualsMatcher(const EntityTransportVariant &expected);

  /////////////////////////////////////////////////
  /// @brief Constructor with TestContext
  ///
  /// @param expected Expected EntityTransportVariant object to compare against
  /// @param context Test context with metadata and tick information
  /////////////////////////////////////////////////
  EntityTransportEqualsMatcher(const EntityTransportVariant &expected,
                               const TestContext &context);

  /////////////////////////////////////////////////
  /// @brief Match method to compare actual EntityTransportVariant with expected
  ///
  /// @param actual EntityTransportVariant object to compare
  /// @return Whether the actual EntityTransportVariant matches the expected
  /////////////////////////////////////////////////
  bool match(const EntityTransportVariant &actual) const override;

  /////////////////////////////////////////////////
  /// @brief Describe method to provide detailed mismatch description
  ///
  /// @return Formatted description string
  /////////////////////////////////////////////////
  std::string describe() const override;
};

/////////////////////////////////////////////////
/// @brief Helper function to create EntityTransportEqualsMatcher
///
/// @param expected Expected EntityTransportVariant object
/// @return EntityTransportEqualsMatcher instance
/////////////////////////////////////////////////
inline EntityTransportEqualsMatcher
EqualsEntityTransport(const EntityTransportVariant &expected) {
  return EntityTransportEqualsMatcher(expected);
}

/////////////////////////////////////////////////
/// @brief Helper function to create EntityTransportEqualsMatcher with context
///
/// @param expected Expected EntityTransportVariant object
/// @param context Test context with metadata and tick information
/// @return EntityTransportEqualsMatcher instance
/////////////////////////////////////////////////
inline EntityTransportEqualsMatcher
EqualsEntityTransport(const EntityTransportVariant &expected,
                      const TestContext &context) {
  return EntityTransportEqualsMatcher(expected, context);
}

} // namespace steamrot::tests
