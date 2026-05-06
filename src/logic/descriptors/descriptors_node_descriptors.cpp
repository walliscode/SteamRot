/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the free functions for Node Descriptors
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "descriptors_node_descriptors.h"
#include <string>

namespace steamrot::logic::descriptors {

/////////////////////////////////////////////////
NodeDescriptor::NodeDescriptor(std::string name, FnType fn)
    : m_name(std::move(name)), m_fn(std::move(fn)) {}

/////////////////////////////////////////////////
NodeDescriptorResult NodeDescriptor::operator()(const PartGraph &parts,
                                                uint32_t id,
                                                uint32_t depth) const {
  AnalysisEvent eval_event{};
  eval_event.kind = TraceEventKind::NodeEval;
  eval_event.depth = depth;
  eval_event.part_id = id;
  eval_event.predicate_name = m_name;

  auto result = m_fn(parts, id);

  AnalysisEvent result_event{};
  result_event.kind = TraceEventKind::NodeResult;
  result_event.depth = depth;
  result_event.part_id = id;
  result_event.predicate_name = m_name;
  result_event.result = static_cast<bool>(result);
  result_event.reason = result.m_reason;

  result.m_trace.push_back(std::move(eval_event));
  result.m_trace.push_back(std::move(result_event));

  return result;
}

/////////////////////////////////////////////////
const NodeDescriptor is_fragment{
    "is_fragment",
    [](const PartGraph &parts, uint32_t id) -> NodeDescriptorResult {
      const bool holds = std::holds_alternative<FragmentInstance>(parts.at(id));
      return NodeDescriptorResult{
          holds,
          holds ? "node holds FragmentInstance" : "node holds JointInstance"};
    }};

/////////////////////////////////////////////////
const NodeDescriptor is_joint{
    "is_joint",
    [](const PartGraph &parts, uint32_t id) -> NodeDescriptorResult {
      const bool holds = std::holds_alternative<JointInstance>(parts.at(id));
      return NodeDescriptorResult{
          holds,
          holds ? "node holds JointInstance" : "node holds FragmentInstance"};
    }};

/////////////////////////////////////////////////
NodeDescriptor has_exactly_n_edges(size_t n) {
  return NodeDescriptor{
      "has_exactly_n_edges(" + std::to_string(n) + ")",
      [n](const PartGraph &parts, uint32_t id) -> NodeDescriptorResult {
        const size_t count = std::visit(
            [](const auto &inst) -> size_t { return inst.connection_count; },
            parts.at(id));
        return NodeDescriptorResult{
            count == n,
            "connection_count=" + std::to_string(count) +
                ", expected==" + std::to_string(n)};
      }};
}

/////////////////////////////////////////////////
const NodeDescriptor is_serial = has_exactly_n_edges(2);

/////////////////////////////////////////////////
NodeDescriptor has_minimum_n_edges(size_t n) {
  return NodeDescriptor{
      "has_minimum_n_edges(" + std::to_string(n) + ")",
      [n](const PartGraph &parts, uint32_t id) -> NodeDescriptorResult {
        const size_t count = std::visit(
            [](const auto &inst) -> size_t { return inst.connection_count; },
            parts.at(id));
        return NodeDescriptorResult{
            count >= n,
            "connection_count=" + std::to_string(count) +
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
        const size_t count = std::visit(
            [](const auto &inst) -> size_t { return inst.connection_count; },
            parts.at(id));
        return NodeDescriptorResult{
            count <= n,
            "connection_count=" + std::to_string(count) +
                ", expected<=" + std::to_string(n)};
      }};
}

/////////////////////////////////////////////////
const NodeDescriptor is_terminal = has_maximum_n_edges(1);
} // namespace steamrot::logic::descriptors
