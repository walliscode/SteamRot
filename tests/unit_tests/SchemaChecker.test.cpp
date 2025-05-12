#include "SchemaChecker.h"
#include "Logger.h"
#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace steamrot {
TEST_CASE("Schema is not an array", "[data_check]") {

  // register global logger for testing
  Logger test_logger = Logger("global_logger");

  // test top level being an array (it should be an object)
  json test_json = json::array();
  REQUIRE_THROWS(SchemaChecker(test_json));
}

TEST_CASE("Schema errors on incorrrect field names", "[data_check]") {

  // register global logger for testing

  Logger test_logger = Logger("global_logger");

  // test first object having incorrect fieldname

  json test_json = json::object();
  test_json["incorrect_field"] = "value";
  REQUIRE_THROWS(SchemaChecker(test_json));
}

TEST_CASE("Schema has correct field names", "[data_check]") {

  // register global logger for testing

  Logger test_logger = Logger("global_logger");

  // test first object having correct fieldname
  json test_json = json::object();
  test_json["fieldname"] = "value";
  test_json["type"] = "int";

  REQUIRE_NOTHROW(SchemaChecker(test_json));
}
} // namespace steamrot
