/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the ChainDescriptorBuilder class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ChainDescriptorBuilder.h"
#include <catch2/catch_test_macros.hpp>

using namespace steamrot::logic::descriptors;

// Helper function to create a simple NodeDescriptor for testing
steamrot::logic::descriptors::NodeDescriptor make_test_predicate() {

  return [](const steamrot::PartGraph & /*parts*/, uint32_t /*id*/)
             -> steamrot::logic::descriptors::NodeDescriptorResult {
    return steamrot::logic::descriptors::NodeDescriptorResult{true};
  };
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
