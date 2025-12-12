/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersFragmentDataProvider
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersFragmentDataProvider.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersFragmentDataProvider is constructed correctly",
          "[unit][FlatbuffersFragmentDataProvider]") {

  steamrot::FlatbuffersFragmentDataProvider provider;
  REQUIRE_NOTHROW(provider);
}

TEST_CASE("FlatbuffersFragmentDataProvider::LoadFragment returns error for "
          "non-existent fragment",
          "[unit][FlatbuffersFragmentDataProvider]") {

  steamrot::FlatbuffersFragmentDataProvider provider;
  auto result = provider.LoadFragment("non_existent_fragment");

  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("FlatbuffersFragmentDataProvider::LoadFragment loads valid fragment",
          "[unit][FlatbuffersFragmentDataProvider]") {

  steamrot::FlatbuffersFragmentDataProvider provider;
  auto result = provider.LoadFragment("valid_fragment");

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &fragment = result.value();
  REQUIRE(fragment.m_name == "valid_fragment");
  REQUIRE(!fragment.m_overlays.empty());
  REQUIRE(!fragment.m_sockets.empty());
}

TEST_CASE("FlatbuffersFragmentDataProvider::LoadAllFragments returns error for "
          "non-existent fragments",
          "[unit][FlatbuffersFragmentDataProvider]") {

  steamrot::FlatbuffersFragmentDataProvider provider;
  std::vector<std::string> fragment_names = {"non_existent_fragment"};
  auto result = provider.LoadAllFragments(fragment_names);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE(
    "FlatbuffersFragmentDataProvider::LoadAllFragments loads valid fragments",
    "[unit][FlatbuffersFragmentDataProvider]") {

  steamrot::FlatbuffersFragmentDataProvider provider;
  std::vector<std::string> fragment_names = {"valid_fragment"};
  auto result = provider.LoadAllFragments(fragment_names);

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &fragments = result.value();
  REQUIRE(fragments.size() == 1);
  REQUIRE(fragments.contains("valid_fragment"));

  const auto &fragment = fragments.at("valid_fragment");
  REQUIRE(fragment.m_name == "valid_fragment");
}

TEST_CASE("FlatbuffersFragmentDataProvider::LoadAllFragments handles empty "
          "vector",
          "[unit][FlatbuffersFragmentDataProvider]") {

  steamrot::FlatbuffersFragmentDataProvider provider;
  std::vector<std::string> fragment_names = {};
  auto result = provider.LoadAllFragments(fragment_names);

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &fragments = result.value();
  REQUIRE(fragments.empty());
}

TEST_CASE("FlatbuffersFragmentDataProvider validates fragment structure",
          "[unit][FlatbuffersFragmentDataProvider]") {

  steamrot::FlatbuffersFragmentDataProvider provider;
  auto result = provider.LoadFragment("valid_fragment");

  REQUIRE(result.has_value());

  const auto &fragment = result.value();
  // Verify basic fragment structure
  REQUIRE(!fragment.m_name.empty());
  REQUIRE(fragment.m_overlays.contains(
      steamrot::ViewDirection::ViewDirection_FRONT));

  // Verify socket data
  REQUIRE(fragment.m_sockets.size() == 1);
  REQUIRE(fragment.m_sockets[0].x == 5.0f);
  REQUIRE(fragment.m_sockets[0].y == 7.0f);
}
