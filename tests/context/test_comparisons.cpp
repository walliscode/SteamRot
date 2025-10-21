/////////////////////////////////////////////////
/// @file
/// @brief Implementation of reusable test comparison functions
/////////////////////////////////////////////////

#include "test_comparisons.h"
#include "CUserInterface.h"
#include "CGrimoireMachina.h"
#include "CUIState.h"
#include "emp_helpers.h"
#include <sstream>

namespace steamrot::tests {

/////////////////////////////////////////////////
ComparisonResult CompareEntityMemoryPools(const EntityMemoryPool &actual,
                                          const EntityMemoryPool &expected) {
  std::vector<std::string> differences;

  // Compare pool sizes
  size_t actual_size = std::get<0>(actual).size();
  size_t expected_size = std::get<0>(expected).size();

  if (actual_size != expected_size) {
    std::ostringstream oss;
    oss << "Pool sizes differ: actual=" << actual_size
        << ", expected=" << expected_size;
    differences.push_back(oss.str());
  }

  // Compare component counts for each component type
  size_t min_size = std::min(actual_size, expected_size);

  // Helper to compare component vectors
  auto compare_vector_sizes = [&](auto &actual_vec, auto &expected_vec,
                                   const std::string &component_name) {
    if (actual_vec.size() != expected_vec.size()) {
      std::ostringstream oss;
      oss << component_name << " vector sizes differ: actual="
          << actual_vec.size() << ", expected=" << expected_vec.size();
      differences.push_back(oss.str());
    }
  };

  std::apply(
      [&](auto &...actual_components) {
        std::apply(
            [&](auto &...expected_components) {
              size_t idx = 0;
              (compare_vector_sizes(
                   actual_components, expected_components,
                   "Component" + std::to_string(idx++)),
               ...);
            },
            expected);
      },
      actual);

  if (differences.empty()) {
    return ComparisonResult(true);
  }
  return CreateFailedComparison(differences);
}

/////////////////////////////////////////////////
ComparisonResult CompareEventBuses(const EventBus &actual,
                                   const EventBus &expected) {
  std::vector<std::string> differences;

  if (actual.size() != expected.size()) {
    std::ostringstream oss;
    oss << "EventBus sizes differ: actual=" << actual.size()
        << ", expected=" << expected.size();
    differences.push_back(oss.str());
  }

  size_t min_size = std::min(actual.size(), expected.size());
  for (size_t i = 0; i < min_size; ++i) {
    if (actual[i].m_event_type != expected[i].m_event_type) {
      std::ostringstream oss;
      oss << "Event type at index " << i << " differs: actual="
          << static_cast<uint64_t>(actual[i].m_event_type) << ", expected="
          << static_cast<uint64_t>(expected[i].m_event_type);
      differences.push_back(oss.str());
    }
  }

  if (differences.empty()) {
    return ComparisonResult(true);
  }
  return CreateFailedComparison(differences);
}

/////////////////////////////////////////////////
ComparisonResult CompareArchetypeManagers(const ArchetypeManager &actual,
                                          const ArchetypeManager &expected) {
  std::vector<std::string> differences;

  const auto &actual_archetypes = actual.GetArchetypes();
  const auto &expected_archetypes = expected.GetArchetypes();

  if (actual_archetypes.size() != expected_archetypes.size()) {
    std::ostringstream oss;
    oss << "Number of archetypes differ: actual="
        << actual_archetypes.size()
        << ", expected=" << expected_archetypes.size();
    differences.push_back(oss.str());
  }

  // Compare each archetype
  for (const auto &[archetype_id, expected_archetype] : expected_archetypes) {
    auto actual_it = actual_archetypes.find(archetype_id);
    if (actual_it == actual_archetypes.end()) {
      std::ostringstream oss;
      oss << "Missing archetype with ID: " << archetype_id;
      differences.push_back(oss.str());
      continue;
    }

    const auto &actual_archetype = actual_it->second;
    if (actual_archetype.size() != expected_archetype.size()) {
      std::ostringstream oss;
      oss << "Entity count for archetype " << archetype_id
          << " differs: actual=" << actual_archetype.size()
          << ", expected=" << expected_archetype.size();
      differences.push_back(oss.str());
    }
  }

  if (differences.empty()) {
    return ComparisonResult(true);
  }
  return CreateFailedComparison(differences);
}

/////////////////////////////////////////////////
ComparisonResult CreateFailedComparison(
    const std::vector<std::string> &differences) {
  std::ostringstream oss;
  oss << "Comparison failed with " << differences.size()
      << " difference(s):";
  for (const auto &diff : differences) {
    oss << "\n  - " << diff;
  }

  ComparisonResult result(false, oss.str());
  result.differences = differences;
  return result;
}

/////////////////////////////////////////////////
// Template specializations for component comparisons
/////////////////////////////////////////////////

template <>
ComparisonResult CompareComponents<CUserInterface>(
    const CUserInterface &actual, const CUserInterface &expected) {
  std::vector<std::string> differences;

  if (actual.m_active != expected.m_active) {
    std::ostringstream oss;
    oss << "m_active differs: actual=" << actual.m_active
        << ", expected=" << expected.m_active;
    differences.push_back(oss.str());
  }

  if (actual.m_ui_element_type != expected.m_ui_element_type) {
    std::ostringstream oss;
    oss << "m_ui_element_type differs: actual="
        << static_cast<int>(actual.m_ui_element_type) << ", expected="
        << static_cast<int>(expected.m_ui_element_type);
    differences.push_back(oss.str());
  }

  if (differences.empty()) {
    return ComparisonResult(true);
  }
  return CreateFailedComparison(differences);
}

template <>
ComparisonResult CompareComponents<CGrimoireMachina>(
    const CGrimoireMachina &actual, const CGrimoireMachina &expected) {
  std::vector<std::string> differences;

  if (actual.m_active != expected.m_active) {
    std::ostringstream oss;
    oss << "m_active differs: actual=" << actual.m_active
        << ", expected=" << expected.m_active;
    differences.push_back(oss.str());
  }

  if (differences.empty()) {
    return ComparisonResult(true);
  }
  return CreateFailedComparison(differences);
}

template <>
ComparisonResult CompareComponents<CUIState>(
    const CUIState &actual, const CUIState &expected) {
  std::vector<std::string> differences;

  if (actual.m_active != expected.m_active) {
    std::ostringstream oss;
    oss << "m_active differs: actual=" << actual.m_active
        << ", expected=" << expected.m_active;
    differences.push_back(oss.str());
  }

  if (actual.m_is_hovered != expected.m_is_hovered) {
    std::ostringstream oss;
    oss << "m_is_hovered differs: actual=" << actual.m_is_hovered
        << ", expected=" << expected.m_is_hovered;
    differences.push_back(oss.str());
  }

  if (differences.empty()) {
    return ComparisonResult(true);
  }
  return CreateFailedComparison(differences);
}

/////////////////////////////////////////////////
// Template implementations for inline functions
/////////////////////////////////////////////////

template <typename TComponent>
ComparisonResult CompareComponentActivation(const EntityMemoryPool &pool,
                                            size_t entity_count,
                                            bool expected_active) {
  std::vector<std::string> differences;

  for (size_t i = 0; i < entity_count; ++i) {
    const TComponent &component =
        emp_helpers::GetComponent<TComponent>(i, pool);
    if (component.m_active != expected_active) {
      std::ostringstream oss;
      oss << "Entity " << i << " component activation differs: actual="
          << component.m_active << ", expected=" << expected_active;
      differences.push_back(oss.str());
    }
  }

  if (differences.empty()) {
    return ComparisonResult(true);
  }
  return CreateFailedComparison(differences);
}

// Explicit instantiations for common component types
template ComparisonResult
CompareComponentActivation<CUserInterface>(const EntityMemoryPool &pool,
                                           size_t entity_count,
                                           bool expected_active);

template ComparisonResult
CompareComponentActivation<CGrimoireMachina>(const EntityMemoryPool &pool,
                                             size_t entity_count,
                                             bool expected_active);

template ComparisonResult
CompareComponentActivation<CUIState>(const EntityMemoryPool &pool,
                                     size_t entity_count,
                                     bool expected_active);

template <typename TComponent>
ComparisonResult
CompareComponentsForEntities(const EntityMemoryPool &actual,
                             const EntityMemoryPool &expected,
                             const std::vector<size_t> &entity_ids) {
  std::vector<std::string> differences;

  for (size_t entity_id : entity_ids) {
    const TComponent &actual_component =
        emp_helpers::GetComponent<TComponent>(entity_id, actual);
    const TComponent &expected_component =
        emp_helpers::GetComponent<TComponent>(entity_id, expected);

    ComparisonResult component_result =
        CompareComponents(actual_component, expected_component);
    if (!component_result.passed) {
      std::ostringstream oss;
      oss << "Entity " << entity_id << " component differs: "
          << component_result.message;
      differences.push_back(oss.str());
    }
  }

  if (differences.empty()) {
    return ComparisonResult(true);
  }
  return CreateFailedComparison(differences);
}

// Explicit instantiations for common component types
template ComparisonResult CompareComponentsForEntities<CUserInterface>(
    const EntityMemoryPool &actual, const EntityMemoryPool &expected,
    const std::vector<size_t> &entity_ids);

template ComparisonResult CompareComponentsForEntities<CGrimoireMachina>(
    const EntityMemoryPool &actual, const EntityMemoryPool &expected,
    const std::vector<size_t> &entity_ids);

template ComparisonResult CompareComponentsForEntities<CUIState>(
    const EntityMemoryPool &actual, const EntityMemoryPool &expected,
    const std::vector<size_t> &entity_ids);

} // namespace steamrot::tests
