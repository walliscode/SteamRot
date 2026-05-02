/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the ChainDescriptorBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ChainDescriptorBuilder.h"
#include "descriptors_node_descriptors.h"
#include <catch2/catch_test_macros.hpp>

using namespace steamrot::logic::descriptors;

// Helper function to create a simple NodeDescriptor for testing
steamrot::logic::descriptors::NodeDescriptor make_test_predicate() {

  return [](const steamrot::MachinaFormScaffold & /*scaffold*/,
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
