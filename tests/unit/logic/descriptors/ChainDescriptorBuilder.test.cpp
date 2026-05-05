/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the ChainDescriptorBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ChainDescriptorBuilder.h"
#include "MachinaFormScaffold.h"
#include "descriptors_node_descriptors.h"
#include "part_library.h"
#include <catch2/catch_test_macros.hpp>

using namespace steamrot::logic::descriptors;

// Helper function to create a simple NodeDescriptor for testing
steamrot::logic::descriptors::NodeDescriptor make_test_predicate() {

  return [](const steamrot::PartGraph & /*parts*/,
            uint32_t /*id*/)
             -> steamrot::logic::descriptors::NodeDescriptorResult {
    return steamrot::logic::descriptors::NodeDescriptorResult{true};
  };
}

TEST_CASE("ChainDescriptor constructs correctly with StartWith and Then",
          "[ChainDescriptorBuilder]") {

  ChainDescriptorBuilder builder;
  const auto &steps = builder.GetSteps();
  REQUIRE(steps.size() == 0);
}

TEST_CASE("ChainDescriptorBuilder::Then tests", "[ChainDescriptorBuilder]") {
  // arrange
  ChainDescriptorBuilder builder;
  const auto &steps = builder.GetSteps();
  REQUIRE(steps.size() == 0);
  NodeDescriptor test_predicate = make_test_predicate();
  SECTION("Then adds a step with the correct predicate and kind") {
    // act
    builder.Then(test_predicate);
    // assert
    REQUIRE(steps.size() == 1);
    const ChainStep &step = steps[0];
    REQUIRE(step.kind == ChainStepKind::Sequence);
    REQUIRE(step.predicate.target_type() == test_predicate.target_type());
  }
}

TEST_CASE("ChainDescriptorBuilder::WhileIsTrue tests",
          "[ChainDescriptorBuilder]") {
  // arrange
  ChainDescriptorBuilder builder;
  const auto &steps = builder.GetSteps();
  REQUIRE(steps.size() == 0);
  NodeDescriptor test_predicate = make_test_predicate();
  SECTION("WhileIsTrue adds a step with the correct predicate and kind") {
    // act
    builder.WhileIsTrue(test_predicate);
    // assert
    REQUIRE(steps.size() == 1);
    const ChainStep &step = steps[0];
    REQUIRE(step.kind == ChainStepKind::WhileIsTrue);
    REQUIRE(step.predicate.target_type() == test_predicate.target_type());
  }
}

TEST_CASE("ChainDescriptorBuilder supports multiple steps",
          "[ChainDescriptorBuilder]") {
  // arrange
  ChainDescriptorBuilder builder;
  const auto &steps = builder.GetSteps();
  REQUIRE(steps.size() == 0);
  NodeDescriptor test_predicate = make_test_predicate();
  // act
  builder.Then(test_predicate).WhileIsTrue(test_predicate).Then(test_predicate);
  // assert
  REQUIRE(steps.size() == 3);
  REQUIRE(steps[0].kind == ChainStepKind::Sequence);
  REQUIRE(steps[1].kind == ChainStepKind::WhileIsTrue);
  REQUIRE(steps[2].kind == ChainStepKind::Sequence);
}

TEST_CASE("ChainDescriptorBuilder::Validate tests",
          "[ChainDescriptorBuilder]") {
  // arrange
  ChainDescriptorBuilder builder;
  NodeDescriptor test_predicate = make_test_predicate();

  SECTION("Validate returns empty string for valid builder") {
    // act
    std::string error_message = builder.Validate();
    // assert
    REQUIRE(error_message.empty());
  }

  SECTION("Validate returns error message if builder is modified after End()") {
    // act
    builder.Then(test_predicate);
    std::string error_message_before_end = builder.Validate();
    auto build_result = builder.Build();
    std::string error_message_after_end = builder.Validate();
    // assert
    REQUIRE(error_message_before_end.empty());
    REQUIRE(error_message_after_end ==
            "Cannot modify builder after Build() has been called. ");
  }
}

TEST_CASE("ChainDescriptorBuilder::Build tests", "[ChainDescriptorBuilder]") {
  // arrange
  ChainDescriptorBuilder builder;
  NodeDescriptor test_predicate = make_test_predicate();
  SECTION("Build returns a valid descriptor for a simple builder") {
    // act
    builder.Then(test_predicate);
    auto build_result = builder.Build();
    // assert
    REQUIRE(build_result.has_value());
    ChainDescriptor descriptor = build_result.value();
  }
  SECTION("Build prevents further modification of the builder") {
    // act
    builder.Then(test_predicate);
    // call first Build() to set the flag
    auto build_result = builder.Build();
    REQUIRE(build_result.has_value());

    // call Build() again to check that it prevents modification
    build_result = builder.Build();
    REQUIRE_FALSE(build_result.has_value());
    REQUIRE(build_result.error() ==
            "Cannot modify builder after Build() has been called. ");
  }
}

TEST_CASE("ChainDescriptorBuilder::dfs WhileIsTrue semantics",
          "[ChainDescriptorBuilder]") {

  steamrot::tests::TestPartLibrary lib =
      steamrot::tests::TestPartLibrary::Create();
  steamrot::tests::PartLibraryBuilder part_builder{lib};

  SECTION("WhileIsTrue rejects when predicate fails on the first node "
          "(minimum n=1)") {
    // An isolated fragment has connection_count 0, so is_serial returns false.
    steamrot::MachinaFormScaffold scaffold =
        part_builder.MakeScaffoldWithParts({"fragment_one_socket"}, {});
    const uint32_t start_id = scaffold.parts.begin()->first;

    ChainDescriptorBuilder chain_builder;
    auto build_result =
        chain_builder.WhileIsTrue(descriptors::is_serial).Build();
    REQUIRE(build_result.has_value());

    ChainDescriptorResult result =
        build_result.value()(scaffold.parts, start_id);
    REQUIRE_FALSE(result);
  }

  SECTION("WhileIsTrue succeeds at dead end when no more steps remain") {
    // An isolated fragment has connection_count 0, so is_terminal returns true.
    // The predicate passes, there are no neighbours, and no more steps: success.
    steamrot::MachinaFormScaffold scaffold =
        part_builder.MakeScaffoldWithParts({"fragment_one_socket"}, {});
    const uint32_t start_id = scaffold.parts.begin()->first;

    ChainDescriptorBuilder chain_builder;
    auto build_result =
        chain_builder.WhileIsTrue(descriptors::is_terminal).Build();
    REQUIRE(build_result.has_value());

    ChainDescriptorResult result =
        build_result.value()(scaffold.parts, start_id);
    REQUIRE(result);
  }

  SECTION("WhileIsTrue fails at dead end when more steps remain") {
    // Isolated terminal fragment: predicate passes but the following Then step
    // can never be reached because there are no neighbours.
    steamrot::MachinaFormScaffold scaffold =
        part_builder.MakeScaffoldWithParts({"fragment_one_socket"}, {});
    const uint32_t start_id = scaffold.parts.begin()->first;

    ChainDescriptorBuilder chain_builder;
    auto build_result = chain_builder.WhileIsTrue(descriptors::is_terminal)
                            .Then(descriptors::is_serial)
                            .Build();
    REQUIRE(build_result.has_value());

    ChainDescriptorResult result =
        build_result.value()(scaffold.parts, start_id);
    REQUIRE_FALSE(result);
  }

  SECTION("WhileIsTrue consumes multiple matching nodes then succeeds at dead "
          "end") {
    // fragment_one_socket — joint_one_socket: both have connection_count 1,
    // so is_terminal returns true for each. WhileIsTrue(is_terminal) starts
    // at the fragment, consumes it, walks to the joint (still terminal), and
    // then hits a dead end (all neighbours visited) with no remaining steps:
    // success.
    steamrot::tests::ScaffoldResult scaffold_result =
        part_builder.MakeConnectedScaffold({"fragment_one_socket"},
                                           {"joint_one_socket"}, {{0, 0, 1, 0}});
    const uint32_t start_id = scaffold_result.part_ids[0];

    ChainDescriptorBuilder chain_builder;
    auto build_result =
        chain_builder.WhileIsTrue(descriptors::is_terminal).Build();
    REQUIRE(build_result.has_value());

    ChainDescriptorResult result =
        build_result.value()(scaffold_result.scaffold.parts, start_id);
    REQUIRE(result);
  }

  SECTION("WhileIsTrue passes the failing node to the next step") {
    // LinearChain: frag0(terminal) — joint0(serial) — frag1(terminal).
    // WhileIsTrue(is_terminal) starts at frag0, consumes it, then fails on
    // joint0 (serial). It passes joint0 to WhileIsTrue(is_serial), which
    // consumes joint0, then fails on frag1 (terminal), passing frag1 to
    // steps_end: success.
    const steamrot::MachinaFormScaffold &scaffold =
        lib.scaffold_scenarios.at(steamrot::tests::ScaffoldScenario::LinearChain);

    ChainDescriptorBuilder chain_builder;
    auto build_result = chain_builder.WhileIsTrue(descriptors::is_terminal)
                            .WhileIsTrue(descriptors::is_serial)
                            .Build();
    REQUIRE(build_result.has_value());

    // The chain should match starting from at least one of the terminal nodes.
    bool any_match = false;
    for (const auto &[id, ignored] : scaffold.parts) {
      if (build_result.value()(scaffold.parts, id))
        any_match = true;
    }
    REQUIRE(any_match);
  }
}
