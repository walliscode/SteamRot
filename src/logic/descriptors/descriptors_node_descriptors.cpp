/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the free functions for Node Descriptors
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "descriptors_node_descriptors.h"
#include "descriptors_analysis_event_helpers.h"
#include <string>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
NodeDescriptor::NodeDescriptor(std::string name, FnType fn)
    : m_name(std::move(name)), m_fn(std::move(fn)) {}

/////////////////////////////////////////////////
NodeDescriptorResult NodeDescriptor::operator()(const PartGraph &parts,
                                                uint32_t id,
                                                uint32_t depth) const {

  const auto part_it = parts.find(id);

  NodeDescriptorResult result{};
  if (part_it == parts.end()) {
    result = NodeDescriptorResult{false, "incorrect key: part_id=" +
                                             std::to_string(id)};
  } else {
    result = m_fn(parts, id);
  }
  AnalysisEvent eval_event =
      make_node_eval_event(depth, id, m_name, parts, static_cast<bool>(result));

  result.m_trace.push_back(std::move(eval_event));

  return result;
}

/////////////////////////////////////////////////
const NodeDescriptor is_fragment{
    "is_fragment",
    [](const PartGraph &parts, uint32_t id) -> NodeDescriptorResult {
      const auto part_it = parts.find(id);
      if (part_it == parts.end())
        return NodeDescriptorResult{false, "incorrect key: part_id=" +
                                               std::to_string(id)};
      const bool holds =
          std::holds_alternative<FragmentInstance>(part_it->second);
      return NodeDescriptorResult{holds, holds ? "node holds FragmentInstance"
                                               : "node holds JointInstance"};
    }};

/////////////////////////////////////////////////
const NodeDescriptor is_joint{
    "is_joint",
    [](const PartGraph &parts, uint32_t id) -> NodeDescriptorResult {
      const auto part_it = parts.find(id);
      if (part_it == parts.end())
        return NodeDescriptorResult{false, "incorrect key: part_id=" +
                                               std::to_string(id)};
      const bool holds = std::holds_alternative<JointInstance>(part_it->second);
      return NodeDescriptorResult{holds, holds ? "node holds JointInstance"
                                               : "node holds FragmentInstance"};
    }};

/////////////////////////////////////////////////
NodeDescriptor has_exactly_n_edges(size_t n, std::string name) {
  return NodeDescriptor{
      name, [n](const PartGraph &parts, uint32_t id) -> NodeDescriptorResult {
        const auto part_it = parts.find(id);
        if (part_it == parts.end())
          return NodeDescriptorResult{false, "incorrect key: part_id=" +
                                                 std::to_string(id)};
        const size_t count = std::visit(
            [](const auto &inst) -> size_t { return inst.connection_count; },
            part_it->second);
        return NodeDescriptorResult{
            count == n, "connection_count=" + std::to_string(count) +
                            ", expected==" + std::to_string(n)};
      }};
}

/////////////////////////////////////////////////
const NodeDescriptor is_serial = has_exactly_n_edges(2, "is_serial");

/////////////////////////////////////////////////
NodeDescriptor has_minimum_n_edges(size_t n) {
  return NodeDescriptor{
      "has_minimum_n_edges(" + std::to_string(n) + ")",
      [n](const PartGraph &parts, uint32_t id) -> NodeDescriptorResult {
        const auto part_it = parts.find(id);
        if (part_it == parts.end())
          return NodeDescriptorResult{false, "incorrect key: part_id=" +
                                                 std::to_string(id)};
        const size_t count = std::visit(
            [](const auto &inst) -> size_t { return inst.connection_count; },
            part_it->second);
        return NodeDescriptorResult{
            count >= n, "connection_count=" + std::to_string(count) +
                            ", expected>=" + std::to_string(n)};
      }};
}

/////////////////////////////////////////////////
const NodeDescriptor is_branched = has_minimum_n_edges(3);

/////////////////////////////////////////////////
NodeDescriptor has_maximum_n_edges(size_t n) {
  return NodeDescriptor{
      "has_maximum_n_edges(" + std::to_string(n) + ")",
      [n](const PartGraph &parts, uint32_t id) -> NodeDescriptorResult {
        const auto part_it = parts.find(id);
        if (part_it == parts.end())
          return NodeDescriptorResult{false, "incorrect key: part_id=" +
                                                 std::to_string(id)};
        const size_t count = std::visit(
            [](const auto &inst) -> size_t { return inst.connection_count; },
            part_it->second);
        return NodeDescriptorResult{
            count <= n, "connection_count=" + std::to_string(count) +
                            ", expected<=" + std::to_string(n)};
      }};
}

/////////////////////////////////////////////////
const NodeDescriptor is_terminal = has_exactly_n_edges(1, "is_terminal");
} // namespace steamrot::logic::descriptors
