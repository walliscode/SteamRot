/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for AnchorBranchWorkflow example orchestration.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AnchorBranchWorkflow.h"
#include "PartGraphBuilder.h"
#include "descriptors_chain_descriptors.h"
#include "descriptors_node_descriptors.h"
#include "part_graph_library.h"
#include <catch2/catch_test_macros.hpp>
#include <vector>

namespace descriptors = steamrot::logic::descriptors;
namespace tests = steamrot::tests;

TEST_CASE("AnchorBranchWorkflow executes anchor/branch/cardinality assignment",
          "[unit][logic][descriptors][workflow]") {
  // anchor: j0 (three sockets)
  // branch A (pass): j1 -> j2 -> j3 -> f0
  // branch B (pass): j4 -> j5 -> j6 -> f1
  // branch C (fail, too short): j7 -> j8 -> f2
  const tests::PartGraphPackage pkg =
      tests::PartGraphBuilder{}
          .AddJoint(tests::JointNames::ThreeSockets, "j0")
          .AddJoint(tests::JointNames::TwoSockets, "j1")
          .AddJoint(tests::JointNames::TwoSockets, "j2")
          .AddJoint(tests::JointNames::TwoSockets, "j3")
          .AddJoint(tests::JointNames::TwoSockets, "j4")
          .AddJoint(tests::JointNames::TwoSockets, "j5")
          .AddJoint(tests::JointNames::TwoSockets, "j6")
          .AddJoint(tests::JointNames::TwoSockets, "j7")
          .AddJoint(tests::JointNames::TwoSockets, "j8")
          .AddFragment(tests::FragmentNames::OneSocket, "f0")
          .AddFragment(tests::FragmentNames::OneSocket, "f1")
          .AddFragment(tests::FragmentNames::OneSocket, "f2")
          .ConnectUnchecked("j0", 0, "j1", 0)
          .ConnectUnchecked("j1", 1, "j2", 0)
          .ConnectUnchecked("j2", 1, "j3", 0)
          .Connect("f0", 0, "j3", 1)
          .ConnectUnchecked("j0", 1, "j4", 0)
          .ConnectUnchecked("j4", 1, "j5", 0)
          .ConnectUnchecked("j5", 1, "j6", 0)
          .Connect("f1", 0, "j6", 1)
          .ConnectUnchecked("j0", 2, "j7", 0)
          .ConnectUnchecked("j7", 1, "j8", 0)
          .Connect("f2", 0, "j8", 1)
          .Build();

  const auto &ids = pkg.id_to_part_graph_id;
  const descriptors::AnchorBranchWorkflow workflow{
      {.m_anchor_rule = descriptors::is_joint,
       .m_branch_rule = descriptors::is_serial_chain_of_min_length(3),
       .m_minimum_valid_branches = 2,
       .m_assignment_count = 2}};

  const descriptors::AnchorBranchWorkflowResult result =
      workflow.Execute(pkg.part_graph, ids.at("j0"));

  REQUIRE(result);
  REQUIRE(result.m_anchor_result);
  REQUIRE(result.m_anchor_id == ids.at("j0"));
  REQUIRE(result.m_branch_evaluations.size() == 3);
  REQUIRE(result.m_assigned_branches.size() == 2);
  REQUIRE(result.m_assigned_branches[0] == std::vector<uint32_t>{
                                            ids.at("j1"), ids.at("j2"),
                                            ids.at("j3"), ids.at("f0")});
  REQUIRE(result.m_assigned_branches[1] == std::vector<uint32_t>{
                                            ids.at("j4"), ids.at("j5"),
                                            ids.at("j6"), ids.at("f1")});
  REQUIRE_FALSE(result.m_branch_evaluations[0].m_reason.size());
  REQUIRE_FALSE(result.m_branch_evaluations[1].m_reason.size());
  REQUIRE(result.m_branch_evaluations[2].m_reason == "branch_rule_failed");
}

TEST_CASE("AnchorBranchWorkflow fails early when anchor rule fails",
          "[unit][logic][descriptors][workflow]") {
  const descriptors::AnchorBranchWorkflow workflow{
      {.m_anchor_rule = descriptors::is_joint,
       .m_branch_rule = descriptors::is_serial_chain_of_min_length(3),
       .m_minimum_valid_branches = 2,
       .m_assignment_count = 2}};

  const descriptors::AnchorBranchWorkflowResult result =
      workflow.Execute(tests::pair.part_graph, 0);

  REQUIRE_FALSE(result);
  REQUIRE_FALSE(result.m_anchor_result);
  REQUIRE(result.m_branch_evaluations.empty());
  REQUIRE(result.m_assigned_branches.empty());
}
