/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for SceneInfoEqualsMatcher
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneInfoEqualsMatcher.h"
#include "SceneInfo.h"
#include "conmat.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SceneInfoEqualsMatcher works correctly",
          "[unit][SceneInfo][matcher]") {
  // Define expected SceneInfo
  steamrot::SceneInfo expected;
  expected.type = steamrot::SceneType::TEST;
  uuids::uuid id = uuids::uuid_system_generator{}();
  expected.id = id;

  // intialize actual SceneInfo
  steamrot::SceneInfo actual;

  SECTION("Matcher detects differences in scene type") {

    actual.type = steamrot::SceneType::TITLE;
    actual.id = id;
    REQUIRE_THAT(actual, !steamrot::tests::EqualsSceneInfo(expected));
  }
  SECTION("Matcher detects differences in scene ID") {

    actual.type = steamrot::SceneType::TEST;
    actual.id = uuids::uuid_system_generator{}();
    REQUIRE_THAT(actual, !steamrot::tests::EqualsSceneInfo(expected));
  }
  SECTION("Matcher detects equality") {
    actual.type = steamrot::SceneType::TEST;
    actual.id = id;
    REQUIRE_THAT(actual, steamrot::tests::EqualsSceneInfo(expected));
  }
}

TEST_CASE("SceneInfoEqualsMatcher describe is as expected on success",
          "[unit][SceneInfo][matcher]") {
  steamrot::SceneInfo expected;
  expected.type = steamrot::SceneType::TEST;
  uuids::uuid id = uuids::uuid_system_generator{}();
  expected.id = id;

  steamrot::SceneInfo actual;
  actual.type = steamrot::SceneType::TEST;
  actual.id = id;

  auto matcher = steamrot::tests::EqualsSceneInfo(expected);
  matcher.match(actual);

  std::ostringstream oss;
  oss << conmat::Header(conmat::TestPassed() + "SceneInfo Match:", 3) << "\n";

  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("SceneInfoEqualsMatcher describe is as expected on scene type failure",
          "[unit][SceneInfo][matcher]") {
  steamrot::SceneInfo expected;
  expected.type = steamrot::SceneType::TEST;
  uuids::uuid id = uuids::uuid_system_generator{}();
  expected.id = id;

  steamrot::SceneInfo actual;
  actual.type = steamrot::SceneType::TITLE;
  actual.id = id;

  auto matcher = steamrot::tests::EqualsSceneInfo(expected);
  matcher.match(actual);

  std::ostringstream oss;
  oss << conmat::Header(conmat::TestFailed() + "SceneInfo Mismatch:", 3)
      << "\n";
  oss << conmat::Indent(1) << conmat::TestFailed()
      << "Scene type differs:" << "\n";
  oss << conmat::Indent(2)
      << "actual: " << conmat::Colorize("TITLE", conmat::Color::Red) << "\n";
  oss << conmat::Indent(2)
      << "expected: " << conmat::Colorize("TEST", conmat::Color::Blue) << "\n";

  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("SceneInfoEqualsMatcher describe is as expected on scene ID failure",
          "[unit][SceneInfo][matcher]") {
  steamrot::SceneInfo expected;
  expected.type = steamrot::SceneType::TEST;
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  expected.id = id1;

  steamrot::SceneInfo actual;
  actual.type = steamrot::SceneType::TEST;
  uuids::uuid id2 = uuids::uuid_system_generator{}();
  actual.id = id2;

  auto matcher = steamrot::tests::EqualsSceneInfo(expected);
  matcher.match(actual);

  std::ostringstream oss;
  oss << conmat::Header(conmat::TestFailed() + "SceneInfo Mismatch:", 3)
      << "\n";
  oss << conmat::Indent(1) << conmat::TestFailed()
      << "Scene ID differs:" << "\n";
  oss << conmat::Indent(2)
      << "actual: " << conmat::Colorize(uuids::to_string(id2), conmat::Color::Red) << "\n";
  oss << conmat::Indent(2)
      << "expected: " << conmat::Colorize(uuids::to_string(id1), conmat::Color::Blue) << "\n";

  REQUIRE(matcher.describe() == oss.str());
}

TEST_CASE("SceneInfoEqualsMatcher describe is as expected on both failures",
          "[unit][SceneInfo][matcher]") {
  steamrot::SceneInfo expected;
  expected.type = steamrot::SceneType::TEST;
  uuids::uuid id1 = uuids::uuid_system_generator{}();
  expected.id = id1;

  steamrot::SceneInfo actual;
  actual.type = steamrot::SceneType::CRAFTING;
  uuids::uuid id2 = uuids::uuid_system_generator{}();
  actual.id = id2;

  auto matcher = steamrot::tests::EqualsSceneInfo(expected);
  matcher.match(actual);

  std::ostringstream oss;
  oss << conmat::Header(conmat::TestFailed() + "SceneInfo Mismatch:", 3)
      << "\n";
  oss << conmat::Indent(1) << conmat::TestFailed()
      << "Scene type differs:" << "\n";
  oss << conmat::Indent(2)
      << "actual: " << conmat::Colorize("CRAFTING", conmat::Color::Red) << "\n";
  oss << conmat::Indent(2)
      << "expected: " << conmat::Colorize("TEST", conmat::Color::Blue) << "\n";
  oss << conmat::Indent(1) << conmat::TestFailed()
      << "Scene ID differs:" << "\n";
  oss << conmat::Indent(2)
      << "actual: " << conmat::Colorize(uuids::to_string(id2), conmat::Color::Red) << "\n";
  oss << conmat::Indent(2)
      << "expected: " << conmat::Colorize(uuids::to_string(id1), conmat::Color::Blue) << "\n";

  REQUIRE(matcher.describe() == oss.str());
}
