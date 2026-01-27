/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Catch2 matcher for EntityMemoryPool comparison
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EntityMemoryPoolEqualsMatcher.h"
#include "CGrimoireMachinaEqualsMatcher.h"
#include "CMachinaFormEqualsMatcher.h"
#include "CMetaEqualsMatcher.h"
#include "CUIStateEqualsMatcher.h"
#include "CUserInterfaceEqualsMatcher.h"
#include "conmat.h"
#include "containers.h"
#include "entity_memory.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

/////////////////////////////////////////////////
template <typename TComponent>
void EntityMemoryPoolEqualsMatcher::CompareComponentVector(
    const std::vector<TComponent> &actual_vec,
    const std::vector<TComponent> &expected_vec,
    std::ostringstream &oss) const {

  FAIL("Component type does not have a matcher set up");
}

/////////////////////////////////////////////////
template <>
void EntityMemoryPoolEqualsMatcher::CompareComponentVector(
    const std::vector<CMeta> &actual_vec,
    const std::vector<CMeta> &expected_vec, std::ostringstream &oss) const {

  for (size_t i = 0; i < expected_vec.size(); ++i) {
    CMetaEqualsMatcher matcher(expected_vec[i], i);
    if (!matcher.match(actual_vec[i])) {
      // pass through the mismatch description from CMetaEqualsMatcher
      oss << matcher.describe() << "\n";

      // switch flag to false
      do_components_match = false;
    }
  }
}

/////////////////////////////////////////////////
template <>
void EntityMemoryPoolEqualsMatcher::CompareComponentVector(
    const std::vector<CUserInterface> &actual_vec,
    const std::vector<CUserInterface> &expected_vec,
    std::ostringstream &oss) const {

  for (size_t i = 0; i < expected_vec.size(); ++i) {
    CUserInterfaceEqualsMatcher matcher(expected_vec[i], i);
    if (!matcher.match(actual_vec[i])) {
      // pass through the mismatch description from
      // CUserInterfaceEqualsMatcher
      oss << matcher.describe() << "\n";
      // switch flag to false
      do_components_match = false;
    }
  }
}

/////////////////////////////////////////////////
template <>
void EntityMemoryPoolEqualsMatcher::CompareComponentVector(
    const std::vector<CMachinaForm> &actual_vec,
    const std::vector<CMachinaForm> &expected_vec,
    std::ostringstream &oss) const {
  for (size_t i = 0; i < expected_vec.size(); ++i) {
    CMachinaFormEqualsMatcher matcher(expected_vec[i], i);
    if (!matcher.match(actual_vec[i])) {

      // pass through the mismatch description from CMachinaFormEqualsMatcher
      oss << matcher.describe() << "\n";
      // switch flag to false
      do_components_match = false;
    }
  }
}

/////////////////////////////////////////////////
template <>
void EntityMemoryPoolEqualsMatcher::CompareComponentVector(
    const std::vector<CGrimoireMachina> &actual_vec,
    const std::vector<CGrimoireMachina> &expected_vec,
    std::ostringstream &oss) const {
  for (size_t i = 0; i < expected_vec.size(); ++i) {
    CGrimoireMachinaEqualsMatcher matcher(expected_vec[i], i);
    if (!matcher.match(actual_vec[i])) {
      // pass through the mismatch description from
      // CGrimoireMachinaEqualsMatcher
      oss << matcher.describe() << "\n";
      // switch flag to false
      do_components_match = false;
    }
  }
}

/////////////////////////////////////////////////
template <>
void EntityMemoryPoolEqualsMatcher::CompareComponentVector(
    const std::vector<CUIState> &actual_vec,
    const std::vector<CUIState> &expected_vec, std::ostringstream &oss) const {

  for (size_t i = 0; i < expected_vec.size(); ++i) {
    CUIStateEqualsMatcher matcher(expected_vec[i], i);
    if (!matcher.match(actual_vec[i])) {

      // pass through the mismatch description from CUIStateEqualsMatcher
      oss << matcher.describe() << "\n";

      // switch flag to false
      do_components_match = false;
    }
  }
}

/////////////////////////////////////////////////
template <std::size_t... Is>
void EntityMemoryPoolEqualsMatcher::CompareAllComponentVectorsImpl(
    const EntityMemoryPool &actual_pool, const EntityMemoryPool &expected_pool,
    std::ostringstream &oss, std::index_sequence<Is...>) const {

  // Expand the parameter pack to compare each component vector
  (CompareComponentVector<std::tuple_element_t<Is, ComponentRegister>>(
       std::get<Is>(actual_pool), std::get<Is>(expected_pool), oss),
   ...);
}

/////////////////////////////////////////////////
void EntityMemoryPoolEqualsMatcher::CompareAllComponentVectors(
    const EntityMemoryPool &actual_pool, const EntityMemoryPool &expected_pool,
    std::ostringstream &oss) const {

  // Generate an index sequence for the number of components
  CompareAllComponentVectorsImpl(
      actual_pool, expected_pool, oss,
      std::make_index_sequence<ComponentRegisterSize>{});
}

/////////////////////////////////////////////////
EntityMemoryPoolEqualsMatcher::EntityMemoryPoolEqualsMatcher(
    const EntityMemoryPool &expected)
    : m_expected(expected), m_test_metadata(""), m_context(std::nullopt) {}

/////////////////////////////////////////////////
EntityMemoryPoolEqualsMatcher::EntityMemoryPoolEqualsMatcher(
    const EntityMemoryPool &expected, const std::string &test_metadata)
    : m_expected(expected), m_test_metadata(test_metadata),
      m_context(std::nullopt) {}

/////////////////////////////////////////////////
EntityMemoryPoolEqualsMatcher::EntityMemoryPoolEqualsMatcher(
    const EntityMemoryPool &expected, const TestContext &context)
    : m_expected(expected), m_test_metadata(""), m_context(context) {}

/////////////////////////////////////////////////
bool EntityMemoryPoolEqualsMatcher::match(
    const EntityMemoryPool &actual) const {
  m_mismatch_description.clear();
  std::ostringstream oss;

  // Early assertion: check pool sizes first
  size_t actual_size = entity::memory::GetMemoryPoolSize(actual);
  size_t expected_size = entity::memory::GetMemoryPoolSize(m_expected);

  if (actual_size != expected_size) {

    oss << conmat::Divider("-", 40) << "\n";
    oss << conmat::TestFailed() << "EntityMemoryPool Size Mismatch:" << "\n";
    oss << "\t"
        << "actual size = " << conmat::Colorize(actual_size, conmat::Color::Red)
        << "\n";
    oss << "\t"
        << "expected size = "
        << conmat::Colorize(expected_size, conmat::Color::Blue) << "\n";
    oss << conmat::Divider("-", 40) << "\n";

    m_mismatch_description = oss.str();
    return false;
  }

  // Compare each component vector in the pools
  CompareAllComponentVectors(actual, m_expected, oss);
  if (!do_components_match) {

    m_mismatch_description = oss.str();
    return false;
  }
  return true;
}

/////////////////////////////////////////////////
std::string EntityMemoryPoolEqualsMatcher::describe() const {

  if (m_mismatch_description.empty()) {

    std::ostringstream oss;
    oss << conmat::Divider("=", 40) << "\n";
    oss << conmat::Colorize("[PASSED] ", conmat::Color::Green)
        << "EntityMemoryPool Match" << "\n";
    oss << conmat::Divider("=", 40) << "\n";
    return oss.str();
  } else {

    std::ostringstream oss;
    oss << conmat::Divider("=", 40) << "\n";
    oss << conmat::Colorize("[FAILED] ", conmat::Color::Red)
        << "EntityMemoryPool Match " << "\n";
    oss << m_mismatch_description << "\n";
    oss << conmat::Divider("=", 40) << "\n";

    return oss.str();
  }
}

} // namespace steamrot::tests
